#include "../utils/HttpServer.h"
#include <memory>
#include <QDataStream>
#include <QThreadPool>
#include <QDateTime>
#include <QFile>
#include <QDir>

QMap<int, QString> HttpServer::HttpWebResponce::defaultStatusMessages = generateDefaultStatusMessages();
const QString HttpServer::Router::ALL_METHODS = "ALL";




HttpServer::HttpWebResponce HttpServer::default404Handler(HttpWebRequest &request){
    HttpWebResponce resp = request.responce(404);
    resp.setText("Cannot  "+request.method_+" "+request.route_+"/");
    return resp;
}


HttpServer::HttpWebResponce HttpServer::loggerMiddlware(HttpServer::HttpWebRequest &request, HttpServer::Handler next){
    qDebug() << QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate) << request.method_ << request.url_;
    HttpServer::HttpWebResponce responce = next(request);
    qDebug() << QDateTime::currentDateTime().toTimeSpec(Qt::OffsetFromUTC).toString(Qt::ISODate) << request.method_ << request.url_
             << "[" << responce.statusCode_ << "]";
    return responce;
}
HttpServer::HttpWebResponce HttpServer::errorCatcherMiddlware(HttpServer::HttpWebRequest &request, HttpServer::Handler next){
    try{
        return next(request);

    }catch(const std::exception& exception){
        HttpServer::HttpWebResponce resp = request.responce(500);
        resp.setText(QDateTime::currentDateTime().toString() + "\n" + exception.what());
        qDebug() << exception.what();
        return resp;

    }catch(...){
        HttpServer::HttpWebResponce resp = request.responce(520);
        resp.setText(QDateTime::currentDateTime().toString() + "\n" + "не перехватываемая стандартными средствами ошибка");

        return resp;
    }
}










HttpServer::ClientProcess::ClientProcess(qintptr descriptor, HttpServer *server):QRunnable(), descriptor_(descriptor), server_(server){


}

void HttpServer::ClientProcess::run(){

    clientSocket_ = std::make_unique<QTcpSocket>();
    clientSocket_->setSocketDescriptor(descriptor_);
    clientSocket_->waitForReadyRead();

    QTcpSocket * socket = &(*clientSocket_);
    HttpServer::HttpWebRequest request;
    bool parsed_ok=false;

    try{
       request.parse(socket);
       parsed_ok=true;
    }catch(const std::exception& exception){
        // учитывая, что трэйсбек в плюсах нихрена не кросплатформенный: за информативность эксепшена сорян
        // а ещё есть ощущение, что сквозь
        HttpServer::HttpWebResponce responce = request.responce(400);
        responce.setText(QDateTime::currentDateTime().toString() + "\n" + exception.what());
        responce.send(socket);
    }catch(...){
        HttpServer::HttpWebResponce responce = request.responce(418); //первый случай в истории, когда этот код подходит
        responce.setText(QDateTime::currentDateTime().toString() + "\n" + "при парсинге получена ошибка, которую даже не удалось перехватить(");
        responce.send(socket);
    }

    if(parsed_ok){

        QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > > target = server_->router_.getHandler(request.method_, request.route_.split("/"));
        HttpServer::RestContainer restContainer(&request, target.first, target.second);

        HttpServer::HttpWebResponce responce = restContainer.execute(); //если падает обработчик, то хендлера нет

        responce.send();
    }


    clientSocket_->waitForBytesWritten();
    clientSocket_->disconnectFromHost();
    clientSocket_->close();
}





HttpServer::HttpServer(unsigned short port, bool addDefaultHandlers, unsigned short maxThreads, QObject *parent)
    :QTcpServer(parent),threadPool_(this),port_(port){

    threadPool_.setMaxThreadCount(maxThreads);
    if(addDefaultHandlers)
    {
        router_.setDefaultHandler(HttpServer::Router::ALL_METHODS, std::make_shared<HttpServer::Handler>(HttpServer::default404Handler));
        router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(loggerMiddlware));
        router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(errorCatcherMiddlware));

    }
}

HttpServer::~HttpServer(){
    stop();
}

void HttpServer::start(){
    if (!listen(QHostAddress::Any, port_)) {
        qDebug() <<  tr("Unable to start the server: %1.").arg(errorString());
    } else {
        qDebug() << tr("listen http://localhost:%1").arg(port_);
    }
}

void HttpServer::stop(){
    close();
}

void HttpServer::incomingConnection(qintptr socketDescriptor){

    threadPool_.start(
                new ClientProcess(socketDescriptor, this)
                );
}



void HttpServer::HttpWebResponce::send(QTcpSocket *messageStream){
    if(!messageStream)
        messageStream = request_->messageSocket_;

    QString responceHeader;

    responceHeader += QString("HTTP/1.0 ") + QString::number(statusCode_)+ " " + (statusMessage_ == "" ? defaultStatusMessages[statusCode_] : statusMessage_) + "\r\n";

    foreach(QString header, headers_.keys()){
        foreach(QString value, headers_[header]){
            responceHeader +=  header + ": " + value + "\r\n";
        }
    }
    responceHeader += "\r\n";
    messageStream->write(responceHeader.toUtf8());

    if(body_.length()){
        messageStream->write(body_);
        if(!(messageStream -> waitForBytesWritten())){
            qDebug() << request_->url_ << "sending responce body problem ((";
        }
    }
}

void HttpServer::HttpWebResponce::setText(QString text){
    headers_["Content-Type"] = QStringList() << "text/html; charset=\"utf-8\"";
    body_ = text.toUtf8();
}


void HttpServer::HttpWebResponce::_addHeader(QString header, QString value){
    if(!headers_.contains(header)){
        headers_[header] = QStringList();
    }
    headers_[header] << value;
}

void HttpServer::HttpWebResponce::_addText(QString text){
    body_.append(text.toUtf8());
}


void HttpServer::HttpWebRequest::parse(QTcpSocket *messageSocket){

    QTextStream textStream(messageSocket);
    textStream.setAutoDetectUnicode(true);
    messageSocket_ = messageSocket;
    QStringList firstLine;

    while(firstLine.length() == 0 || firstLine[0] == ""){
        firstLine = textStream.readLine().split(' ');
    }


    method_ = firstLine.first();
    firstLine.pop_front();

    protocol_ = firstLine.last();
    firstLine.pop_back();

    url_ = firstLine.join(' ');

    QStringList buff = url_.split('?');
    route_ = buff[0];
    if(route_[route_.length() - 1] == '/')
           route_.truncate(route_.length() - 1);

    if(buff.length() > 1){
        foreach(QString arg, buff[1].split('&')){
            QStringList splittedArg = arg.split('=');
            QString key = splittedArg[0];
            splittedArg.pop_front();

            if(!args_.contains(key))
                args_[key] = QStringList();
            args_[key].push_back(splittedArg.join('='));
        }
    }

    QString line = textStream.readLine();

    while(line != ""){
        buff = line.split(':');
        QString key = buff[0];
        buff.pop_front();
        if(!headers_.contains(key))
            headers_[key] = QStringList();

        headers_[key].push_back(
                    buff.join(':')
                    );

        line = textStream.readLine();
    }
}

HttpServer::HttpWebResponce HttpServer::HttpWebRequest::responce(int status){
    HttpServer::HttpWebResponce responce;

    responce.statusCode_ = status;
    responce.request_ = this;

    return responce;
}

QMap<int, QString> generateDefaultStatusMessages(){
    QMap<int, QString> result;

    result[100] = "Continue";
    result[101] = "Switching Protocols";
    result[102] = "Processing";
    result[103] = "Early Hints";
    result[200] = "OK";
    result[201] = "Created";
    result[202] = "Accepted";
    result[203] = "Non-Authoritative Information";
    result[204] = "No Content";
    result[205] = "Reset Content";
    result[206] = "Partial Content";
    result[207] = "Multi-Status;";
    result[208] = "Already Reported";
    result[226] = "IM Used";
    result[300] = "Multiple Choices";
    result[301] = "Moved Permanently";
    result[302] = "Moved Temporarily";
    result[303] = "See Other";
    result[304] = "Not Modified";
    result[305] = "Use Proxy";
    result[306] = "  "; //который зарезервирован
    result[307] = "Temporary Redirect";
    result[308] = "Permanent Redirect";
    result[400] = "Bad Request";
    result[401] = "Unauthorized";
    result[402] = "Payment Required";
    result[403] = "Forbidden";
    result[404] = "Not Found";
    result[405] = "Method Not Allowed";
    result[406] = "Not Acceptable";
    result[407] = "Proxy Authentication Required";
    result[408] = "Request Timeout";
    result[409] = "Conflict";
    result[410] = "Gone";
    result[411] = "Length Required";
    result[412] = "Precondition Failed";
    result[413] = "Payload Too Large";
    result[414] = "URI Too Long";
    result[415] = "Unsupported Media Type";
    result[416] = "Range Not Satisfiable";
    result[417] = "Expectation Failed";
    result[418] = "I’m a teapot";
    result[419] = "Authentication Timeout";
    result[421] = "Misdirected Request";
    result[422] = "Unprocessable Entity";
    result[423] = "Locked";
    result[424] = "Failed Dependency";
    result[425] = "Too Early";
    result[426] = "Upgrade Required";
    result[428] = "Precondition Required";
    result[429] = "Too Many Requests";
    result[431] = "Request Header Fields Too Large";
    result[449] = "Retry With";
    result[451] = "Unavailable For Legal Reasons";
    result[499] = "Client Closed Request";
    result[500] = "Internal Server Error";
    result[501] = "Not Implemented";
    result[502] = "Bad Gateway";
    result[503] = "Service Unavailable";
    result[504] = "Gateway Timeout";
    result[505] = "HTTP Version Not Supported";
    result[506] = "Variant Also Negotiates";
    result[507] = "Insufficient Storage";
    result[508] = "Loop Detected";
    result[509] = "Bandwidth Limit Exceeded";
    result[510] = "Not Extended";
    result[511] = "Network Authentication Required";
    result[520] = "Unknown Error";
    result[521] = "Web Server Is Down";
    result[522] = "Connection Timed Out";
    result[523] = "Origin Is Unreachable";
    result[524] = "A Timeout Occurred";
    result[525] = "SSL Handshake Failed";
    result[526] = "Invalid SSL Certificate";

    return result;
}


void HttpServer::Router::setRootHandler(QString method, std::shared_ptr<Handler> handler){
    rootHandlers_[method] = handler;
    hasRoot_ = true;
}

void HttpServer::Router::setDefaultHandler(QString method, std::shared_ptr<Handler> handler){
    defaultHandlers_[method] = handler;
    hasDefault_ = true;
}

void HttpServer::Router::addRouter(QStringList route, Router router){

    QPair<Router*, QString> target = getRouter(route);

    target.first->routers_[target.second] = router;
}

void HttpServer::Router::addMiddlware(QStringList route, std::shared_ptr<Middleware> middlware){
//    qDebug() << "add mid at" << route;
    //    route.push_back("");
    QPair<Router*, QString> target = getRouter(route);
    target.first->middlewares_.push_back(middlware);
}

void HttpServer::Router::addStatic(QStringList route, QString path){
    route.push_back("");
    QPair<Router*, QString> target = getRouter(route);
    target.first->hasStatic_ = true;
    target.first->staticPath_ = path;
}


std::shared_ptr<HttpServer::Handler> HttpServer::Router::sendFileHandlerFactory(QStringList route, QString basePath){
    QString path = basePath + route.join("/");
    QFile file(path);
    if(file.exists()){
        return std::make_shared<Handler>([path](HttpWebRequest& request) -> HttpWebResponce{

            QFile file(path);
            HttpWebResponce responce = request.responce(200);
            if(!file.open(QIODevice::ReadOnly)){
                   responce.statusCode_ = 500;
                   responce.body_ = file.errorString().toUtf8();
            }
            else
                responce.body_ = file.readAll();
            file.close();

            return responce;
        });
    }
    return std::make_shared<Handler>(nullptr);

//    return [self, route_length](HttpWebRequest& request) -> HttpWebResponce{

//        QStringList relpath = request.route_.split("/");
//        for(int i=0; i< route_length; i)
//                relpath.pop_front();

//        QString targetPath = self->staticPath_ + relpath.join("/");


//    };
//    QFile image("D:/prog/cpp/threader/13.jpg");
//    image.open(QIODevice::ReadOnly);
//    resp.body_ = image.readAll();
//    image.close();
}

void HttpServer::Router::addHandler(QString method, QStringList route, std::shared_ptr<Handler> handler){

    QPair<Router*, QString> target = getRouter(route);


    QHash<QString, HttpServer::Router::Route > &handlers  = target.first->handlers_;

    if(!handlers.contains(target.second))
        handlers[target.second] = HttpServer::Router::Route();
    handlers[target.second][method] = handler;
}

QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > > HttpServer::Router::getHandler(QString method, QStringList route){
//    qDebug() << method << route;

    if(route.length() && route[0] == ""){
        route.pop_front();
    }

    Router *currentRouter = this;
    QList<std::shared_ptr<Middleware> > midlwares;

    HttpServer::Router::Route *resultMethods = nullptr;

    QList<std::shared_ptr<Middleware> > currentDefaultsMidlwares;
    std::shared_ptr<HttpServer::Handler> currentDefault = nullptr;
    std::shared_ptr<HttpServer::Handler> buffHandler;


    //этот кусок повторён трижды -- лямбды спасут?
    midlwares.append(currentRouter->middlewares_);
    if(currentRouter->hasDefault_){
        buffHandler = Router::handlerFromRoute(method, currentRouter->defaultHandlers_);
        if(buffHandler != nullptr){
            currentDefault = buffHandler;
            currentDefaultsMidlwares = midlwares;
        }
    }
    //конец куска

    while(route.length() > 1){
        if(!currentRouter->routers_.contains(route[0])){
            if(currentRouter->hasStatic_){
                buffHandler = currentRouter->sendFileHandlerFactory(route, currentRouter->staticPath_);
                if(buffHandler != nullptr){
                    return QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > >(buffHandler, midlwares);
                }
            }
            return QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > >(currentDefault, currentDefaultsMidlwares);
        }
        currentRouter = &(currentRouter->routers_[route[0]]);
        route.pop_front();
        midlwares.append(currentRouter->middlewares_);
        if(currentRouter->hasDefault_){
            buffHandler = Router::handlerFromRoute(method, currentRouter->defaultHandlers_);
            if(buffHandler != nullptr){
                currentDefault = buffHandler;
                currentDefaultsMidlwares = midlwares;
            }
        }
    }
    if(route.length() == 0){
        if(currentRouter->hasRoot_)
            resultMethods = &(currentRouter->rootHandlers_);
    }
    else if(currentRouter->handlers_.contains(route[0])){
        resultMethods = &(currentRouter->handlers_[route[0]]);
    }
    else if(currentRouter->routers_.contains(route[0])){
        currentRouter = &(currentRouter->routers_[route[0]]);
        midlwares.append(currentRouter->middlewares_);
        if(currentRouter->hasDefault_){

            buffHandler = Router::handlerFromRoute(method, currentRouter->defaultHandlers_);

            if(buffHandler != nullptr){
                currentDefault = buffHandler;
                currentDefaultsMidlwares = midlwares;
            }
        }
        resultMethods = &(currentRouter->rootHandlers_);
    }

    if(!resultMethods && currentRouter->hasStatic_){
        buffHandler = currentRouter->sendFileHandlerFactory(route, currentRouter->staticPath_);
        if(buffHandler != nullptr){
            return QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > >(buffHandler, midlwares);
        }
    }

    if(resultMethods){
       buffHandler = Router::handlerFromRoute(method, *resultMethods);
        if(buffHandler != nullptr){
            return QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > >(buffHandler, midlwares);
        }
    }
    return QPair<std::shared_ptr<HttpServer::Handler>, QList<std::shared_ptr<HttpServer::Middleware> > >(currentDefault, currentDefaultsMidlwares);


}

QString HttpServer::Router::_print(QString prefix){
    QString result = "";
    result += prefix+"+-root\n";
    result += _printRoute(&rootHandlers_, prefix+"| ");
    result += prefix+"+-handlers\n";
    foreach(QString route, handlers_.keys()){
        result += prefix + "| +-" + route + "\n";
        result += _printRoute(&handlers_[route], prefix+"| |  ");
    }
    result += prefix+"+-routers\n";
    foreach(QString route, routers_.keys()){
        result += prefix + "| +-" + route + "\n";
        result += routers_[route]._print(prefix+"| |  ");
    }
    return result;
}

QString HttpServer::Router::_printRoute(QHash<QString, std::shared_ptr<Handler> > *route, QString prefix){
    QString result = "";
    foreach(QString method, route->keys()){
        result += prefix + "+-" + method + "\n";
    }
    return result;
}

std::shared_ptr<HttpServer::Handler> HttpServer::Router::handlerFromRoute(QString method, Route &route){
    if(route.contains(method))
        return route[method];
    if(route.contains(Router::ALL_METHODS))
        return route[Router::ALL_METHODS];
    return nullptr;
}

QPair<HttpServer::Router *, QString> HttpServer::Router::getRouter(QStringList route, bool addIfNotExists){
    Router *currentRouter = this;

    while(route.length() > 1){
        if(!currentRouter->routers_.contains(route[0])){
            if(addIfNotExists)
                currentRouter->routers_[route[0]] = Router();
            else
                return QPair<Router*, QString>(nullptr, route[0]);
        }

        currentRouter = &(currentRouter->routers_[route[0]]);
        route.pop_front();
    }

    return QPair<Router*, QString>(currentRouter, route[0]);

}

HttpServer::RestContainer::RestContainer(HttpWebRequest *request, std::shared_ptr<Handler> handler, QList<std::shared_ptr<Middleware> > middlewares)
    : handler_(handler), middlewares_(middlewares), request_(request)
{

}

HttpServer::HttpWebResponce HttpServer::RestContainer::execute(){
    return nextFactory(middlewares_.begin())(*request_);
}

HttpServer::Handler HttpServer::RestContainer::nextFactory(QList<std::shared_ptr<Middleware> >::iterator index) const{
    const RestContainer* self = this;
    return [self, index](HttpWebRequest& request) -> HttpWebResponce{
        if(index != self->middlewares_.end())
            return (*(*index))(request, self->nextFactory(index + 1));
        return (*(self->handler_))(request);
    };
}

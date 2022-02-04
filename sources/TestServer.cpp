#include "../test/TestServer.h"
#include <QFile>



HttpServer::HttpWebResponce rootPage(TestServer::HttpWebRequest &request){
    TestServer::HttpWebResponce resp = request.responce(200);
    resp.setText("<h1>Опачки</h1>"
                 "hello, чертила<br/>"
                 "<a href='/content/pages/1'>Какой-то контент</a><br/>"
                 "<a href='/content/pages/2'>Тут ничего нет</a><br/>"
                 "<a href='/errorPage'>А тут должно быть больно</a><br/>"
                 "<a href='/api/over9000'>9Mb текст</a><br/>"
                 "<img src='/img/image.jpeg' />");
    return resp;
}

HttpServer::HttpWebResponce image(TestServer::HttpWebRequest &request){

    TestServer::HttpWebResponce resp = request.responce(200);

    resp.headers_["Content-Type"] = QStringList() << "image/png";

    QFile image("test/interface/build/favicon-16x16.png");
    image.open(QIODevice::ReadOnly);

    resp.body_ = image.readAll();
    image.close();

    resp.headers_["Content-Length"] = QStringList() << QString("%1").arg(resp.body_.length()) ;

    return resp;
}


HttpServer::HttpWebResponce contentPage(TestServer::HttpWebRequest &request){

    TestServer::HttpWebResponce resp = request.responce(200);
    resp.statusMessage_ = "U can write some shitty things here";
    resp.setText("<h1>Какой-то контент</h1>"
                 "<a href='/testPage'>домой</a><br/>");
    return resp;
}
HttpServer::HttpWebResponce errorPage(TestServer::HttpWebRequest &request){
    TestServer::HttpWebResponce resp = request.responce(200);

    std::ignore = std::string().at(5);

    return resp;
}

HttpServer::HttpWebResponce over9000(TestServer::HttpWebRequest &request){
    TestServer::HttpWebResponce resp = request.responce(200);

    QString answer = "дела:<br/>";
    resp.setText(answer);

    for(unsigned long long i=0; i< 320000;i++)
        resp._addText(QString::number(i) + ") " + "Дело №" + QString::number(i) + "<br/>");


    return resp;
}

HttpServer::HttpWebResponce headersControlLoggerMiddlware(HttpServer::HttpWebRequest &request, HttpServer::Handler next){

    HttpServer::HttpWebResponce responce = next(request);
    qDebug() << request.method_ << request.url_
             << "[" << responce.statusCode_ << "]" << responce.headers_;
    return responce;
}



TestServer::TestServer(unsigned short port, QObject *parent):HttpServer(port, true, 10, parent){

    router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(corsMiddlware));
//    router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(headersControlLoggerMiddlware));



    router_.setRootHandler("GET", router_.sendFileHandlerFactory(QStringList() << "index.html", "test/interface/build/"));

    router_.addStatic(tr("interface").split("/"),"test/interface/build/");

    router_.addHandler("GET",tr("api/over9000").split("/"), std::make_shared<Handler>(over9000));


    router_.addHandler("GET",tr("testPage").split("/"), std::make_shared<Handler>(rootPage));
    router_.addHandler("GET",tr("errorPage").split("/"), std::make_shared<Handler>(errorPage));
    router_.addHandler("GET",tr("content/pages/1").split("/"), std::make_shared<Handler>(contentPage));
    router_.addHandler("GET",tr("img/image.jpeg").split("/"), std::make_shared<Handler>(image));


//    qDebug() << router_._print();

}

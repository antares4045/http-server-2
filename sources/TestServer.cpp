#include "../test/TestServer.h"

#include <QJsonDocument>
#include <memory>
#include <functional>

void F1(QJsonObject header, QJsonObject body){
    for(int i=0; i<3; i++){
        qDebug() << "F1" << header << body;
        QThread::sleep(1);
    }
    qDebug() << "F1" << header << body << "finished";
}

void F2(QJsonObject header, QJsonObject body){
    for(int i=0; i<5; i++){
        qDebug() << "F2" << header << body;
        QThread::sleep(1);
    }
    qDebug() << "F2" << header << body << "finished";
}

void Exit(QJsonObject, QJsonObject){
    QThread::sleep(1);
    exit(0);
}


HttpServer::HttpWebResponce TestServer::functionResolver(HttpServer::HttpWebRequest &request){
    HttpServer::HttpWebResponce resp = request.responce(200);

    QTextStream textStream(request.messageSocket_);
    textStream.setAutoDetectUnicode(true);

    if(!request.headers_.contains("content-length")){
        resp.statusCode_ =500;
        return resp;
    }

    QJsonDocument body = request.readJson();
//    qDebug() << body;



    taskManager_->onMessage(
                body["header"].toObject(),
                body["body"].toObject()
                );

    resp.setText("OK");
    return resp;
}




TestServer::TestServer(QString interfacePath, unsigned short port, QObject *parent):HttpServer(port, true, 10, parent) {

    router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(corsMiddlware));


    std::shared_ptr<Handler> root = router_.sendFileHandlerFactory(QStringList() << "index.html", interfacePath);
    router_.setRootHandler("GET",  *root != nullptr ? root : std::make_shared<Handler>(default404Handler));

    router_.addStatic(tr("interface").split("/"), interfacePath);

//    auto self = this;

    Handler handler = [this](TestServer::HttpWebRequest &req) -> HttpWebResponce{
        return  functionResolver(req);
    };
    router_.addHandler("POST", tr("api/FunctionResolver").split("/"), std::make_shared<Handler>(handler));
    router_.addHandler("GET", tr("api/exit").split("/"), std::make_shared<Handler>(
       [this](TestServer::HttpWebRequest &req) -> HttpWebResponce{
                       taskManager_->onMessage({
                           {"function", "exit"}
                       },
                       {});
                         HttpWebResponce res = req.responce(200);
                         res.setText("stopped");
                         return res;
                       }));


    QJsonObject config{
            {"functionSelectorFieldName", "function"}
        };

    serviceBase::TaskManager::FunctionMapperType mapper {
        {"F1", F1},
        {"F2", F2},
        {"exit", Exit},
    };


    taskManager_ = std::make_shared<serviceBase::TaskManager>(config, mapper);
    taskManager_->start();
}

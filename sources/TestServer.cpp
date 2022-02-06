#include "../test/TestServer.h"

#include <QJsonDocument>
#include <memory>

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




TestServer::TestServer(unsigned short port, QObject *parent):HttpServer(port, true, 10, parent) {

    router_.addMiddlware(QString("").split("/"), std::make_shared<Middleware>(corsMiddlware));


    router_.setRootHandler("GET", router_.sendFileHandlerFactory(QStringList() << "index.html", "test/interface/build/"));
    router_.addStatic(tr("interface").split("/"),"test/interface/build/");

    auto self = this;

    router_.addHandler("POST", tr("api/FunctionResolver").split("/"), std::make_shared<Handler>([self](HttpWebRequest &req)->HttpWebResponce {
                           return self->functionResolver(req);
                       }));


    QJsonObject config{
            {"functionSelectorFieldName", "function"}
        };

    serviceBase::TaskManager::FunctionMapperType mapper {
        {"F1", F1},
        {"F2", F2}
    };


    taskManager_ = std::make_shared<serviceBase::TaskManager>(config, mapper);
    taskManager_->start();
}

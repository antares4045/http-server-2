#pragma once
#include "../utils/HttpServer.h"
#include "includes/TaskManager.h"

class TestServer: public HttpServer{
    Q_OBJECT
public:
    TestServer(QString interfacePath="test/interface/build/", unsigned short port=8000, QObject *parent = nullptr);

private:
    HttpServer::HttpWebResponce functionResolver(HttpServer::HttpWebRequest &request);
    std::shared_ptr<serviceBase::TaskManager> taskManager_;
};


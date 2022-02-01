#pragma once
#include "../utils/HttpServer.h"

class TestServer: public HttpServer{
    Q_OBJECT
public:
    TestServer(unsigned short port=8000, QObject *parent = nullptr);
};


#include <QtCore/QCoreApplication>

#include "test/TestServer.h"





int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestServer server(":/test/interface/build/", 8000);

    server.start();


    return a.exec();
}

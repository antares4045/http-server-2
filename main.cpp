#include <QtCore/QCoreApplication>

#include "test/TestServer.h"





int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QJsonObject config{
//            {"functionSelectorFieldName", "function"}
//        };

//    serviceBase::TaskManager::FunctionMapperType mapper{
//        {"F1", F1},
//        {"F2", F2}
//    };

//    serviceBase::TaskManager manager(config, mapper);
//    manager.start();

//    manager.onMessage({{"function", "F1"}}, {{"bodyText", "body1"}});
//    manager.onMessage({{"function", "F2"}}, {{"bodyText", "body2"}});
//    manager.onMessage({{"function", "F1"}}, {{"bodyText", "body3"}});

    TestServer server(8000);

    server.start();


    return a.exec();
}

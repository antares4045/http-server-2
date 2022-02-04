#include <QtCore/QCoreApplication>

#include "test/TestServer.h"
#include "test/TestQObjects.h"
#include "includes/TaskManager.h"



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


void pushMessage(serviceBase::TaskManager* manager, QString header, QString body){

    manager->onMessage(
                QJsonDocument::fromJson(header.toUtf8()).object(),
                QJsonDocument::fromJson(body.toUtf8()).object()
                );
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QJsonObject config{
            {"functionSelectorFieldName", "function"}
        };

    serviceBase::TaskManager::FunctionMapperType mapper{
        {"F1", F1},
        {"F2", F2}
    };

    serviceBase::TaskManager manager(config, mapper);


    manager.onMessage({{"function", "F1"}}, {{"bodyText", "body1"}});
    manager.onMessage({{"function", "F2"}}, {{"bodyText", "body2"}});
    manager.onMessage({{"function", "F1"}}, {{"bodyText", "body3"}});


    return a.exec();
}

#include <QtCore/QCoreApplication>

#include "test/TestServer.h"
#include "includes/ThreadManager.h"
#include "test/TestQObjects.h"
#include "includes/TaskManager.h"


#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

using serviceBase::ThreadManager;

std::shared_ptr<TestServer> startServer(){


    std::shared_ptr<TestServer> server = std::make_shared<TestServer>(8000);

    server->start();
    return server;
}
QPair<std::shared_ptr<ThreadManager>, std::shared_ptr<SuperWaiter> > testThreadManager(){
    std::shared_ptr<ThreadManager> manager = std::make_shared<ThreadManager>();
    std::shared_ptr<SuperWaiter> waiter = std::make_shared<SuperWaiter>(manager.get());

    manager->start();


    Waiter* w1 = new Waiter(1, 5);
    Waiter* w2 = new Waiter(2, 3);
    Waiter* w3 = new Waiter(3, 5);

    manager->addTask(w1, false);
    manager->addTask(w2, false);
    manager->addTask(w3, false);

   // manager.waitAllCurentTasks();
   return QPair<std::shared_ptr<ThreadManager>, std::shared_ptr<SuperWaiter> >(manager, waiter);
}



void testOdbc(){

    QString driverName = "PostgreSQL Unicode(x64)";

//    QSqlDatabase::registerSqlDriver(driverName, new QSqlDriverCreator<QSqlDriver>);

    qDebug() << QSqlDatabase::drivers();

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
//    QString connString = "DRIVER={PostgreSQL Unicode(x64)};SERVER=127.0.0.1;PORT=43434;DATABASE=data-provider;UID=postgress;PWD=xtqdUnzQykq6eSjMnaZsGnmaBTzvVRY7XqF6vQdx9SBbtn9UNHrzdWRX6dHPFPLP";
    QString connString = QStringLiteral(
                //"DRIVER=C:/Program Files/psqlODBC/1302/bin/psqlodbc35w.dll;"
                "SERVERNODE=127.0.0.1:43434;"
                "UID=postgress;"
                "PWD=xtqdUnzQykq6eSjMnaZsGnmaBTzvVRY7XqF6vQdx9SBbtn9UNHrzdWRX6dHPFPLP;"
                "SCROLLABLERESULT=false"
                );

    qDebug() << connString;
    db.setDatabaseName(connString);
//    db.setConnectOptions("DRIVER={PostgreSQL Unicode(x64)};SERVER=127.0.0.1;PORT=43434;DATABASE=data-provider;UID=postgress;PWD=xtqdUnzQykq6eSjMnaZsGnmaBTzvVRY7XqF6vQdx9SBbtn9UNHrzdWRX6dHPFPLP");

    if(!db.open()){
        qDebug() << db.lastError();
        return;
    }
    else{
        qDebug() << "база открылась";
    }

    QSqlQuery result = db.exec("SELECT id, login, \"hashPassword\", role, email, displayed_name, disabled, \"createdAt\", \"updatedAt\", \"deletedAt\"\
            FROM public.users;");

    QSqlRecord rec = result.record();

    qDebug() << "columns: " << rec;


}

int main_old(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

  auto manager = testThreadManager();
//  auto server = startServer();
//    testOdbc();

    return a.exec();
}


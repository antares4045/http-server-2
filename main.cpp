#include <QCoreApplication>

#include "test/TestServer.h"
#include "includes/ThreadManager.h"
#include "test/TestQObjects.h"





int main(int argc, char *argv[])
{
      QCoreApplication a(argc, argv);


      ThreadManager manager;
      SuperWaiter waiter(&manager);
      manager.start();


      Waiter w1(1, 5);
      Waiter w2(2, 3);
      Waiter w3(3, 5);

      manager.addTask(&w1);
      manager.addTask(&w2);
      manager.addTask(&w3);

      QThread::sleep(1);

      return a.exec();

//    QCoreApplication a(argc, argv);

//    TestServer server(8000);
//    server.start();

//    return a.exec();
}

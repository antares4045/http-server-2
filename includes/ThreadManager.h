#pragma once

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>
#include <QThread>
#include <QtConcurrent>
#include <QHash>

class ThreadManager:public QObject
{
    Q_OBJECT
public:
    ThreadManager(QObject *parent=nullptr);
    ~ThreadManager();
signals:
    void taskFinished(QRunnable *);
    //обратите внимание, что слоты будут выполняться в потоке, эмитящем сигнал (и он не имеет возможности проверять, дошёл ли сигнал до кого)
    // => настоятельная просьба ЛОГИКУ СЛОТА, если она сложнее палки, РЕАЛИЗОВЫВАТЬ В ОТДЕЛЬНОМ ПОТОКЕ ОТ СЛОТА

public slots:
    void addTask(QRunnable *task);

    void start();

    void stop(bool dropRunning=true);
    // dropRunning=false использует waitAllCurentTasks: к использованию не рекомендуется

protected slots:
    void waitAllCurentTasks();
    // джойнит все фьючеры в основной поток из-за чего сначала они все выполняться, а потом уже вызовутся слоты, привязанные к taskFinished
    // => не пользуйтесь этим слотом, если вы не уверены, что знаете, что делаете
private:
    void mainLoop();
    static void processTask(QRunnable *task, ThreadManager *self);

    QHash<QRunnable *, QFuture<void> >  pool_;
    QLinkedList<QRunnable *>  finished_;
    bool running_=false;
    bool emitAllTasks_=false; //в случае остановки убедиться, что все завершённые таски были эмитнуты (при удалении поток всеравно будет дропнут: так что смысла мало)

    std::shared_ptr<QFuture<void> > mainLoopHandler_=nullptr;

    static const unsigned long POOLING_SLEEP_MSECS=100;
};


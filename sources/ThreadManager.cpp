#include "../includes/ThreadManager.h"

namespace serviceBase{
    ThreadManager::ThreadManager(QObject *parent):QObject(parent){

    }

    ThreadManager::~ThreadManager(){
        stop();
    }


    void ThreadManager::processTask(QRunnable *task, ThreadManager *self, bool clearOnFinish){
        task->run();
        self->finished_.push_back(task);
        self->pool_.remove(task);
        if(clearOnFinish)
            delete task;
    }

    void ThreadManager::addTask(QRunnable *task, bool clearOnFinish){

        QFuture<void> future = QtConcurrent::run(ThreadManager::processTask, task, this, clearOnFinish);
        pool_[task] = future;

    }

    void ThreadManager::start(){
        if(mainLoopHandler_){
            throw std::runtime_error("Поток уже запущен");
        }
        running_=true;
        mainLoopHandler_=std::make_shared<QFuture<void> >(
                    QtConcurrent::run(this, &ThreadManager::mainLoop) //в шестёрке аргументы в другом порядке: БЕСИТ (пятёрка логичнее бтв (если мы не делаем вид, что это питон))
                    );

    }

    void ThreadManager::stop(bool dropRunning){
        if(!dropRunning){
            emitAllTasks_=true;
            waitAllCurentTasks();
        }
        if(running_){
            running_=false;
            mainLoopHandler_->waitForFinished();
        }else{
            if(mainLoopHandler_ == nullptr)
                qDebug() << "остановлен не запущенный менеджер потоков: возможно вы пропустили вызов слота start()";
        }
    }

    void ThreadManager::waitAllCurentTasks(){
        for(QFuture<void> &future: pool_.values()){
            future.waitForFinished();
        }
    }

    void ThreadManager::mainLoop(){
        while(running_ || (emitAllTasks_ && !finished_.isEmpty()) ){
            if(finished_.isEmpty()){
                QThread::msleep(POOLING_SLEEP_MSECS);
            }else{
                emit taskFinished(finished_.first());
                finished_.pop_front();
            }
        }
    }

}

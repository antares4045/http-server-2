#include "../test/TestQObjects.h"


SuperWaiter::SuperWaiter(serviceBase::ThreadManager *manager):QObject(){
    connect(manager, SIGNAL(taskFinished(QRunnable *)), this, SLOT(onWaited(QRunnable *)));
}

void SuperWaiter::onWaited(QRunnable *task){
    Waiter *waiter = static_cast<Waiter *>(task);
    qDebug() << "waiter" << waiter->id_ << "completed" << waiter->comleted_ << waiter->result_;
    delete waiter;
}

Waiter::Waiter(int id, int duration): QRunnable(), id_(id), duration_(duration) {

}

void Waiter::run(){
    result_ = 0;
    qDebug() << id_ << 0 << "/" << duration_;
    for(int i = 0; i < duration_; i++){
        qDebug() << id_ << i << "/" << duration_;

        result_ += id_;

        QThread::sleep(1);
    }
    qDebug() << id_ << "finished";
    comleted_ = true;
    return;
}

Waiter::~Waiter(){
    qDebug() << "отчистка waiter" << id_;
}

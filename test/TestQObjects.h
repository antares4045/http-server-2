#pragma once
#include <QObject>
#include <QRunnable>
#include <QThread>
#include <QDebug>

#include "../includes/ThreadManager.h"

class Waiter: public QRunnable {
public:
    Waiter(int id, int duration=5);
    virtual ~Waiter();

    void run() override;

    int id_;
    int duration_;

    bool comleted_ = false;
    int result_;
};


class SuperWaiter:public QObject{
    Q_OBJECT
public:
    SuperWaiter(serviceBase::ThreadManager *manager);
public slots:
    void onWaited(QRunnable *waiter);

};

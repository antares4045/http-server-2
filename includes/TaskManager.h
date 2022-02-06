#pragma once

#include <QObject>
#include <QThread>
#include <QtConcurrent>
#include <QHash>
#include <QJsonObject>
#include <QRunnable>
#include "../includes/ThreadManager.h"



class createQrunnablePolyfill: QRunnable{
   // потому что QRunnable::create имплементировано в 5.15
public:
    static QRunnable *create(std::function<void ()> functionToRun){
        return new createQrunnablePolyfill(functionToRun);
    }
private:
    createQrunnablePolyfill(std::function<void ()> &function):QRunnable(),func_(function){

    }
    void run() override{
        func_();
    }
    std::function<void ()> func_;
};


namespace serviceBase{
    // пока тупой пересылальщик
    typedef const QJsonObject headerType;
    typedef const QJsonObject bodyType;
    typedef QString headerFunctionControlType;

    class TaskManager:public ThreadManager
    {
        Q_OBJECT
    public:
        typedef QHash<headerFunctionControlType, std::function<void(headerType&, bodyType&)> > FunctionMapperType;
        TaskManager(QJsonObject config, FunctionMapperType mapper, QObject *parent=nullptr);

        virtual void defaultHandler(headerType&, bodyType&);

        virtual QRunnable *construct(headerType header, bodyType body);
    public slots:
        void onMessage(headerType header, bodyType body);
    private slots:
        void onTaskFinished(QRunnable *task);
        void onNewTask(QRunnable *task);
    private:
        FunctionMapperType mapper_;
        std::shared_ptr<headerFunctionControlType> controlHeader_ = nullptr;
        static QString configFunctionSelectorFieldName;
    };

}



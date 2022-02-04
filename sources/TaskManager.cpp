#include "../includes/TaskManager.h"



QString serviceBase::TaskManager::configFunctionSelectorFieldName = "functionSelectorFieldName";

serviceBase::TaskManager::TaskManager(QJsonObject config, FunctionMapperType mapper, QObject *parent)
    :QObject(parent), mapper_(mapper), threadpool_(this)
{
    if(config.contains(TaskManager::configFunctionSelectorFieldName)){
        controlHeader_ = std::make_shared<headerFunctionControlType>(config[TaskManager::configFunctionSelectorFieldName].toString());
    }
}

void serviceBase::TaskManager::defaultHandler(headerType &, bodyType &){
    throw std::invalid_argument("serviceBase::TaskManager::defaultHandler has been called");
}

QRunnable *serviceBase::TaskManager::construct(headerType header, bodyType body){
    TaskManager *self = this;
    if(controlHeader_ != nullptr && header.contains(*controlHeader_) && mapper_.contains(header[*controlHeader_].toString())){
        return createQrunnablePolyfill::create(
                    [self, header, body]() -> void {
            std::function<void(headerType&, bodyType&)> func = self->mapper_[header[*(self->controlHeader_)].toString()];
            return func(header, body);
        }
        );
    }

    return createQrunnablePolyfill::create(
                [self, header, body]() -> void {
        return self->defaultHandler(header, body);
    }
    );
}

void serviceBase::TaskManager::onMessage(headerType header, bodyType body){
    //место под планировщик
    QRunnable * task = construct(header, body);
    onNewTask(task);
}

void serviceBase::TaskManager::onTaskFinished(QRunnable *task) {
    //таски будут пробрасываться с флагом запрета автоматической отчистки (по крайней мере пока) так что тут обязательно надо отчистить
    delete task;
    //место под балансировщик, проверяющий какую задачу из очереди можно впихнуть на осободившееся место
}

void serviceBase::TaskManager::onNewTask(QRunnable *task) //над этой сигнатурой мы ещё подумаем
{
    //место под балансировщик, проверяющий можно ли впихнуть задачу
    threadpool_.addTask(task, true); //пока бездумный проброс -\()/-
}

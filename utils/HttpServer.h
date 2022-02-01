#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QJsonObject>
//Class smart pointers
#include <memory>//std::unique_ptr, std::make_unique


class HttpServer:public QTcpServer
{
    Q_OBJECT
public:
    struct HttpWebRequest;
    struct HttpWebResponce;
    class Router;
    typedef std::function<HttpWebResponce(HttpWebRequest &)> Handler;
    typedef std::function<HttpWebResponce(HttpWebRequest &, Handler)> Middleware ;

    static HttpWebResponce default404Handler(HttpWebRequest &request);
    static HttpWebResponce errorCatcherMiddlware(HttpWebRequest &request, Handler next);
    static HttpWebResponce loggerMiddlware(HttpWebRequest &request, Handler next);

    class ClientProcess;
    friend ClientProcess;


    HttpServer(unsigned short port=8000, bool addDefaultHandlers=true, unsigned short maxThreads=100, QObject *parent=nullptr);
    ~HttpServer();


public slots:
    void start();
    void stop();


private:
    void incomingConnection(qintptr socketDescriptor) override;

    QThreadPool threadPool_;

    unsigned short port_;

public:
    class ClientProcess:public QRunnable{
      public:
        ClientProcess(qintptr descriptor, HttpServer *server);
        void run();
      private:
        std::unique_ptr<QTcpSocket> clientSocket_;
        qintptr descriptor_;
        HttpServer *server_;
    };
    class Router{
        //это делается СОВСЕМ не так, но для моих задач сойдёт)
    public:
        static const QString ALL_METHODS;

        void setRootHandler(QString method, std::shared_ptr<Handler> handler);
        void setDefaultHandler(QString method, std::shared_ptr<Handler> handler);
        void addRouter(QStringList route, Router router);

        void addHandler(QString method, QStringList route, std::shared_ptr<Handler> handler);
        void addMiddlware(QStringList route, std::shared_ptr<Middleware> middlware);
        void addStatic(QStringList route, QString path); // мне влом думать как взаимодействует статика и дефолты -- не мешайте их по одному руту

        std::shared_ptr<Handler> sendFileHandlerFactory(QStringList route, QString basePath);

        QPair<std::shared_ptr<Handler>, QList<std::shared_ptr<Middleware> > > getHandler(QString method, QStringList route);


        QString _print(QString prefix="");
        QString _printRoute(QHash<QString, std::shared_ptr<Handler> > *route, QString prefix);

    private:
        typedef QHash<QString, std::shared_ptr<Handler> > Route;

        static std::shared_ptr<Handler> handlerFromRoute(QString method, Route &route);

        QPair<Router*, QString> getRouter(QStringList route, bool addIfNotExists=true);
        QHash<QString, Router>  routers_;
        QHash<QString, Route> handlers_;
        QString staticPath_;

        Route rootHandlers_;
        Route defaultHandlers_;
        bool hasRoot_ = false;
        bool hasDefault_ = false;
        bool hasStatic_ = false;

        QList<std::shared_ptr<Middleware> > middlewares_;
    };

    struct HttpWebRequest{
        QString method_;
        QString route_;
        QString url_;
        QString protocol_;

        QHash<QString, QStringList > args_;
        QHash<QString, QStringList > headers_;

        QTcpSocket *messageSocket_;

        QJsonObject diveValues; //точка взаимодействия мидлвари с детьми

        void parse(QTcpSocket *messageSocket);

        HttpServer::HttpWebResponce responce(int status = 200);
    };

    struct HttpWebResponce{
        HttpWebRequest* request_;
        int statusCode_=200;
        QString statusMessage_="";
        QHash<QString, QStringList > headers_;
        QByteArray body_;

        QJsonObject ascentValues; //точка взаимодействия детей с мидлварью

        static QMap<int, QString> defaultStatusMessages;

        void send(QTcpSocket *messageStream=nullptr);

        void setText(QString text);

        void _addHeader(QString header, QString value);
        void _addText(QString text);

    };
protected:
    HttpServer::Router router_;
private:
    class RestContainer{
    public:
        RestContainer(HttpWebRequest *request, std::shared_ptr<Handler> handler, QList<std::shared_ptr<Middleware> > middlewares);
        HttpWebResponce execute();

    private:
        Handler nextFactory(QList<std::shared_ptr<Middleware> >::iterator index) const;

        std::shared_ptr<Handler> handler_;
        QList<std::shared_ptr<Middleware> > middlewares_;
        HttpWebRequest *request_;
    };
};



QMap<int, QString> generateDefaultStatusMessages();

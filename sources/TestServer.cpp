#include "../test/TestServer.h"
#include <QFile>



HttpServer::HttpWebResponce rootPage(TestServer::HttpWebRequest &request){
    TestServer::HttpWebResponce resp = request.responce(200);
    resp.setText("<h1>Опачки</h1>"
                 "hello, чертила<br/>"
                 "<a href='/content/pages/1'>Какой-то контент</a><br/>"
                 "<a href='/content/pages/2'>Тут ничего нет</a><br/>"
                 "<a href='/errorPage'>А тут должно быть больно</a><br/>"
                 "<img src='/img/image.jpeg' />");
    return resp;
}

HttpServer::HttpWebResponce image(TestServer::HttpWebRequest &request){

    TestServer::HttpWebResponce resp = request.responce(200);

    resp.headers_["Content-Type"] = QStringList() << "image/jpeg";

    QFile image("D:/prog/cpp/threader/13.jpg");
    image.open(QIODevice::ReadOnly);
//    image.open("D:/prog/cpp/threader/13.jpg", QIODevice::ReadOnly);
    resp.body_ = image.readAll();
    image.close();

    resp.headers_["Content-Length"] = QStringList() << QString("%1").arg(resp.body_.length()) ;

    QFile saved("D:/prog/cpp/threader/save.jpg");
    saved.open(QIODevice::WriteOnly);
    saved.write(resp.body_);
    saved.close();

    return resp;
}


HttpServer::HttpWebResponce contentPage(TestServer::HttpWebRequest &request){

    TestServer::HttpWebResponce resp = request.responce(200);
    resp.statusMessage_ = "U can write some shitty things here";
    resp.setText("<h1>Какой-то контент</h1>"
                 "<a href='/'>домой</a><br/>");
    return resp;
}
HttpServer::HttpWebResponce errorPage(TestServer::HttpWebRequest &request){
    TestServer::HttpWebResponce resp = request.responce(200);


    std::ignore = std::string().at(5);

    return resp;
}




TestServer::TestServer(unsigned short port, QObject *parent):HttpServer(port, true, 10, parent){



    router_.setRootHandler("GET",
                           router_.sendFileHandlerFactory(QStringList() << "index.html", "test/interface/build/"));

    router_.addStatic(tr("interface").split("/"),"test/interface/build/");

//    router_.addHandler("GET",tr("errorPage").split("/"), std::make_shared<Handler>(errorPage));
//    router_.addHandler("GET",tr("content/pages/1").split("/"), std::make_shared<Handler>(contentPage));
//    router_.addHandler("GET",tr("img/image.jpeg").split("/"), std::make_shared<Handler>(image));


//    qDebug() << router_._print();

}

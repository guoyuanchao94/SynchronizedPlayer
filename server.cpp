#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    qInfo() << "Server::构造服务端";
}

Server::~Server()
{
    qInfo() << "Server::服务端析构,端口号: " << port << " closed.";
    //清理套接字
    for (auto client : clients)
    {
        client->close();
        client->deleteLater();
        client = nullptr;
    }
    tcpServer->close();
    tcpServer->deleteLater();
    tcpServer = nullptr;
}

void Server::initServer(int port)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);
    this->port = port;
    listen();   //开始监听
}

void Server::newConnection()
{
    //添加到客户端列表
    while (tcpServer->hasPendingConnections())
    {
        appendNewConnection(tcpServer->nextPendingConnection());
    }
}

//这里的tcpsocket获取到的新连接和sender函数获取到的信号发送者是一个对象
void Server::appendNewConnection(QTcpSocket *client)
{
    qInfo() << "Server::添加新连接: " << client->peerPort();
    clients.append(client);
    qDebug() <<"Server::连接到的客户端的数目: " << clients.count();
    connect(client, &QTcpSocket::disconnected, this, &Server::clientDisconnected);  //绑定客户端端口事件
    connect(client, &QTcpSocket::readyRead, this, &Server::readFromClient); // 绑定接收端读取数据的响应函数
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (isSourceSet)    //如果设置了播放源,将播放源发送给新连接的客户端
    {
        QString type = "src";
        client->write(type.toUtf8() + source.toUtf8() + QString("\n").toUtf8());// src+source+"\n"
        client->flush();
    }
    //对于第一次连接,这个客户端队列是空的,得先建立连接再接收数据
    //等再有新连接之后,先发送消息给客户端,把当前用户列表发给他
    QString clientList = "";
    for (auto myClient : clients)
    {
        if (myClient->objectName() != "")
        {
            clientList.append(myClient->objectName() + "&&");  //添加客户端带有&&,作为定界符
        }
    }
    clientList.chop(2); //移除最后的定界符&&
    //如果第n次连接,这里只会打印n-1次的客户端信息
    qDebug() << "Server::打印客户端列表" << clientList;
    if (clientList != "")
    {
        //发送客户端列表,等下客户端收到消息再去分割
        // usrlst+列表+"\n"
        client->write(QString("usrlst").toUtf8() + clientList.toUtf8() + QString("\n").toUtf8());
        client->flush();
    }
}

//发送给所有的客户端
void Server::writeToClients(QString msg, QString type)
{
    if(clients.size() == 0)
    {
        return;
    }
    qInfo() << "Server::发送给所有的客户端, type: " << type << ",msg: " << msg;
    for (const auto &client : clients)
    {
        QByteArray buffer = type.toUtf8() + msg.toUtf8() + "\n";
        client->write(buffer);  //这里是和连接到服务器的客户端通信,是他们知道有哪些客户端连接
        client->flush();
    }
}

//这是断开信号的时候,当前客户端已经断开连接了,不必要发送信息
//发送给所有的client除了currrentClient
void Server::writeToClients(QString msg, QString type, QTcpSocket *currentClient)
{
    qInfo() << "Server::发送给除了当前客户端以外的客户端, type: " << type << ", msg: " << msg;
    for (auto client: clients)
    {
        if (client != currentClient)
        {
            QByteArray buffer = type.toUtf8() + msg.toUtf8() + "\n";
            client->write(buffer);
            client->flush();
        }
    }
}

//从clients队列删除断开的client
void Server::clientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());  //获取发送者
    if(client == nullptr)
    {
        qDebug() << "Server::无法确定发送者";
        return;
    }
    qInfo() << "Server::客户端断开连接,移除客户端: " << client << " = " << clients.removeOne(client);
    //让其他客户端也知道这个消息,除了这个断开连接的客户端
    writeToClients("dis" + client->objectName(), "usr", client);
    emit removeUdpClient(client);   //同时也移除UDP客户端
    clients.squeeze();  //移除不存储项的内存
    client->deleteLater();
    client = nullptr;
}

//响应客户端的处理
void Server::readFromClient()
{
    //获取哪一个客户端套接字发送的信息,这个返回的是与客户端相连接的服务端这边的套接字,而不是发送信息的远程客户端
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    while (client->canReadLine())
    {
        QByteArray buffer = client->readLine();
        qDebug() <<"Server::读取到的数据: " << buffer;

        QString header = buffer.mid(0, 3);
        QString content = buffer.mid(3).trimmed();  //从位置3的字符获取子字符串,去除首尾空格
        qInfo() << "Server::读取客户端数据, header: " << header << ", content: " << content;
        if (header == "snc")
        {
            qInfo() << "Server::服务端同步消息来自于" << client->objectName() << " : " << content;
            writeToClients(content, "snc"); //同步给其他的客户端
        }   
        else if (header == "msg")
        {
            //qInfo() << "Message from " << client->objectName() << " : " << content;
            writeToClients(client->objectName() + ": " + content, "msg");
        }   
        else if (header == "usr")
        {
            client->setObjectName(content); //相当于给服务端维持的客户端列表的某一个设置对象名
            writeToClients("con" + client->objectName(), "usr");    //客户端连接消息,通知其他客户端
            // usrcon+name
        }
        else if (header == "src")
        {
            source = content;
            isSourceSet = true;
            QString srcType = content.mid(0, 3);//已经去除了src头部,srcType = net or lcl
            content = content.mid(3);   //去除net or lcl
            if (srcType == "net")
            {
                writeToClients(content, "srcnet");
            }
            if (srcType == "lcl")
            {
               writeToClients(content, "srclcl");
            }
            //writeToClients("New source: " + content, "msg");
            qInfo() << "Server::新播放源: " << content;
        }
        else if (header == "prt")
        {
            qInfo() << "Server::发送端的IP地址:" << client->peerAddress() << "Udp端口: " << content << "Tcp端口: " << client->peerPort();
            emit addUdpClient(client, client->peerAddress(), content.toUShort());   //tcp和udp联系起来
        }
    }
}

void Server::listen()
{
    tcpServer->listen(QHostAddress::Any, port);
    qInfo() << "Server::开始监听端口号: " << port;
}

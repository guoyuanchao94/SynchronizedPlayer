#include "udpserver.h"

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{

}

UdpServer::~UdpServer()
{
    for (auto client : clients)
    {
        delete client;
        client = nullptr;
    }
    socket->close();
    // socket->deleteLater();
    delete socket;
    socket = nullptr;
}

void UdpServer::addUdpClient(QTcpSocket *client, QHostAddress ip, quint16 udpPort)
{
    clients.append(new UdpConnection(client, ip, udpPort, this));
    qDebug() << "UdpServer::当前UDP连接数: " << clients.count();

    for(int i = 0; i < clients.count(); ++i)
    {
        qDebug() << "UdpServer::打印udp端口号: " << clients[i]->getUdpPort();
    }
}

void UdpServer::removeUdpClient(QTcpSocket *client)
{
    for (auto myClient : clients)
    {
        if (myClient->getClient() == client)
        {
            qInfo() << "UdpServer::UDP移除客户端:" << clients.removeOne(myClient);
            clients.squeeze();
            myClient->deleteLater();
            myClient = nullptr;
        }
    }
}

void UdpServer::initServer(int port)
{
    qDebug() << "UdpServer::初始化UDP服务端,接收端口: " << port << "的信息";
    socket = new QUdpSocket(this);
    this->port = port;
    socket->bind(port); //监听来自这个端口的信息
    connect(socket, &QUdpSocket::readyRead, this, &UdpServer::udpReadFromClient);
}

void UdpServer::udpReadFromClient()
{
    while (socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket->receiveDatagram();
        //这里是UDP模拟的连接,当当发送connected信号后,像udpsocket发射数据,表示连接
        if (datagram.data() == "UDP")
        {
            qInfo() << "UdpServer::Udp连接:" << datagram.senderAddress() << datagram.senderPort();
        }
        else
        {
            //将语音消息转发给其他客户端
            writeToClients(datagram.data(), datagram.senderAddress(), datagram.senderPort());
        }
    }
}

void UdpServer::writeToClients(QByteArray data, QHostAddress senderAddress, quint16 senderPort)
{
    for (auto client : clients)
    {
        if (client->getIp() != senderAddress && client->getUdpPort() != senderPort)
        {
            if (client->getIp() == QHostAddress("::1"))
            {
                //发送数据给本地服务器
                qint64 sendMessage = socket->writeDatagram(data, QHostAddress("127.0.0.1"), client->getUdpPort());
                if(sendMessage == -1)
                {
                    qDebug() << "UdpServer::数据发送失败";
                    qDebug() << "UdpServer::失败原因: " << socket->errorString();
                }
            }
            else
            {
                qint64 sendMessage = socket->writeDatagram(data, client->getIp(), client->getUdpPort());
                if(sendMessage == -1)
                {
                    qDebug() << "UdpServer::数据发送失败";
                    qDebug() << "UdpServer::失败原因: " << socket->errorString();
                }
            }
        }
    }
}

#include "udpclient.h"

UdpClient::UdpClient(QObject *parent) : QObject(parent)
{
    qInfo() << "UdpClient::UdpClient";
}

UdpClient::~UdpClient()
{
    qInfo() << "UdpClient::~UdpClient";
    socket->close();
    delete socket;
    socket = nullptr;
}

void UdpClient::udpConnectToLocalServer(int port)
{
    socket = new QUdpSocket(this);
    socket->connectToHost(QHostAddress(QHostAddress::LocalHost), port);
    qDebug() << "UdpClient::打印udpsocket的端口: " << socket->localPort();
    QByteArray data;
    data.append("UDP");
    socket->write(data);    //发数据模拟连接,udp本身无连接
    emit connected();
    emit setUdpPort(socket->localPort());
    //将系统分配给该udpsocket的端口号发射出去
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::udpReadFromServer);
}

void UdpClient::udpConnectToServer(QUrl url, int port)
{
    socket = new QUdpSocket(this);
    //也可能是服务端的IP地址
    socket->connectToHost(QHostAddress(url.toString()), port);
    qDebug() << "UdpClient::打印udpsocket的端口: " << socket->localPort();
    QByteArray data;
    data.append("UDP");
    socket->write(data);
    emit connected();
    qDebug() << "UdpClient::本地端口号是 " << socket->localPort();
    emit setUdpPort(socket->localPort());   //发射当前udpsocket分配的端口号
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::udpReadFromServer);
}

void UdpClient::udpReadFromServer()
{
    while (socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket->receiveDatagram();
        emit voiceReceived(datagram.data());
    }
}

void UdpClient::udpWriteToServer(QByteArray data)
{
    if (socket)
    {
        socket->write(data);
    }
}

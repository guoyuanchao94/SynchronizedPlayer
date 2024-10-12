#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QUrl>
#include <QNetworkDatagram>

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr);
    ~UdpClient();
public:
    void udpConnectToLocalServer(int port);
    void udpWriteToServer(QByteArray data);

signals:
    void setUdpPort(quint16 udpPort);   //设置UDP端口号
    void connected();   //udp本身无连接,这里是模拟连接
    void voiceReceived(QByteArray data);    //收到麦克风消息

public slots:
    void udpConnectToServer(QUrl url, int port);

private slots:
    void udpReadFromServer();
private:
    QUdpSocket *socket = nullptr;
};

#endif // UDPCLIENT_H

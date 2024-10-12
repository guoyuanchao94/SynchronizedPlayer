#include "connecttoserver.h"
#include "ui_connecttoserver.h"
#include <QPushButton>

ConnectToServer::ConnectToServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToServer)
{
    ui->setupUi(this);
    connect(ui->ipInput,&QLineEdit::textChanged,this,&ConnectToServer::on_TextChanged);
    connect(ui->portInput,&QLineEdit::textChanged,this,&ConnectToServer::on_TextChanged);
    connect(ui->userNameInput,&QLineEdit::textChanged,this,&ConnectToServer::on_TextChanged);
}

ConnectToServer::~ConnectToServer()
{
    qInfo() << "ConnectToServer::~ConnectToServer";
    delete ui;
}

void ConnectToServer::on_buttonBox_accepted()
{
    QUrl url = ui->ipInput->text();
    int port = ui->portInput->text().toInt();
    //前面已经限制这三项不能为空了,无需判断
    qInfo() << "ConnectToServer::on_buttonBox_accepted,连接到: " << url << ", 端口号:" << port;
    QString userName = ui->userNameInput->text();
    emit connectToServer(url, port);
    emit connectToUdpServer(url, port); //连接到Udp服务端
    emit setUserName(userName); //设置客户端名字,便于区分
    emit setServerURL(url);
}

void ConnectToServer::on_TextChanged(const QString &text)
{
    Q_UNUSED(text)
    QString ip= ui->ipInput->text().trimmed();
    QString port = ui->portInput->text().trimmed();
    QString userName = ui->userNameInput->text().trimmed();
    bool enable = !ip.isEmpty() && !port.isEmpty() && !userName.isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

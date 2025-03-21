#include "openurl.h"
#include "ui_openurl.h"
#include <QFileDialog>
openURL::openURL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::openURL)
{
    ui->setupUi(this);
}

openURL::~openURL()
{
    delete ui;
}

void openURL::on_openURLbuttonBox_accepted()
{
    QString url = ui->urlInput->text();
    if(url.isEmpty())
    {
        return;
    }
    emit urlSet(url);
}



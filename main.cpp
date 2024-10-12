#include "mainwindow.h"

#include <QApplication>
#include <QThread>
#include <QFile>
#include <QTimer>
#include <QSGRendererInterface>
#include <QQuickWindow>

int main(int argc, char *argv[])
{
    qputenv("QSG_RHI_BACKEND", "opengl");

    QApplication a(argc, argv);
    MainWindow w;

    // 创建 QQuickWindow 实例
    QQuickWindow window;

    // 获取 QSGRendererInterface 实例，并输出渲染后端
    QSGRendererInterface *interface = window.rendererInterface();
    QSGRendererInterface::GraphicsApi api = interface->graphicsApi();

    // 输出渲染后端信息
    switch (api)
    {
    case QSGRendererInterface::OpenGL:
        qDebug() << "Renderer backend: OpenGL";
        break;
    case QSGRendererInterface::Direct3D11:
        qDebug() << "Renderer backend: Direct3D 11";
        break;
    case QSGRendererInterface::Direct3D12:
        qDebug() << "Renderer backend: Direct3D 12";
        break;
    default:
        qDebug() << "Renderer backend: Unknown";
    }

    QFile file(":/stylesheets/Combinear.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(file.readAll()) };
    a.setStyleSheet(styleSheet);

    w.setWindowTitle("同步视频播放器");
    w.show();

    QObject::connect(&w, &MainWindow::themeChanged, &w, [&w](QString theme)->void
    {
        QFile file(":/stylesheets/" + theme + ".qss");
        file.open(QFile::ReadOnly);
        QString styleSheet { QLatin1String(file.readAll()) };
        w.setStyleSheet(styleSheet);
    });

    return a.exec();
}

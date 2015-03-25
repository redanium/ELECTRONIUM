#include "mainwindow.h"
#include <QApplication>
#include <QPaintDevice>
#include <QDesktopWidget>
#include "DatabaseManager.h"
#include <QSplashScreen>
#include<QMessageBox>
#include<QStatusBar>
#include<QStyleFactory>
#include<QDebug>
#include<QThread>

class SplashScrThread : public QThread
{
public:
    static void sleep(unsigned long secs) {
        QThread::sleep(secs);
    }
};

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);
    QString tmp_str;
    QApplication a(argc, argv);
    //QStyleFactory::create("fusion");
    //QApplication::setStyle("fusion");

#ifdef Q_OS_ANDROID //test purpose

    tmp_str=QString::number(QFileInfo("/sdcard/mydatabase.sqlite").exists());
    
#else

    tmp_str=QString::number(QFileInfo(QCoreApplication::applicationDirPath().append("/mydatabase.sqlite")).exists());

#endif // Q_OS_ANDROID
    
    QPixmap pixmap(":/files/images/electrical.png");
    
    QSplashScreen splash(pixmap);
    
    splash.show();

    SplashScrThread::sleep(1);

    MainWindow w;

    
    w.showMaximized();

    w.show();
    QApplication::beep();
    splash.finish(&w);
    
    return a.exec();

}

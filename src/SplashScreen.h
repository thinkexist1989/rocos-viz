#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H


#include <QSplashScreen>
#include <QPixmap>
#include <QProgressBar>
#include <QList>
#include <QtGlobal>

class SplashScreen: public QSplashScreen
{
    Q_OBJECT

public:
    SplashScreen(QPixmap& pixmap,int time);
    ~SplashScreen();

private:
    //进度条
    QProgressBar *ProgressBar;
    //随机数列表
    QList<int> numbersList;
    //启动界面停留的时间
    int elapseTime;

public:
    void setProgress(int value);
    void generateAscendRandomNumber();

private slots:
     void slotUpdateProgress();
};


#endif // SPLASHSCREEN_H

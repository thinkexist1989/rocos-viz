#include "SplashScreen.h"

#include <QTime>
#include <QTimer>

SplashScreen::SplashScreen(QPixmap& pixmap,int time) :
    QSplashScreen(pixmap),
    elapseTime(time)
{
    ProgressBar = new QProgressBar(this);
    //设置进度条的位置
    ProgressBar->setGeometry(0,pixmap.height()-20,pixmap.width(),40);
    //设置进度条的样式
    ProgressBar->setStyleSheet("QProgressBar {color:black;font:30px;text-align:center; }QProgressBar::chunk {background-color: rgb(50, 200, 50);}");
    //设置进度条的范围
    ProgressBar->setRange(0, 100);
    //设置进度条的当前进度
    ProgressBar->setValue(0);

    ProgressBar->setTextVisible(false);

    QFont font("Arial",15);
    this->setFont(font);
    ProgressBar->setFont(font);

//    generateAscendRandomNumber();
//    setProgress();
}

SplashScreen::~SplashScreen()
{

}

void SplashScreen::setProgress(int value)
{
//    int tempTime=elapseTime/100;
//    for(int i=0;i<100;i++)
//    {
//        QTimer::singleShot(i*tempTime, this, SLOT(slotUpdateProgress()));
//    }
//    QTimer::singleShot(elapseTime, this, SLOT(close()));
ProgressBar->setValue(value);
}

void SplashScreen::slotUpdateProgress()
{
    static int num=0;
    ProgressBar->setValue(numbersList[num]);
    num++;
}

void SplashScreen::generateAscendRandomNumber()
{
    int i;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    //生成100个大小在[0,100]之间的随机数
    for(i=0;i<100;i++)
    {
        numbersList.append(qrand()%101);
    }
    //递增排序
    qSort(numbersList.begin(),numbersList.end());
}


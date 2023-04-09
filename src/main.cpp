#include "RocosMainWindow.h"

#include <QApplication>

#include <QSurfaceFormat>
#include <QStyleFactory>

#include <QVTKOpenGLNativeWidget.h>

#include <SplashScreen.h>

#include <QThread>

int main(int argc, char *argv[])
{
    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));

//    QPixmap pixmap(":/res/splash.png");://res/rocos-viz.jpg
    QPixmap pixmap(":/res/logo.jpg");
    QPixmap scaledPixmap = pixmap.scaled(700,700, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    SplashScreen* splash = new SplashScreen(scaledPixmap, 10000);
    splash->show();

    for(int i = 0; i <= 100; i++) {
        splash->setProgress(i);
        QThread::usleep(10000);
    }

    RocosMainWindow w;
    w.show();
    splash->finish(&w);
    return a.exec();
}

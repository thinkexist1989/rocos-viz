#include "RocosMainWindow.h"

#include <QApplication>

#include <QSurfaceFormat>
#include <QStyleFactory>

#include <QVTKOpenGLNativeWidget.h>

int main(int argc, char *argv[])
{
    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.ico"));

    RocosMainWindow w;
    w.show();
    return a.exec();
}

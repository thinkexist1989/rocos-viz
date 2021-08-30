#include "RocosMainWindow.h"

#include <QApplication>

#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>

int main(int argc, char *argv[])
{
    // needed to ensure appropriate OpenGL context is created for VTK rendering.
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icon.png"));

    RocosMainWindow w;
    w.show();
    return a.exec();
}

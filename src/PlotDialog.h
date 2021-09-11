#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>

#include <vtkAxis.h>
#include <vtkChartMatrix.h>     //矩阵图表 subplot
#include <vtkChartXY.h>         //平面图表
#include <vtkContextScene.h>    //
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPen.h>
#include <vtkPlot.h>
#include <vtkPlotPoints.h>
#include <vtkTable.h>

#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>   //配合QVTKOpenGLNativeWidget，在Qt中显示VTK

namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = nullptr);
    ~PlotDialog();

private:
    Ui::PlotDialog *ui;

    vtkSmartPointer<vtkContextView> view;

};

#endif // PLOTDIALOG_H

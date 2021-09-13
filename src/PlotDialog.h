#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QMovie>
#include <QTimer>

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
#include <vtkPlotLine.h>
#include <vtkPlotPoints.h>
#include <vtkTable.h>
#include <vtkChartLegend.h>
#include <vtkTextProperty.h>
#include <vtkBrush.h>
#include <vtkVariantArray.h> //用于向vtkTable插入数据

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

private slots:
    void on_recordButton_clicked();

    void on_typeComboBox_currentIndexChanged(int index);

private:
    Ui::PlotDialog *ui;

    QMovie* movie;

    bool isRecording = false;

    vtkSmartPointer<vtkNamedColors> colors; //颜色
    vtkSmartPointer<vtkContextView> view; //画图区域的展示窗口

    //Table存储数据格式： t x y z r p y j1 j2 j3 j4 j5 j6 j7
    vtkSmartPointer<vtkTable> posTable; //用于保存所有pos数据
    vtkSmartPointer<vtkTable> velTable; //TODO: 暂未实现 用于保存所有vel数据
    vtkSmartPointer<vtkTable> accTable; //TODO: 暂未实现 用于保存所有acc数据
    vtkSmartPointer<vtkTable> jerkTable; //TODO: 暂未实现 用于保存所有jerk数据

    vtkSmartPointer<vtkChartMatrix> chartMat; //用于展示不同曲线的组合

    vtkSmartPointer<vtkChartXY>   posChart; //用于显示位置
    QVector<vtkSmartPointer<vtkPlot>> posPlots; //用于存储pos中的曲线

    vtkSmartPointer<vtkChartXY>   velChart; //用于显示速度
    QVector<vtkSmartPointer<vtkPlot>> velPlots; //用于存储vel中的曲线

    vtkSmartPointer<vtkChartXY>   accChart; //用于显示加速度
    QVector<vtkSmartPointer<vtkPlot>> accPlots; //用于存储acc中的曲线

    vtkSmartPointer<vtkChartXY>   jerkChart; //用于显示加加速度
    QVector<vtkSmartPointer<vtkPlot>> jerkPlots; //用于存储jerk中的曲线

    QTimer* timer;
    vtkSmartPointer<vtkVariantArray> newData;

    int ii = 0;



};

#endif // PLOTDIALOG_H

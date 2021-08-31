#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QVTKOpenGLNativeWidget.h>

#include <vtkNew.h>
#include <vtkVersion.h>

#include <vtkNamedColors.h>                 //颜色
#include <vtkGenericOpenGLRenderWindow.h>   //配合QVTKOpenGLNativeWidget，在Qt中显示VTK
#include <vtkRenderer.h>                    //渲染器
#include <vtkRenderWindow.h>                //渲染窗口 this->ui->qvtkWidget->GetRenderWindow()获取
#include <vtkPolyDataMapper.h>              //多边形数据映射
#include <vtkProperty.h>                    // 配置Actor的各种属性
#include <vtkCamera.h>                      // 配置相机
#include <vtkTransform.h>                   // 变换

#include <vtkConeSource.h>                  //三角锥
#include <vtkPlaneSource.h>                 //平面画网格
#include <vtkSphereSource.h>                //球面
#include <vtkAnnotatedCubeActor.h>           //正方体

#include <vtkAxesActor.h>                   //坐标系

#include <vtkInteractorStyleTrackballActor.h>  // 操作Actor
#include <vtkInteractorStyleTrackballCamera.h> // 操作Camera

#include <vtkSTLReader.h>                   //读取stl文件
#include <vtkOrientationMarkerWidget.h>     //带marker的widget

#include <vtkSmartPointer.h>

#include <Model.h>

class SceneWidget : public QVTKOpenGLNativeWidget
{
public:
    SceneWidget(QWidget  * parent  =   0);
    ~SceneWidget();

    void setJointPos(std::vector<double> &jntRads);

    void setJointAxesVisibility(bool isVisible);

private:
    vtkSmartPointer<vtkNamedColors> colors;         //负责处理各种颜色

    vtkSmartPointer<vtkRenderer>    renderer;       //渲染操作

    vtkSmartPointer<vtkGenericOpenGLRenderWindow>  renderWindow; //渲染窗口，关联到this

    vtkSmartPointer<vtkPlaneSource> groundSource;  //存储地面网格相关信息
    vtkSmartPointer<vtkActor>       groundActor;   //地面显示操作

    vtkSmartPointer<vtkAxesActor>   axesActor;     //Marker坐标系显示操作

    vtkSmartPointer<vtkOrientationMarkerWidget> marker; //左下角姿态Marker显示

    std::shared_ptr<Model> model;
};

#endif // SCENEWIDGET_H

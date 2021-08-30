#include "rocosmainwindow.h"
#include "./ui_rocosmainwindow.h"

#include <vtkNew.h>
#include <vtkVersion.h>

#include <vtkNamedColors.h>                 //颜色
#include <vtkGenericOpenGLRenderWindow.h>   //配合QVTKOpenGLNativeWidget，在Qt中显示VTK
#include <vtkRenderer.h>                    //渲染器
#include <vtkRenderWindow.h>                //渲染窗口 this->ui->qvtkWidget->GetRenderWindow()获取
#include <vtkPolyDataMapper.h>              //多边形数据映射
#include <vtkProperty.h>                    // 配置Actor的各种属性

#include <vtkConeSource.h>                  //三角锥
#include <vtkPlaneSource.h>                 //平面画网格
#include <vtkSphereSource.h>                //球面

#include <vtkAxesActor.h>                   //坐标系

#include <vtkInteractorStyleTrackballActor.h>  // 操作Actor
#include <vtkInteractorStyleTrackballCamera.h> // 操作Camera

RocosMainWindow::RocosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RocosMainWindow)
{
    ui->setupUi(this);

    vtkNew<vtkNamedColors> colors;

    /* ======== 三角锥 ======== */
    vtkNew<vtkConeSource> cone;
//    cone->Update();

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->SetPosition(1,1,1);
//    coneActor->SetOrientation();

    /* ======== 画平面 ======== */
    vtkNew<vtkPlaneSource> plane;
    plane->SetXResolution(10);
    plane->SetYResolution(10);
    plane->SetOrigin(-5,-5, 0);
    plane->SetPoint1(5, -5, 0);
    plane->SetPoint2(-5, 5, 0);

    vtkNew<vtkPolyDataMapper> planeMapper;
    planeMapper->SetInputConnection(plane->GetOutputPort());

    vtkNew<vtkActor> planeActor;
    planeActor->SetMapper(planeMapper);
    planeActor->GetProperty()->SetRepresentationToWireframe();
    planeActor->GetProperty()->SetColor(colors->GetColor3d("DeepSkyBlue").GetData());


    /* ======== 画坐标系 ======== */
    vtkNew<vtkAxesActor> axesActor;
    axesActor->SetPosition(0,0,0);
//    axesActor->SetTotalLength(2,2,2);
//    axesActor->SetShaftType(0);
//    axesActor->SetAxisLabels(0);
//    axesActor->SetXAxisLabelText("X axis");
//    axesActor->SetYAxisLabelText("Y axis");
//    axesActor->SetScale(2);
//    axesActor->SetCylinderRadius(0.01);
//    axesActor->SetConeRadius(0.05);

    //渲染
    vtkNew<vtkRenderer> renderer;

    renderer->AddActor(coneActor); // 画三角锥
    renderer->AddActor(axesActor); // 画坐标系
    renderer->AddActor(planeActor); // 画平面

    renderer->GradientBackgroundOn(); //开启渐变
    renderer->SetBackground(colors->GetColor3d("LightSkyBlue").GetData());
    renderer->SetBackground2(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

//    this->ui->widget->GetRenderWindow()->AddRenderer(renderer);
    this->ui->widget->SetRenderWindow(renderWindow);

//    vtkNew<vtkInteractorStyleTrackballActor> actorStyle;
//    this->ui->widget->GetInteractor()->SetInteractorStyle(actorStyle);

    renderer->ResetCamera();
//    renderer.Cam

}

RocosMainWindow::~RocosMainWindow()
{
    delete ui;
}


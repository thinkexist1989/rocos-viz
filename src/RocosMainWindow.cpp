#include <RocosMainWindow.h>
#include "./ui_RocosMainWindow.h"

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

RocosMainWindow::RocosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RocosMainWindow)
{
    ui->setupUi(this);

    vtkNew<vtkNamedColors> colors;

    /* ======== 三角锥 ======== */
//    vtkNew<vtkConeSource> cone;
////    cone->Update();

//    vtkNew<vtkPolyDataMapper> coneMapper;
//    coneMapper->SetInputConnection(cone->GetOutputPort());

//    vtkNew<vtkActor> coneActor;
//    coneActor->SetMapper(coneMapper);
//    coneActor->SetPosition(1,1,1);

    /* ======== 画平面 ======== */
    vtkNew<vtkPlaneSource> plane;
    plane->SetXResolution(10);
    plane->SetYResolution(10);
    plane->SetOrigin(-1,-1, 0);
    plane->SetPoint1(1, -1, 0);
    plane->SetPoint2(-1, 1, 0);

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
   axesActor->SetScale(0.5);
//    axesActor->SetCylinderRadius(0.01);
//    axesActor->SetConeRadius(0.05);

    /* ======== 关节0 ========== */
    vtkNew<vtkSTLReader> reader0;
    reader0->SetFileName("models/iiwa/link_0.stl");
    reader0->Update();

    vtkNew<vtkPolyDataMapper> mapper0;
    mapper0->SetInputConnection(reader0->GetOutputPort());

    vtkNew<vtkActor> link0Actor;
    link0Actor->SetMapper(mapper0);
//    link0Actor->GetProperty()->SetDiffuse();

    /* ======== 关节1 ========== */
    vtkNew<vtkSTLReader> reader1;
    reader1->SetFileName("models/iiwa/link_1.stl");
    reader1->Update();

    vtkNew<vtkPolyDataMapper> mapper1;
    mapper1->SetInputConnection(reader1->GetOutputPort());

    vtkNew<vtkTransform> trans1;
    trans1->Translate(0.0, 0.0, 0.15);
    trans1->RotateZ(0.0);
    trans1->RotateY(0.0);
    trans1->RotateX(0.0);

    trans1->Translate(0.0, 0.0, 0.0075);
    trans1->RotateZ(0.0);
    trans1->RotateY(0.0);
    trans1->RotateX(0.0);

    vtkNew<vtkActor> link1Actor;
    link1Actor->SetMapper(mapper1);
    link1Actor->SetUserTransform(trans1);



    //渲染
    vtkNew<vtkRenderer> renderer;

//    renderer->AddActor(coneActor); // 画三角锥
    renderer->AddActor(axesActor); // 画坐标系
    renderer->AddActor(planeActor); // 画平面

    renderer->AddActor(link0Actor); // link0
    renderer->AddActor(link1Actor); // link1

    renderer->GradientBackgroundOn(); //开启渐变
    renderer->SetBackground(colors->GetColor3d("LightSkyBlue").GetData());
    renderer->SetBackground2(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

//    this->ui->widget->GetRenderWindow()->AddRenderer(renderer);
    this->ui->widget->SetRenderWindow(renderWindow);

    vtkNew<vtkAnnotatedCubeActor> cubeActor;
    cubeActor->SetXPlusFaceText("L");
    cubeActor->SetXMinusFaceText("R");
    cubeActor->SetYMinusFaceText("I");
    cubeActor->SetYPlusFaceText("S");
    cubeActor->SetZMinusFaceText("P");
    cubeActor->SetZPlusFaceText("A");
    cubeActor->GetTextEdgesProperty()->SetColor(colors->GetColor3d("Yellow").GetData());
    cubeActor->GetTextEdgesProperty()->SetLineWidth(2);
    cubeActor->GetCubeProperty()->SetColor(colors->GetColor3d("Red").GetData());

    static vtkNew<vtkOrientationMarkerWidget> axes;
    axes->SetOrientationMarker(axesActor);
    axes->SetInteractor(this->ui->widget->GetInteractor());
    axes->EnabledOn();
    axes->InteractiveOff();
//    axes->InteractiveOn();
//    axes->SetPickingManaged(false);
    renderer->ResetCamera();

//    renderWindow->Render();
//    renderer->GetActiveCamera()->Azimuth(45);
//    renderer->GetActiveCamera()->Elevation(30);

//    vtkNew<vtkInteractorStyleTrackballActor> actorStyle;
//    this->ui->widget->GetInteractor()->SetInteractorStyle(actorStyle);

}

RocosMainWindow::~RocosMainWindow()
{
    delete ui;
}


/*
Copyright 2021, Yang Luo"
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@Author
Yang Luo, PHD
Shenyang Institute of Automation, Chinese Academy of Sciences.
 email: luoyang@sia.cn
*/

#include <SceneWidget.h>

/*
 * @brief 构造函数中，初始化各种默认组件
 */
SceneWidget::SceneWidget(QWidget *parent) :
    QVTKOpenGLNativeWidget(parent),
    colors(vtkNamedColors::New()),
    renderer(vtkRenderer::New()),
    renderWindow(vtkGenericOpenGLRenderWindow::New()),
    groundSource(vtkPlaneSource::New()),
    groundActor(vtkActor::New()),
    axesActor(vtkAxesActor::New()),
    marker(vtkOrientationMarkerWidget::New())
{
    /* ======== 画平面 ======== */
    groundSource->SetXResolution(10);
    groundSource->SetYResolution(10);
    groundSource->SetOrigin(-1,-1, 0);
    groundSource->SetPoint1(1, -1, 0);
    groundSource->SetPoint2(-1, 1, 0);

    vtkNew<vtkPolyDataMapper> groundMapper;
    groundMapper->SetInputConnection(groundSource->GetOutputPort());

    groundActor->SetMapper(groundMapper);
    groundActor->GetProperty()->SetRepresentationToWireframe();
    groundActor->GetProperty()->SetColor(colors->GetColor3d("DeepSkyBlue").GetData());

    /* ======== 画坐标系 ======== */
//    axesActor->SetPosition(0,0,0);
//    axesActor->SetTotalLength(2,2,2);
//    axesActor->SetShaftType(0);
//    axesActor->SetAxisLabels(0);
//    axesActor->SetXAxisLabelText("X axis");
//    axesActor->SetYAxisLabelText("Y axis");
//   axesActor->SetScale(0.5);
//    axesActor->SetCylinderRadius(0.01);
//    axesActor->SetConeRadius(0.05);







//    /* ======== 关节0 ========== */
//    vtkNew<vtkSTLReader> reader0;
//    reader0->SetFileName("models/iiwa/link_0.stl");
//    reader0->Update();

//    vtkNew<vtkPolyDataMapper> mapper0;
//    mapper0->SetInputConnection(reader0->GetOutputPort());

//    vtkNew<vtkActor> link0Actor;
//    link0Actor->SetMapper(mapper0);
////    link0Actor->GetProperty()->SetDiffuse();

//    /* ======== 关节1 ========== */
//    vtkNew<vtkSTLReader> reader1;
//    reader1->SetFileName("models/iiwa/link_1.stl");
//    reader1->Update();

//    vtkNew<vtkPolyDataMapper> mapper1;
//    mapper1->SetInputConnection(reader1->GetOutputPort());

//    vtkNew<vtkTransform> trans1;
//    trans1->Translate(0.0, 0.0, 0.15);
//    trans1->RotateZ(0.0);
//    trans1->RotateY(0.0);
//    trans1->RotateX(0.0);

//    trans1->Translate(0.0, 0.0, 0.0075);
//    trans1->RotateZ(0.0);
//    trans1->RotateY(0.0);
//    trans1->RotateX(0.0);

//    vtkNew<vtkActor> link1Actor;
//    link1Actor->SetMapper(mapper1);
//    link1Actor->SetUserTransform(trans1);

    model = std::make_shared<Model>("models/iiwa/config.yaml");
//    model = new Model("models/iiwa/config.yaml");
    model->addToRenderer(renderer);


    /* ======== 渲染器 ========== */
    renderer->GradientBackgroundOn(); //开启渐变
    renderer->SetBackground(colors->GetColor3d("LightSkyBlue").GetData());
    renderer->SetBackground2(colors->GetColor3d("MidnightBlue").GetData());
    /* ======== 渲染窗口 ========== */
    renderWindow->AddRenderer(renderer);
    /* ======== 关联渲染窗口 ========== */
    this->SetRenderWindow(renderWindow);


//    renderer->AddActor(axesActor); // 画坐标系
    renderer->AddActor(groundActor); // 画平面

//    renderer->AddActor(link0Actor); // link0
//    renderer->AddActor(link1Actor); // link1

    marker->SetOrientationMarker(axesActor);
    marker->SetInteractor(this->GetInteractor());
    marker->EnabledOn();
    marker->InteractiveOff();
//    axes->InteractiveOn();
//    axes->SetPickingManaged(false);


//    renderer->ResetCamera();


//    renderer->GetActiveCamera()->Pitch(45);
//    renderer->GetActiveCamera()->Azimuth(30);
//    renderer->GetActiveCamera()->Elevation(-30);
    renderer->GetActiveCamera()->SetPosition(3,3,3);
    renderer->GetActiveCamera()->SetFocalPoint(0,0,0.5);
    renderer->GetActiveCamera()->SetViewUp(0,0,1);

}

SceneWidget::~SceneWidget()
{

}

void SceneWidget::setJointPos(std::vector<double>& jntRads)
{
    model->updateModel(jntRads);
    renderWindow->Render();
}

void SceneWidget::setJointAxesVisibility(bool isVisible)
{
    model->setAxesVisiblity(isVisible);
    renderWindow->Render();
}

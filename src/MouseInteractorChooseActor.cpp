#include "MouseInteractorChooseActor.h"

MouseInteractorChooseActor::MouseInteractorChooseActor()
{
    lastPickedActor_ = nullptr;
    lastPickedProperty_ = vtkProperty::New();
}

MouseInteractorChooseActor::~MouseInteractorChooseActor() {
    lastPickedProperty_->Delete();
}

void MouseInteractorChooseActor::OnLeftButtonDown() {
    vtkNew<vtkNamedColors> colors;

    int* clickPos = this->GetInteractor()->GetEventPosition();

    // Pick from this location.
    vtkNew<vtkPropPicker> picker;
    picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

    // If we picked something before, reset its property
    if(this->lastPickedActor_) {
        this->lastPickedActor_->GetProperty()->DeepCopy(this->lastPickedProperty_);
    }
    this->lastPickedActor_ = picker->GetActor(); //获取现在的actor，如果没有则为null
    if(this->lastPickedActor_) {
        this->lastPickedProperty_->DeepCopy(this->lastPickedActor_->GetProperty());
        //高亮显示选中的actor
        this->lastPickedActor_->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
        this->lastPickedActor_->GetProperty()->SetDiffuse(1.0);
        this->lastPickedActor_->GetProperty()->SetSpecular(0.0);
        this->lastPickedActor_->GetProperty()->EdgeVisibilityOn();
    }


    // Forward events 正常处理OnLeftButtonDown事件
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}


vtkStandardNewMacro(MouseInteractorChooseActor);

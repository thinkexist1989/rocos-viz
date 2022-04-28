#ifndef MOUSEINTERACTORCHOOSEACTOR_H
#define MOUSEINTERACTORCHOOSEACTOR_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkObjectFactory.h> // vtkStandardNewMacro()


class MouseInteractorChooseActor : public vtkInteractorStyleTrackballCamera
{
public:
    static MouseInteractorChooseActor* New();
    vtkTypeMacro(MouseInteractorChooseActor, vtkInteractorStyleTrackballCamera);

    MouseInteractorChooseActor();
    ~MouseInteractorChooseActor() override;

    void OnLeftButtonDown() override;


private:
    vtkActor* lastPickedActor_;
    vtkProperty* lastPickedProperty_;
};

#endif // MOUSEINTERACTORCHOOSEACTOR_H

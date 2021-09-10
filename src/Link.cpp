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

#include "Link.h"

#include <iostream>
#include <string>
#include <vtkProperty.h>

Link::Link() {
    init();
}

/**
 * @param fileName
 */
Link::Link(const std::string &fileName) {
    init();
    setMesh(fileName);
}

Link::~Link() {

}

void Link::init()
{
    translate << 0, 0, 0;
    rotate << 0, 0, 0;
    translateLink << 0, 0, 0;
    rotateLink << 0, 0, 0;

    reader = vtkSTLReader::New();
    actor  = vtkActor::New();
    axesActor = vtkAxesActor::New();

    axesActor->SetTotalLength(axesLength, axesLength, axesLength);
    axesActor->SetShaftType(vtkAxesActor::CYLINDER_SHAFT);
    axesActor->SetAxisLabels(isAxesLableShow);
    axesActor->SetVisibility(isAxesVisible);

}

void Link::setMesh(const std::string &fileName)
{
    reader->SetFileName(fileName.c_str());
    reader->Update();

    vtkNew<vtkPolyDataMapper> mappper;
    mappper->SetInputConnection(reader->GetOutputPort());

    actor->SetMapper(mappper);
}

void Link::setAngle(double rad)
{
    angle = rad;
}

void Link::setActorTransform(vtkTransform *t)
{
    actor->SetUserTransform(t);
    axesActor->SetUserTransform(t);
}

void Link::setAxesVisibility(bool isVisible)
{
    if(isVisible) {
        axesActor->SetVisibility(1);
    }
    else {
        axesActor->SetVisibility(0);
    }

}

void Link::setMeshVisibility(bool isMesh)
{
    if(isMesh) {
        actor->GetProperty()->SetRepresentationToWireframe();
    }
    else {
        actor->GetProperty()->SetRepresentationToSurface();
    }
}




//Link &Link::operator=(Link &b) {
//    if(this == &b)
//        return *this;

//    return *this;
//}

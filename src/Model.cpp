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

#include "Model.h"

#include <iostream>
#include <string>

#include <Eigen/Geometry>



Model::Model() {
}

Model::Model(const std::string& fileName) {
    getModelFromYamlFile(fileName);
}

Model::~Model() {
}

void Model::getModelFromYamlFile(const std::string &fileName) {

    // before reload model, need to remove the previous actors. by think 2023.12.25
    removeRobotModel();

    //parse yaml file
    YAML::Node config = YAML::LoadFile(fileName);

    std::string dir = fileName.substr(0, fileName.find_last_of('/'));

    if(config["robot"].IsDefined()) {
        std::cout << "robot info: " << std::endl;
    }

    YAML::Node info = config["robot"];
    assert(info.Type() == YAML::NodeType::Sequence);

    std::cout << "info size is: " << info.size() << std::endl;

    _freedom = info.size(); // The robot's freedom
    _linkGrp.resize(_freedom + 1); // the link is _freedom + 1, because of the base link4
//    _jntRads.resize(_freedom + 1); // _jntRads[0] is always 0

    for (std::size_t j = 0; j < info.size(); ++j) {
        _linkGrp[j].setName(info[j]["name"].as<std::string>()); //set name
        _linkGrp[j].setOrder(info[j]["order"].as<int>()); // set order

        if(info[j]["type"].as<std::string>() == "unknown") {
            _linkGrp[j].setType(Link::UNKNOWN); // set type
            _freedom--;
        } else if(info[j]["type"].as<std::string>() == "continuous") {
            _linkGrp[j].setType(Link::CONTINUOUS); // set type
        } else if(info[j]["type"].as<std::string>() == "prismatic") {
            _linkGrp[j].setType(Link::PRISMATIC); // set type
        } else if(info[j]["type"].as<std::string>() == "floating") {
            _linkGrp[j].setType(Link::FLOATING); // set type
        } else if(info[j]["type"].as<std::string>() == "planar") {
            _linkGrp[j].setType(Link::PLANAR); // set type
        } else if(info[j]["type"].as<std::string>() == "fixed") {
            _linkGrp[j].setType(Link::FIXED); // set type
            _freedom--;
        }

        if(info[j]["translate"].IsDefined())
            _linkGrp[j].setTranslate(info[j]["translate"][0].as<double>(),
                                 info[j]["translate"][1].as<double>(),
                                 info[j]["translate"][2].as<double>());

        if(info[j]["rotate"].IsDefined())
            _linkGrp[j].setRotate(info[j]["rotate"][0].as<double>(),
                              info[j]["rotate"][1].as<double>(),
                              info[j]["rotate"][2].as<double>());

        if(info[j]["angleAxis"].IsDefined())
            _linkGrp[j].setAngleAxis(info[j]["angleAxis"][0].as<int>(),
                                     info[j]["angleAxis"][1].as<int>(),
                                     info[j]["angleAxis"][2].as<int>());

        if(info[j]["translateLink"].IsDefined())
            _linkGrp[j].setTranslateLink(info[j]["translateLink"][0].as<double>(),
                                     info[j]["translateLink"][1].as<double>(),
                                     info[j]["translateLink"][2].as<double>());

        if(info[j]["rotateLink"].IsDefined())
            _linkGrp[j].setRotateLink(info[j]["rotateLink"][0].as<double>(),
                                     info[j]["rotateLink"][1].as<double>(),
                                      info[j]["rotateLink"][2].as<double>());

        if(info[j]["mesh"].IsDefined() && !info[j]["mesh"].IsNull()) {
            _linkGrp[j].setMesh(dir+'/'+info[j]["mesh"].as<std::string>());
        }

        std::cout << " - ";
        std::cout << "\t name: " << info[j]["name"] << std::endl;
        std::cout << "\t order: " << info[j]["order"].as<int>() << std::endl;
        if(info[j]["translate"].IsDefined())
            std::cout << "\t translate: " << info[j]["translate"][0].as<double>()
                      << ", " << info[j]["translate"][1].as<double>()
                      << ", " << info[j]["translate"][2].as<double>() << std::endl;
        if(info[j]["angleAxis"].IsDefined())
            std::cout << "\t angleAxis: " << info[j]["angleAxis"][0].as<int>()
                      << ", " << info[j]["angleAxis"][1].as<int>()
                      << ", " << info[j]["angleAxis"][2].as<int>() << std::endl;
//        std::cout << "\t mesh: " << info[j]["mesh"]  << " size: " << _linkGrp[j].getPointCloudCount() << std::endl;
    }

    std::cout << "robot freedom is " << _freedom << std::endl;

    _points = vtkPoints::New();
    _traj = vtkActor::New();

    _traj->GetProperty()->SetColor(73/255.0, 111/255.0, 255/255.0);
    _traj->GetProperty()->SetLineWidth(3);

    _renderer->AddActor(_traj);

//    std::vector<double> temp(_freedom, 0);
//    updateModel(temp);

    std::cout << "Model is loaded." << std::endl;
    addToRenderer(_renderer); //add model to renderer and the model will be seen
}

void Model::removeRobotModel()
{
    for(size_t i = 0; i < _linkGrp.size(); i++) {
        _renderer->RemoveActor(_linkGrp[i].actor);
        _renderer->RemoveActor(_linkGrp[i].axesActor);
    }

    _renderer->RemoveActor(_traj);
}

void Model::updateModel(std::vector<double> &jointRads)
{
//    if(jointRads.size() != static_cast<size_t>(_freedom)) {
//        std::cerr << "The joints number is not equal to the freedom." << std::endl;
//        return;
//    }

//    _linkGrp[0].setAngle(0);
    int jnt_id = 0;
    for(size_t i = 0; i < _linkGrp.size(); i++) {
        if(_linkGrp[i].getType() == Link::UNKNOWN || _linkGrp[i].getType() == Link::FIXED){

        } else if(_linkGrp[i].getType() == Link::CONTINUOUS || _linkGrp[i].getType() == Link::REVOLUTE) {
            _linkGrp[i].setAngle(jointRads[jnt_id++]);
        } else if(_linkGrp[i].getType() == Link::PRISMATIC) {

        }
    }

    Eigen::Transform<double, 3, Eigen::Affine> t_j(Eigen::Scaling(1.0)); // joint transform
    Eigen::Transform<double, 3, Eigen::Affine> t_l(Eigen::Scaling(1.0)); // link transform

    for(int i = 0; i < _linkGrp.size(); i++) {

        t_j *= Eigen::Translation3d(_linkGrp[i].getTranslate());

        t_j *= Eigen::AngleAxisd(_linkGrp[i].getRotate()[2], Eigen::Vector3d::UnitZ());
        t_j *= Eigen::AngleAxisd(_linkGrp[i].getRotate()[1], Eigen::Vector3d::UnitY());
        t_j *= Eigen::AngleAxisd(_linkGrp[i].getRotate()[0], Eigen::Vector3d::UnitX());
        t_j *= Eigen::AngleAxisd( _linkGrp[i].getAngle(),_linkGrp[i].getAngleAxis());


        auto mat = t_j.matrix();
        vtkNew<vtkMatrix4x4> vm;
        for(size_t i = 0; i < 4; i++) {
            for(size_t j = 0; j < 4; j++) {
                vm->SetElement(i,j, mat(i, j));
            }
        }

        vtkNew<vtkTransform> vt1; //Axes Tranform
        vt1->SetMatrix(vm);

        _linkGrp[i].setAxesActorTransform(vt1);  // Axes on Joint

        t_l = t_j * Eigen::Translation3d(_linkGrp[i].getTranslateLink());

        t_l *= Eigen::AngleAxisd(_linkGrp[i].getRotateLink()[2], Eigen::Vector3d::UnitZ());
        t_l *= Eigen::AngleAxisd(_linkGrp[i].getRotateLink()[1], Eigen::Vector3d::UnitY());
        t_l *= Eigen::AngleAxisd(_linkGrp[i].getRotateLink()[0], Eigen::Vector3d::UnitX());

        mat = t_l.matrix();

        for(size_t i = 0; i < 4; i++) {
            for(size_t j = 0; j < 4; j++) {
                vm->SetElement(i,j, mat(i, j));
            }
        }

        vtkNew<vtkTransform> vt2; //Link Transform
        vt2->SetMatrix(vm);
        _linkGrp[i].setLinkActorTransform(vt2); //vt2 will change with actor
    }

    //trajectory

    if(_isTrajVisible) {
        auto transform = _linkGrp[_linkGrp.size() - 1].axesActor->GetUserTransform();
        // 获取变换矩阵
        vtkSmartPointer<vtkMatrix4x4> matrix = transform->GetMatrix();

        // 获取平移部分
        double p[3];
        if(_points->GetNumberOfPoints() > 0)
            _points->GetPoint(_points->GetNumberOfPoints() - 1, p);
        if(fabs(matrix->GetElement(0, 3) - p[0]) < 1e-6 && fabs(matrix->GetElement(1, 3) - p[1]) < 1e-6 && fabs(matrix->GetElement(1, 3) - p[1]) < 1e-6) {

        } else {
            _points->InsertNextPoint( matrix->GetElement(0, 3), matrix->GetElement(1, 3),  matrix->GetElement(2, 3));
        }

        if(_points->GetNumberOfPoints() > 500) {
            deleteFirstPoint();
        }
        vtkNew<vtkPolyLine> _line;
        _line->GetPointIds()->SetNumberOfIds(_points->GetNumberOfPoints());
        for (vtkIdType i = 0; i < _points->GetNumberOfPoints(); i++) {
            _line->GetPointIds()->SetId(i, i);
        }

        vtkNew<vtkCellArray> _lines;
        _lines->InsertNextCell(_line);

        vtkNew<vtkPolyData> polyData;
        polyData->SetPoints(_points);
        polyData->SetLines(_lines);

        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(polyData);

        _traj->SetMapper(mapper);
    }
    else {
        // _points->SetNumberOfPoints(0);
        if(_points)
            _points->SetNumberOfPoints(0);
    }

}

void Model::addToRenderer(vtkRenderer *renderer)
{
    for(size_t i = 0; i < _linkGrp.size(); i++) {
        renderer->AddActor(_linkGrp[i].actor);
        renderer->AddActor(_linkGrp[i].axesActor);
    }

//    renderer->AddActor(_traj);
}

void Model::setAxesVisiblity(bool isVisible)
{
    for(size_t i = 0; i < _linkGrp.size(); i++) {
        _linkGrp[i].setAxesVisibility(isVisible);
    }
}

void Model::setMeshVisibility(bool isMesh)
{
    for(size_t i = 0; i < _linkGrp.size(); i++) {
        _linkGrp[i].setMeshVisibility(isMesh);
    }
}

void Model::setTrajVisiblity(bool isVisible)
{
    _isTrajVisible = isVisible;
}

void Model::deleteFirstPoint()
{
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    for(vtkIdType i = 1; i < _points->GetNumberOfPoints(); i++)
    {
        double p[3];
        _points->GetPoint(i,p);
        newPoints->InsertNextPoint(p);
    }

    _points->ShallowCopy(newPoints);
}

void Model::setRenderer(vtkRenderer *renderer) {
    _renderer = renderer;
}





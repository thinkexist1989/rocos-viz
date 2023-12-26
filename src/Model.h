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

#ifndef MODEL_H
#define MODEL_H

#include "Link.h"
#include <vector>
#include <yaml-cpp/yaml.h>

#include <vtkRenderer.h>

class Model {

public:
    /**
     * @brief The Model
     * @param fileName The yaml file
     */
    Model(const std::string& fileName);

    Model();

    ~Model();

    void getModelFromYamlFile(const std::string& fileName); // Load model from file
    void removeRobotModel();

//    void addLink(Link link, int order);

    /**
     * @brief Get the number of links
     *
     * @return int
     */
    inline int getFreedom() const { return _freedom; }

    void updateModel(std::vector<double> &jointRads);

    void addToRenderer(vtkRenderer* renderer);

    void setRenderer(vtkRenderer* renderer);

    void setAxesVisiblity(bool isVisible);

    void setMeshVisibility(bool isMesh);            //是否以Mesh形式显示

private:
    /* data */
    std::vector<Link> _linkGrp; //Save all the links of manipulator
//    std::vector<double> _jntRads; // Save the joint state (radians)
    int _freedom = 0;

    vtkRenderer* _renderer = nullptr;

};

#endif //MODEL_H

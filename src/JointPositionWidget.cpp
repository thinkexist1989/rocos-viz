#include "JointPositionWidget.h"
#include "ui_JointPositionWidget.h"

#include <iostream>

JointPositionWidget::JointPositionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JointPositionWidget)
{
    ui->setupUi(this);

    updateJointPosition(jointPos);

}

JointPositionWidget::~JointPositionWidget()
{
    delete ui;
}

void JointPositionWidget::setName(const QString &s)
{
    this->ui->jointPosValBar->setName(s);
}

void JointPositionWidget::setFactor(double f)
{
    factor = f;
}

void JointPositionWidget::updateJointPosition(double val)
{
    jointPos = val;
    this->ui->jointPosValBar->setValue(jointPos);
}

void JointPositionWidget::on_jointPosValBar_valueChanged(double value)
{
//    std::cout << "value change to: " << value << std::endl;
}

void JointPositionWidget::on_jointPosPlusButton_clicked()
{
//    std::cout << "Joint Joggging Plus" << std::endl;
    jointPos += step * factor;

    this->ui->jointPosValBar->setValue(jointPos);
}

void JointPositionWidget::on_jointPosMinusButton_clicked()
{
//    std::cout << "Joint Joggging Minus" << std::endl;
    jointPos -= step * factor;

    this->ui->jointPosValBar->setValue(jointPos);
}

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
    name = s;
    this->ui->jointPosValBar->setName(name);
}

void JointPositionWidget::setFactor(double f)
{
    factor = f;
}

void JointPositionWidget::updateJointPosition(double val)
{
    jointPos = val;
    this->ui->jointPosValBar->setValue(jointPos * r2d);
}

void JointPositionWidget::on_jointPosValBar_valueChanged(double value)
{
//    std::cout << "value change to: " << value << std::endl;
}

void JointPositionWidget::on_jointPosPlusButton_pressed()
{
    emit jointPositionJogging(id, 1);
}

void JointPositionWidget::on_jointPosPlusButton_released()
{
    emit jointPositionJogging(id, 0);
}

void JointPositionWidget::on_jointPosMinusButton_pressed()
{
    emit jointPositionJogging(id, -1);
}


void JointPositionWidget::on_jointPosMinusButton_released()
{
    emit jointPositionJogging(id, 0);
}

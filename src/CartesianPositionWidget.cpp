#include "CartesianPositionWidget.h"
#include "ui_CartesianPositionWidget.h"

#include <iostream>
#include <QTimer>

CartesianPositionWidget::CartesianPositionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CartesianPositionWidget)
{
    ui->setupUi(this);

    // timerPlus = new QTimer(this);
    // connect(timerPlus, &QTimer::timeout, this, [=](){
    //     emit cartesianJogging(id, 1);
    // });

    // timerMinus = new QTimer(this);
    // connect(timerMinus, &QTimer::timeout, this, [=](){
    //     emit cartesianJogging(id, -1);

    // });

    this->ui->cartesianValBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    updateVal(cartesianVal);

}

CartesianPositionWidget::~CartesianPositionWidget()
{
    delete ui;
}

void CartesianPositionWidget::updateDisp()
{
    this->ui->cartesianValBar->setFormat(tr("%1").arg(cartesianVal * r2d, 0 ,'f', 3));
}

void CartesianPositionWidget::setName(const QString &s)
{
    name = s;
    ui->name->setText(name);
    updateDisp();
}

void CartesianPositionWidget::setFactor(double f)
{
    factor = f;
}

void CartesianPositionWidget::updateVal(double val)
{
    cartesianVal = val;
    this->ui->cartesianValBar->setValue(cartesianVal * r2d);
    updateDisp();
}

void CartesianPositionWidget::on_cartesianPlusButton_pressed()
{
//    std::cout << "pressed+++" << std::endl;
    // timerPlus->start(10);
}

void CartesianPositionWidget::on_cartesianPlusButton_released()
{
//    std::cout << "release+++" << std::endl;
    // timerPlus->stop();

}

void CartesianPositionWidget::on_cartesianMinusButton_pressed()
{
//    std::cout << "pressed---" << std::endl;
    // timerMinus->start(20);

}

void CartesianPositionWidget::on_cartesianMinusButton_released()
{
//    std::cout << "release---" << std::endl;
    // timerMinus->stop();

}

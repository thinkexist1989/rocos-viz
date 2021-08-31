#include "CartesianPositionWidget.h"
#include "ui_CartesianPositionWidget.h"

#include <iostream>
#include <QTimer>

CartesianPositionWidget::CartesianPositionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CartesianPositionWidget)
{
    ui->setupUi(this);

    timerPlus = new QTimer(this);
    connect(timerPlus, &QTimer::timeout, this, [=](){
        cartesianVal += step * factor;

        val = (val + (uint16_t)(progressStep*factor)) & 0xFFFF;
    //    std::cout << "val" << (int)val << std::endl;

        updateDisp();

       this->ui->cartesianValBar->setValue(val);
    });

    timerMinus = new QTimer(this);
    connect(timerMinus, &QTimer::timeout, this, [=](){
        cartesianVal -= step * factor;

        val = (val - (uint16_t)(progressStep*factor)) & 0xFFFF;
    //    std::cout << "val" << (int)val << std::endl;

        updateDisp();

        this->ui->cartesianValBar->setValue(val);
    });

    updateCartesianPosition(cartesianVal);

}

CartesianPositionWidget::~CartesianPositionWidget()
{
    delete ui;
}

void CartesianPositionWidget::updateDisp()
{
    this->ui->cartesianValBar->setFormat(tr(" %1 : %2").arg(name).arg(cartesianVal));
}

void CartesianPositionWidget::setName(const QString &s)
{
    name = s;
    updateDisp();
}

void CartesianPositionWidget::setFactor(double f)
{
    factor = f;
}

void CartesianPositionWidget::updateCartesianPosition(double val)
{
    cartesianVal = val;
}

void CartesianPositionWidget::on_cartesianPlusButton_clicked()
{
//    cartesianVal += step;

//    val = (val + (uint16_t)(progressStep*factor)) & 0xFFFF;
////    std::cout << "val" << (int)val << std::endl;

//    updateDisp();

//   this->ui->cartesianValBar->setValue(val);
}

void CartesianPositionWidget::on_cartesianMinusButton_clicked()
{
//    cartesianVal -= step;

//    val = (val - (uint16_t)(progressStep*factor)) & 0xFFFF;
////    std::cout << "val" << (int)val << std::endl;

//    updateDisp();

//    this->ui->cartesianValBar->setValue(val);
}

void CartesianPositionWidget::on_cartesianPlusButton_pressed()
{
//    std::cout << "pressed+++" << std::endl;
    timerPlus->start(10);
}

void CartesianPositionWidget::on_cartesianPlusButton_released()
{
//    std::cout << "release+++" << std::endl;

    timerPlus->stop();
    val = 0;
    this->ui->cartesianValBar->setValue(val);
}

void CartesianPositionWidget::on_cartesianMinusButton_pressed()
{
//    std::cout << "pressed---" << std::endl;
    timerMinus->start(10);
}

void CartesianPositionWidget::on_cartesianMinusButton_released()
{
//    std::cout << "release---" << std::endl;
    timerMinus->stop();
    val = 0;
    this->ui->cartesianValBar->setValue(val);
}

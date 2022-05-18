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

    this->ui->cartesianValBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    updateVal(cartesianVal);

}

CartesianPositionWidget::~CartesianPositionWidget()
{
    delete ui;
}

void CartesianPositionWidget::updateDisp()
{
    this->ui->cartesianValBar->setFormat(tr(" %1 : %2").arg(name).arg(cartesianVal * r2d, 0 ,'f', 3));
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

void CartesianPositionWidget::updateVal(double val)
{
    cartesianVal = val;
    this->ui->cartesianValBar->setValue(cartesianVal * r2d);
    updateDisp();
}

void CartesianPositionWidget::on_cartesianPlusButton_pressed()
{
//    std::cout << "pressed+++" << std::endl;
//    timerPlus->start(10);
    emit cartesianJogging(id, 1);
}

void CartesianPositionWidget::on_cartesianPlusButton_released()
{
//    std::cout << "release+++" << std::endl;

//    timerPlus->stop();
//    val = 0;
//    this->ui->cartesianValBar->setValue(val);

// TODO: 使用了QButton的AutoRepeat，相当于每隔一段时间自动按下并抬起按键，导致released也一直调用，所以屏蔽了
//    emit cartesianJogging(id, 0);
//    std::cout << "Cart Pos Minus Released" << std::endl;

}

void CartesianPositionWidget::on_cartesianMinusButton_pressed()
{
//    std::cout << "pressed---" << std::endl;
//    timerMinus->start(10);
    emit cartesianJogging(id, -1);
}

void CartesianPositionWidget::on_cartesianMinusButton_released()
{
//    std::cout << "release---" << std::endl;
//    timerMinus->stop();
//    val = 0;
//    this->ui->cartesianValBar->setValue(val);

// TODO: 使用了QButton的AutoRepeat，相当于每隔一段时间自动按下并抬起按键，导致released也一直调用，所以屏蔽了
//    emit cartesianJogging(id, 0);
//    std::cout << "Cart Pos Minus Released" << std::endl;
}

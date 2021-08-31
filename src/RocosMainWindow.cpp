#include <RocosMainWindow.h>
#include "./ui_RocosMainWindow.h"

RocosMainWindow::RocosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RocosMainWindow)
{
    ui->setupUi(this);

    ui->Joint1PosWidget->setName("#1 JOINT");
    ui->Joint2PosWidget->setName("#2 JOINT");
    ui->Joint3PosWidget->setName("#3 JOINT");
    ui->Joint4PosWidget->setName("#4 JOINT");
    ui->Joint5PosWidget->setName("#5 JOINT");
    ui->Joint6PosWidget->setName("#6 JOINT");
    ui->Joint7PosWidget->setName("#7 JOINT");

    ui->cartesianXWidget->setName("X");
    ui->cartesianYWidget->setName("Y");
    ui->cartesianZWidget->setName("Z");
    ui->cartesianRollWidget->setName("ROLL");
    ui->cartesianPitchWidget->setName("PITCH");
    ui->cartesianYawWidget->setName("YAW");

    ui->speedPercent->setText(tr("50%"));

}

RocosMainWindow::~RocosMainWindow()
{
    delete ui;
}


void RocosMainWindow::on_pushButton_clicked()
{
    std::vector<double> jntRads;
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    this->ui->visualWidget->setJointPos(jntRads);
}

void RocosMainWindow::on_axesCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) {
        this->ui->visualWidget->setJointAxesVisibility(true);
    }
    else {
        this->ui->visualWidget->setJointAxesVisibility(false);
    }
}

void RocosMainWindow::on_speedSlider_valueChanged(int value)
{
    double f = value / 1000.0;
    ui->speedPercent->setText(tr("%1%").arg((int)(f * 100)));

    ui->Joint1PosWidget->setFactor(f);
    ui->Joint2PosWidget->setFactor(f);
    ui->Joint3PosWidget->setFactor(f);
    ui->Joint4PosWidget->setFactor(f);
    ui->Joint5PosWidget->setFactor(f);
    ui->Joint6PosWidget->setFactor(f);
    ui->Joint7PosWidget->setFactor(f);

    ui->cartesianXWidget->setFactor(f);
    ui->cartesianYWidget->setFactor(f);
    ui->cartesianZWidget->setFactor(f);
    ui->cartesianRollWidget->setFactor(f);
    ui->cartesianPitchWidget->setFactor(f);
    ui->cartesianYawWidget->setFactor(f);
}

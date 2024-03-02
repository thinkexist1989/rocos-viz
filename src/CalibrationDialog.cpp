#include "CalibrationDialog.h"
#include "ui_CalibrationDialog.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QStyledItemDelegate>
CalibrationDialog::CalibrationDialog( ConnectDialog* ConnectDlg , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    connectDlg = ConnectDlg;
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}
void CalibrationDialog::on_exitButton_clicked()
{
    this->close();
}

void CalibrationDialog::on_Setpose1Button_clicked()
{
    // ui->pose1X->setText(QString::number(0));
    // ui->pose1Y->setText(QString::number(0));
    // ui->pose1Z->setText(QString::number(0));
    // ui->pose1RX->setText(QString::number(0));
    // ui->pose1RY->setText(QString::number(0));
    // ui->pose1RZ->setText(QString::number(0));
    qDebug() << "set_pose_frame";
    QVector<double> pose;
     int id=1;
    pose.push_back(ui->spinBox1_x->value() / m2mm);
    pose.push_back(ui->spinBox1_y->value() / m2mm);
    pose.push_back(ui->spinBox1_z->value() / m2mm);
    pose.push_back(ui->spinBox1_rx->value() / r2d);
    pose.push_back(ui->spinBox1_ry->value() / r2d);
    pose.push_back(ui->spinBox1_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox1_x->value()<<"y:"<<ui->spinBox1_y->value()<<"z:"<<ui->spinBox1_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox1_rx->value()<<"ry:"<<ui->spinBox1_ry->value()<<"rz:"<<ui->spinBox1_rz->value()<<std::endl;

    std::cout<<"pose1 set"<<std::endl;

    connectDlg->moveJ_IK(pose);
   
}
void CalibrationDialog::on_Setpose2Button_clicked()
{
    
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id=2;
    pose.push_back(ui->spinBox2_x->value() / m2mm);
    pose.push_back(ui->spinBox2_y->value() / m2mm);
    pose.push_back(ui->spinBox2_z->value() / m2mm);
    pose.push_back(ui->spinBox2_rx->value() / r2d);
    pose.push_back(ui->spinBox2_ry->value() / r2d);
    pose.push_back(ui->spinBox2_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox2_x->value()<<"y:"<<ui->spinBox2_y->value()<<"z:"<<ui->spinBox2_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox2_rx->value()<<"ry:"<<ui->spinBox2_ry->value()<<"rz:"<<ui->spinBox2_rz->value()<<std::endl;

    std::cout<<"pose2 set"<<std::endl;

    connectDlg->setpose(id,pose);

}
void CalibrationDialog::on_Setpose3Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id=3;
    pose.push_back(ui->spinBox3_x->value() / m2mm);
    pose.push_back(ui->spinBox3_y->value() / m2mm);
    pose.push_back(ui->spinBox3_z->value() / m2mm);
    pose.push_back(ui->spinBox3_rx->value() / r2d);
    pose.push_back(ui->spinBox3_ry->value() / r2d);
    pose.push_back(ui->spinBox3_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox3_x->value()<<"y:"<<ui->spinBox3_y->value()<<"z:"<<ui->spinBox3_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox3_rx->value()<<"ry:"<<ui->spinBox3_ry->value()<<"rz:"<<ui->spinBox3_rz->value()<<std::endl;

    std::cout<<"pose3 set"<<std::endl;

    connectDlg->setpose(id,pose);
}
void CalibrationDialog::on_Setpose4Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id=4;
    pose.push_back(ui->spinBox4_x->value() / m2mm);
    pose.push_back(ui->spinBox4_y->value() / m2mm);
    pose.push_back(ui->spinBox4_z->value() / m2mm);
    pose.push_back(ui->spinBox4_rx->value() / r2d);
    pose.push_back(ui->spinBox4_ry->value() / r2d);
    pose.push_back(ui->spinBox4_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox4_x->value()<<"y:"<<ui->spinBox4_y->value()<<"z:"<<ui->spinBox4_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox4_rx->value()<<"ry:"<<ui->spinBox4_ry->value()<<"rz:"<<ui->spinBox4_rz->value()<<std::endl;

    std::cout<<"pose4 set"<<std::endl;

    connectDlg->setpose(id,pose);
}
void CalibrationDialog::on_Setpose5Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id=5;
    pose.push_back(ui->spinBox5_x->value() / m2mm);
    pose.push_back(ui->spinBox5_y->value() / m2mm);
    pose.push_back(ui->spinBox5_z->value() / m2mm);
    pose.push_back(ui->spinBox5_rx->value() / r2d);
    pose.push_back(ui->spinBox5_ry->value() / r2d);
    pose.push_back(ui->spinBox5_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox5_x->value()<<"y:"<<ui->spinBox5_y->value()<<"z:"<<ui->spinBox5_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox5_rx->value()<<"ry:"<<ui->spinBox5_ry->value()<<"rz:"<<ui->spinBox5_rz->value()<<std::endl;

    std::cout<<"pose5 set"<<std::endl;

    connectDlg->setpose(id,pose);
}
void CalibrationDialog::on_Setpose6Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id=6;
    pose.push_back(ui->spinBox6_x->value() / m2mm);
    pose.push_back(ui->spinBox6_y->value() / m2mm);
    pose.push_back(ui->spinBox6_z->value() / m2mm);
    pose.push_back(ui->spinBox6_rx->value() / r2d);
    pose.push_back(ui->spinBox6_ry->value() / r2d);
    pose.push_back(ui->spinBox6_rz->value() / r2d);
    std::cout<<"x: "<<ui->spinBox6_x->value()<<"y:"<<ui->spinBox6_y->value()<<"z:"<<ui->spinBox6_z->value()<<std::endl;
    std::cout<<"rx: "<<ui->spinBox6_rx->value()<<"ry:"<<ui->spinBox6_ry->value()<<"rz:"<<ui->spinBox6_rz->value()<<std::endl;

    std::cout<<"pose6 set"<<std::endl;

    connectDlg->setpose(id,pose);
}

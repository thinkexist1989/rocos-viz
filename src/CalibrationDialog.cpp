#include "CalibrationDialog.h"
#include "ui_CalibrationDialog.h"
#include <iostream>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QStyledItemDelegate>
CalibrationDialog::CalibrationDialog(ConnectDialog *ConnectDlg, QWidget *parent) : QDialog(parent),
                                                                                   ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    connectDlg = ConnectDlg;
    calDlg=new CalDialog(connectDlg,this);
    
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

    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 1;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox1_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox1_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox1_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox1_rx->setValue(roll * r2d);
        ui->doubleSpinBox1_ry->setValue(pitch * r2d);
        ui->doubleSpinBox1_rz->setValue(yaw * r2d);
        // std::cout<<flange_.p.x()*m2mm<<","<<flange_.p.y()*m2mm<<","<<flange_.p.z()*m2mm<<","<<roll*r2d<<","<<pitch*r2d<<","<<yaw*r2d<<std::endl;
    }
    else
    {
        pose.push_back(ui->doubleSpinBox1_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox1_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox1_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox1_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox1_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox1_rz->value() / r2d);
    }

    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_Setpose2Button_clicked()
{

    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 2;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox2_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox2_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox2_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox2_rx->setValue(roll * r2d);
        ui->doubleSpinBox2_ry->setValue(pitch * r2d);
        ui->doubleSpinBox2_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->doubleSpinBox2_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox2_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox2_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox2_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox2_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox2_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_Setpose3Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 3;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox3_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox3_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox3_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox3_rx->setValue(roll * r2d);
        ui->doubleSpinBox3_ry->setValue(pitch * r2d);
        ui->doubleSpinBox3_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->doubleSpinBox3_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox3_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox3_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox3_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox3_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox3_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_Setpose4Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 4;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox4_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox4_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox4_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox4_rx->setValue(roll * r2d);
        ui->doubleSpinBox4_ry->setValue(pitch * r2d);
        ui->doubleSpinBox4_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->doubleSpinBox4_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox4_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox4_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox4_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox4_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox4_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_Setpose5Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 5;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox5_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox5_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox5_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox5_rx->setValue(roll * r2d);
        ui->doubleSpinBox5_ry->setValue(pitch * r2d);
        ui->doubleSpinBox5_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->doubleSpinBox5_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox5_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox5_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox5_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox5_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox5_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_Setpose6Button_clicked()
{
    qDebug() << "set_pose_frame";
    QVector<double> pose;
    int id = 6;
    if (!ui->autoButton->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());

        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->doubleSpinBox6_x->setValue(flange_.p.x() * m2mm);
        ui->doubleSpinBox6_y->setValue(flange_.p.y() * m2mm);
        ui->doubleSpinBox6_z->setValue(flange_.p.z() * m2mm);
        ui->doubleSpinBox6_rx->setValue(roll * r2d);
        ui->doubleSpinBox6_ry->setValue(pitch * r2d);
        ui->doubleSpinBox6_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->doubleSpinBox6_x->value() / m2mm);
        pose.push_back(ui->doubleSpinBox6_y->value() / m2mm);
        pose.push_back(ui->doubleSpinBox6_z->value() / m2mm);
        pose.push_back(ui->doubleSpinBox6_rx->value() / r2d);
        pose.push_back(ui->doubleSpinBox6_ry->value() / r2d);
        pose.push_back(ui->doubleSpinBox6_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_CalButton_clicked()
{
    qDebug() << "calibration";
    connectDlg->calibration("tool");
    QTimer *timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [=]() {
        // 停止定时器
        timer->stop();
    bool ErrorState=connectDlg->getCalibrationFeedback();
    std::cout<<"ErrorState:"<<ErrorState<<std::endl;
    if (ErrorState)
    {
    
        QMessageBox::warning(this, "警告", "计算失败，请检查机器人标定点！");

    }
    else
    {
        
        QMessageBox::information(this, "提示", "计算成功！");
        calDlg->exec();
    }
    // 删除定时器对象
        timer->deleteLater();
    });

    // 启动定时器，设置延时时间为300毫秒（即1秒）
    timer->start(300);
    
}
void CalibrationDialog::on_autoButton_clicked()
{
    qDebug() << "auto_calibration";
    
    ui->autoButton->setDisabled(true);
    ui->manualButton->setEnabled(true);
    QMessageBox::information(this, "提示", "自动模式切换成功！");
}
void CalibrationDialog::on_manualButton_clicked()
{
    qDebug() << "manual_calibration";
    ui->autoButton->setEnabled(true);
    ui->manualButton->setDisabled(true);
    QMessageBox::information(this, "提示", "手动模式切换成功！");
}
void  CalibrationDialog::on_getTool2flangeButton_clicked()
{
    qDebug() << "getTool2flange";
    auto tool_ = connectDlg->getTool2Falange();
    ui->t_tool_x->setValue(tool_.p.x() * m2mm);
    ui->t_tool_y->setValue(tool_.p.y() * m2mm);
    ui->t_tool_z->setValue(tool_.p.z() * m2mm);
    double roll, pitch, yaw;
    tool_.M.GetRPY(roll, pitch, yaw);
    ui->t_tool_rx->setValue(roll * r2d);
    ui->t_tool_ry->setValue(pitch * r2d);
    ui->t_tool_rz->setValue(yaw * r2d);
}
void CalibrationDialog::on_SetObjectButton1_clicked()
{
    qDebug() << "set_object_frame";
    QVector<double> pose;
    int id = 7;
    if (!ui->autoButton_object->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->object1_x->setValue(flange_.p.x() * m2mm);
        ui->object1_y->setValue(flange_.p.y() * m2mm);
        ui->object1_z->setValue(flange_.p.z() * m2mm);
        ui->object1_rx->setValue(roll * r2d);
        ui->object1_ry->setValue(pitch * r2d);
        ui->object1_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->object1_x->value() / m2mm);
        pose.push_back(ui->object1_y->value() / m2mm);
        pose.push_back(ui->object1_z->value() / m2mm);
        pose.push_back(ui->object1_rx->value() / r2d);
        pose.push_back(ui->object1_ry->value() / r2d);
        pose.push_back(ui->object1_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_SetObjectButton2_clicked()
{
    qDebug() << "set_object_frame";
    QVector<double> pose;
    int id = 8;
    if (!ui->autoButton_object->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->object2_x->setValue(flange_.p.x() * m2mm);
        ui->object2_y->setValue(flange_.p.y() * m2mm);
        ui->object2_z->setValue(flange_.p.z() * m2mm);
        ui->object2_rx->setValue(roll * r2d);
        ui->object2_ry->setValue(pitch * r2d);
        ui->object2_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->object2_x->value() / m2mm);
        pose.push_back(ui->object2_y->value() / m2mm);
        pose.push_back(ui->object2_z->value() / m2mm);
        pose.push_back(ui->object2_rx->value() / r2d);
        pose.push_back(ui->object2_ry->value() / r2d);
        pose.push_back(ui->object2_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}
void CalibrationDialog::on_SetObjectButton3_clicked()
{
    qDebug() << "set_object_frame";
    QVector<double> pose;
    int id = 9;
    if (!ui->autoButton_object->isEnabled())
    {
        auto flange_ = connectDlg->getFlangePose();
        pose.push_back(flange_.p.x());
        pose.push_back(flange_.p.y());
        pose.push_back(flange_.p.z());
        double roll, pitch, yaw;
        flange_.M.GetRPY(roll, pitch, yaw);
        pose.push_back(roll);
        pose.push_back(pitch);
        pose.push_back(yaw);
        ui->object3_x->setValue(flange_.p.x() * m2mm);
        ui->object3_y->setValue(flange_.p.y() * m2mm);
        ui->object3_z->setValue(flange_.p.z() * m2mm);
        ui->object3_rx->setValue(roll * r2d);
        ui->object3_ry->setValue(pitch * r2d);
        ui->object3_rz->setValue(yaw * r2d);
    }
    else
    {
        pose.push_back(ui->object3_x->value() / m2mm);
        pose.push_back(ui->object3_y->value() / m2mm);
        pose.push_back(ui->object3_z->value() / m2mm);
        pose.push_back(ui->object3_rx->value() / r2d);
        pose.push_back(ui->object3_ry->value() / r2d);
        pose.push_back(ui->object3_rz->value() / r2d);
    }
    connectDlg->setpose(id, pose);
}

void CalibrationDialog::on_autoButton_object_clicked()
{
    qDebug() << "auto_calibration";
    ui->autoButton_object->setDisabled(true);
    ui->manualButton_object->setEnabled(true);
    QMessageBox::information(this, "提示", "自动模式切换成功！");
}
void CalibrationDialog::on_manualButton_object_clicked()
{
    qDebug() << "manual_calibration";
    ui->autoButton_object->setEnabled(true);
    ui->manualButton_object->setDisabled(true);
    QMessageBox::information(this, "提示", "手动模式切换成功！");
}
void CalibrationDialog::on_CalButton_object_clicked()
{
    qDebug() << "calibration";
    QTimer *timer = new QTimer(this);
    connectDlg->calibration("object");
      connect(timer, &QTimer::timeout, this, [=]() {
        // 停止定时器
        timer->stop();
    bool ErrorState=connectDlg->getCalibrationFeedback();
    std::cout<<"ErrorState:"<<ErrorState<<std::endl;
    if (ErrorState)
    {
    
        QMessageBox::warning(this, "警告", "计算失败，请检查机器人标定点！");

    }
    else
    {
        
        QMessageBox::information(this, "提示", "计算成功！");
        auto frame="object";
        calDlg->setRefenceFrame(frame);
        calDlg->exec();
    }
          // 删除定时器对象
        timer->deleteLater();
    });
    timer->start(300);

}
void CalibrationDialog::on_getObject2flangeButton_clicked()
{
    qDebug() << "getTool2flange";
    auto tool_ = connectDlg->getObject2Falange();
    ui->object4_x->setValue(tool_.p.x() * m2mm);
    ui->object4_y->setValue(tool_.p.y() * m2mm);
    ui->object4_z->setValue(tool_.p.z() * m2mm);
    double roll, pitch, yaw;
    tool_.M.GetRPY(roll, pitch, yaw);
    ui->object4_rx->setValue(roll * r2d);
    ui->object4_ry->setValue(pitch * r2d);
    ui->object4_rz->setValue(yaw * r2d);
}

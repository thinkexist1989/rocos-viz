#include "Caldialog.h"
#include "ui_Caldialog.h"
#include <iostream>

CalDialog::CalDialog(ConnectDialog *ConnectDlg, QWidget *parent) : QDialog(parent),
                                                                   ui(new Ui::CalDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    connectDlg = ConnectDlg;
    connect(connectDlg, &ConnectDialog::newStateComming, this, &CalDialog::updatePoseOut);
}

CalDialog::~CalDialog()
{
    delete ui;
}
void CalDialog::on_exitButton_clicked()
{
    this->close();
}
void CalDialog::updatePoseOut()
{

    auto poseout_ = connectDlg->getPoseOut();

    double roll, pitch, yaw;
    poseout_.M.GetRPY(roll, pitch, yaw);
    QString arrayString;

    // 将数组元素逐个添加到字符串中
    arrayString += "是否将设置[";
    arrayString += QString::number(poseout_.p.x());
    arrayString += ",";
    arrayString += QString::number(poseout_.p.y());
    arrayString += ",";
    arrayString += QString::number(poseout_.p.z());
    arrayString += ",";
    arrayString += QString::number(roll);
    arrayString += ",";
    arrayString += QString::number(pitch);
    arrayString += ",";
    arrayString += QString::number(yaw);
    if(refenceframe == "tool")
    {
        arrayString += "]为工具坐标系相对于变换矩阵？\n";
    }
    else if(refenceframe=="object")
    {
        arrayString += "]为工件坐标系相对于变换矩阵？\n";
    }
    

    // 将字符串写入 QTextEdit
    ui->textEdit->setText(arrayString);
}
void CalDialog::on_acceptButton_clicked()
{
    if(refenceframe == "tool")
    {
        connectDlg->setToolPose();
    }
    else if(refenceframe=="object")
    {
        connectDlg->setObjectPose();
    }
    
    this->close();
}
void CalDialog::setRefenceFrame(std::string frame)
{
    refenceframe = frame;
}

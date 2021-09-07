#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

#include <QDebug>

#include <Protocol.h> //通讯协议

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    tcpSocket = new QTcpSocket;

    connect(tcpSocket, &QTcpSocket::disconnected, this, [=](){connectedToRobot(false);});

}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::on_connectButton_clicked()
{
    connectedToRobot(true);
}

void ConnectDialog::on_ipAddressEdit_textChanged(const QString &ip)
{
    ipAddress = ip;

//    qDebug() << "IP Address is: " << ip;
}

void ConnectDialog::on_portEdit_textChanged(const QString &p)
{
    port = p.toInt();

//    qDebug() << "Port is: " << port;
}

void ConnectDialog::connectedToRobot(bool con)
{
    if(con) { //连接
        if(tcpSocket == Q_NULLPTR) {
            tcpSocket = new QTcpSocket;
        }

        tcpSocket->connectToHost(ipAddress, port);

        if (tcpSocket->waitForConnected(2000)) { //链接等待2s
            qDebug("Robot controller is connected!");
            accept(); // 对话框返回accept
        }
        else {
            reject(); // 对话框返回reject
        }
    }
    else { //断开连接
        if(tcpSocket == Q_NULLPTR) {
            reject(); // 未连接
            return;
        }

        tcpSocket->disconnectFromHost();

        if (tcpSocket->state() == QAbstractSocket::UnconnectedState ||
            tcpSocket->waitForDisconnected(2000)) {
            qDebug("Robot controller is disconnected!");
            reject();
        }
    }
}

void ConnectDialog::setRobotEnabled(bool enabled)
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        qDebug() << "Connection to robot is not established!!";
        return;
    }

    if(enabled){
        tcpSocket->write(ROBOT_ENABLE);
        isRobotEnabled = true;
    }
    else {
        tcpSocket->write(ROBOT_DISABLE);
        isRobotEnabled = false;
    }
}

void ConnectDialog::jointJogging(int id, int dir)
{
    QByteArray data;

    if(dir > 0) {// 正向运动
        qDebug() << "Joint " << id << " jogging++";
        switch(id) {
        case 1:
            data = J1_P;
            break;
        case 2:
            data = J2_P;
            break;
        case 3:
            data = J3_P;
            break;
        case 4:
            data = J4_P;
            break;
        case 5:
            data = J5_P;
            break;
        case 6:
            data = J6_P;
            break;
        case 7:
            data = J7_P;
            break;
        default:
            break;
        }
    }
    else if(dir < 0) {// 负向运动
        qDebug() << "Joint " << id << " jogging--";
        switch(id) {
        case 1:
            data = J1_N;
            break;
        case 2:
            data = J2_N;
            break;
        case 3:
            data = J3_N;
            break;
        case 4:
            data = J4_N;
            break;
        case 5:
            data = J5_N;
            break;
        case 6:
            data = J6_N;
            break;
        case 7:
            data = J7_N;
            break;
        default:
            break;
        }
    }
    else if(dir == 0){// 停止运动
        switch(id) {
        case 1:
            data = J1_Z;
            break;
        case 2:
            data = J2_Z;
            break;
        case 3:
            data = J3_Z;
            break;
        case 4:
            data = J4_Z;
            break;
        case 5:
            data = J5_Z;
            break;
        case 6:
            data = J6_Z;
            break;
        case 7:
            data = J7_Z;
            break;
        default:
            break;
        }
    }

    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        qDebug() << "Connection to robot is not established!!";
        return;
    }
    else {
        tcpSocket->write(data);
    }
}

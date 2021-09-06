#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

#include <QDebug>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

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
    if(tcpSocket == Q_NULLPTR) {
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

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
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ConnectDialog::parseRobotMsg); //解析机器人状态

    timerState = new QTimer(this);
    connect(timerState, &QTimer::timeout, this, &ConnectDialog::getRobotState); //获取机器人状态

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
}

void ConnectDialog::on_portEdit_textChanged(const QString &p)
{
    port = p.toInt();
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

            getJointSpeedScaling(); //查询一下当前速度缩放

            timerState->start(100);

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

void ConnectDialog::setJointSpeedScaling(double factor)
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    QByteArray ba = "sc0!v";
    ba.append(QString::number(factor));
    ba.append("!\0");

    tcpSocket->write(ba);

//    QTimer::singleShot(50, this, &ConnectDialog::getJointSpeedScaling);

//    qDebug() << "Set speed scaling: " << ba;
}

void ConnectDialog::getJointSpeedScaling()
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    tcpSocket->write(GET_JNTSPD);

}

void ConnectDialog::setCartesianSpeedScaling(double factor)
{
}

void ConnectDialog::getCartesianSpeedScaling()
{

}

void ConnectDialog::setToolSpeedScaling(double factor)
{
}

void ConnectDialog::getToolSpeedScaling()
{

}

void ConnectDialog::startScript(QString script)
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    QByteArray ba = "tv99!p";
    ba.append(script);
    ba.append("!\0");

    tcpSocket->write(ba);
}

void ConnectDialog::stopScript()
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    tcpSocket->write(LUA_STOP);
}

void ConnectDialog::pauseScript()
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    tcpSocket->write(LUA_PAUSE);
}

void ConnectDialog::continueScript()
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        return;
    }

    tcpSocket->write(LUA_CONTINUE);
}

void ConnectDialog::getRobotState()
{
    if(tcpSocket == Q_NULLPTR || !tcpSocket->isValid()) {
        qDebug() << "Connection to robot is not established!!";
        return;
    }

    tcpSocket->write(GET_INFO);
}

void ConnectDialog::parseRobotMsg()
{
    if(!tcpSocket->isValid()){
        return;
    }

    QByteArray ba = tcpSocket->readAll();

//    int size = ba.size();

    //机器人状态信息： av10.0!v20.0!bv3.0!v3.4!3.0!v3.4!3.0!v3.4!\0 ,单位为rad, mm a开头为关节 b开头为笛卡尔
    if(ba.startsWith("a")) {
        auto list = ba.split('b'); //<av10.0!v20.0!><v3.0!v3.4!v3.0!v3.4!v3.0!v3.4!>

        QByteArray jp = list[0];
        auto jointsArray =  jp.split('!'); // <av10.0><v20.0>
        QVector<double> jntPos;
        for(auto& j : jointsArray) {
            if(!j.isEmpty()) {
                int idx = j.indexOf('v'); //
                jntPos.push_back(j.mid(idx+1).toDouble());
            }
        }

        emit jointPositions(jntPos); //发送关节位置信号
//        qDebug() << "1: " << jntPos[0];

        QByteArray cp = list[1];
        auto cartArray =  cp.split('!'); // <v3.0!><v3.4!><v3.0!><v3.4!><v3.0!><v3.4!>
        QVector<double> pose;
        for(auto& c : cartArray) {
            if(!c.isEmpty()) {
                int idx = c.indexOf('v'); //
                pose.push_back(c.mid(idx+1).toDouble());
            }
        }

        emit cartPose(pose); //发送 笛卡尔位置信号
//        qDebug() << "x: " << pose[0] << "y: " << pose[1] << "z: " << pose[2] << "r: " << pose[3] << "p: " << pose[4] << "y: " << pose[5];

    }
    else if(ba.startsWith("sc")) { //速度配置信息， 百分比 sc0!v25.0!
        auto list = ba.split('!');
        for(auto& l : list) {
            if(!l.isEmpty()) {
                if(l.startsWith('v')) {
                    int idx = l.indexOf('v'); //
                    emit speedScaling(l.mid(idx+1).toDouble());
                    qDebug() << "Current speed scaling: " << l.mid(idx+1).toDouble();
                }
            }
        }
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

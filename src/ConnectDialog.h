#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

public slots:
    void on_connectButton_clicked();

    void on_ipAddressEdit_textChanged(const QString &ip);

    void on_portEdit_textChanged(const QString &p);

    void connectedToRobot(bool con);  //连接到机器人

    void setRobotEnabled(bool enabled); //操作：机器人是否上电

    void getRobotState(); // 操作：发送GET_INFO

    void parseRobotMsg(); // 解析返回的机器人信息

    void setJointSpeedScaling(double factor); //关节速度
    void getJointSpeedScaling(); //获取关节速度

    void setCartesianSpeedScaling(double factor); //笛卡尔速度
    void getCartesianSpeedScaling(); //获取 笛卡尔速度

    void setToolSpeedScaling(double factor); // 工具速度
    void getToolSpeedScaling(); //获取工具速度

    void startScript(QString script);
    void stopScript();
    void pauseScript();
    void continueScript();

private:
    Ui::ConnectDialog *ui;

    QTcpSocket* tcpSocket = Q_NULLPTR; //连接机器人控制器


    bool isRobotEnabled = false; //机器人默认不上电

    QTimer* timerState;

public:
    QString ipAddress = "192.168.0.99";
    int     port = 6666;

    inline bool isConnected() { return (tcpSocket != Q_NULLPTR) && (tcpSocket->isValid());}  //是否已经连接
    inline bool getRobotEnabled() {return isRobotEnabled;} //获取Enable状态

    void jointJogging(int id, int dir);

signals:
    void jointPositions(QVector<double>& jntPos); //解析到关节位置，发送 信号
    void cartPose(QVector<double>& pose); //解析到笛卡尔空间位置，发送 信号
    void speedScaling(double f100); // 速度缩放因数 25.0

};

#endif // CONNECTDIALOG_H

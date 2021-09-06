#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QTcpSocket>

#define ROBOT_ENABLE    "tv80!"
#define ROBOT_DISABLE   "tv112!"
#define LUA_START       "tv99!p/hanbing/app/Line0.MOV!"
#define LUA_STOP        "tv100!"
#define LUA_PAUSE       "tv115!"
#define LUA_CONTINUE    "tv103!"

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);
    ~ConnectDialog();

private slots:
    void on_connectButton_clicked();

    void on_ipAddressEdit_textChanged(const QString &ip);

    void on_portEdit_textChanged(const QString &p);

private:
    Ui::ConnectDialog *ui;

    QTcpSocket* tcpSocket = Q_NULLPTR; //连接机器人控制器


    bool isRobotEnabled = false; //机器人默认不上电

public:
    QString ipAddress = "192.168.0.99";
    int     port = 6666;

    void connectedToRobot(bool con);  //连接到机器人
    void setRobotEnabled(bool enabled); //机器人是否上电

};

#endif // CONNECTDIALOG_H

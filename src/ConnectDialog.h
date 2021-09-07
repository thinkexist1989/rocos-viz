#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QTcpSocket>

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
    void setRobotEnabled(bool enabled); //机器人是否上电

private:
    Ui::ConnectDialog *ui;

    QTcpSocket* tcpSocket = Q_NULLPTR; //连接机器人控制器


    bool isRobotEnabled = false; //机器人默认不上电

public:
    QString ipAddress = "192.168.0.99";
    int     port = 6666;


    inline bool getRobotEnabled() {return isRobotEnabled;} //获取Enable状态

    void jointJogging(int id, int dir);

};

#endif // CONNECTDIALOG_H

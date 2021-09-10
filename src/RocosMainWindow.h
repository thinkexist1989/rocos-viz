#ifndef ROCOSMAINWINDOW_H
#define ROCOSMAINWINDOW_H

#include <QMainWindow>
#include <ConnectDialog.h> //机器人控制链接
#include <ScriptDialog.h>  //LUA脚本对话框

#include <JointPositionWidget.h>
#include <CartesianPositionWidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class RocosMainWindow; }
QT_END_NAMESPACE

class RocosMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RocosMainWindow(QWidget *parent = nullptr);
    ~RocosMainWindow();

private slots:
    void on_pushButton_clicked();

    void on_axesCheckBox_stateChanged(int arg1);

    void on_speedSlider_valueChanged(int value);

    void on_actionEnabled_triggered();

    void on_actionConnected_triggered();

    void updateJointPos(QVector<double>& jntPos);
    void updateCartPose(QVector<double>& pose);

    void on_meshCheckBox_stateChanged(int arg1);

    void on_actionScript_triggered();

    void on_actionPlotter_triggered();

private:
    Ui::RocosMainWindow *ui;

    ConnectDialog* connectDlg = Q_NULLPTR;
    ScriptDialog*   scriptDlg  = Q_NULLPTR;

    QVector<JointPositionWidget*> jpWdgs;
    QVector<CartesianPositionWidget*> cpWdgs;

public:
    bool isRobotConnected = false; //机器人是否连接
    bool isRobotEnabled = false; //机器人默认不上电


};
#endif // ROCOSMAINWINDOW_H

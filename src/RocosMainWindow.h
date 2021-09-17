#ifndef ROCOSMAINWINDOW_H
#define ROCOSMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTime>

#include <ConnectDialog.h> //机器人控制链接
#include <ScriptDialog.h>  //LUA脚本对话框
#include <PlotDialog.h>    //曲线绘制对话框
#include <AboutDialog.h>  // 关于对话框

#include <JointPositionWidget.h>
#include <CartesianPositionWidget.h>

#include <Protocol.h>

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

    void on_actionAbout_triggered();

    void on_actionAxoView_triggered();

    void on_actionTopView_triggered();

    void on_actionFrontView_triggered();

    void on_actionRightView_triggered();

    void on_actionSetZero_triggered();

    void on_baseFrame_clicked(bool checked);

    void on_flangeFrame_clicked(bool checked);

    void on_actionAngleRep_triggered();

    void on_actionPosRep_triggered();

    void on_logEdit_customContextMenuRequested(const QPoint &pos);

private:
    Ui::RocosMainWindow *ui;

    QLabel*        timeLabel;
    QTime*         time;
    QTimer*        timer;

    ConnectDialog* connectDlg = Q_NULLPTR;
    ScriptDialog*  scriptDlg  = Q_NULLPTR;
    PlotDialog*    plotDlg    = Q_NULLPTR;
    AboutDialog*   aboutDlg   = Q_NULLPTR;

    QVector<JointPositionWidget*> jpWdgs;
    QVector<CartesianPositionWidget*> cpWdgs;

public:
    bool isRobotConnected = false; //机器人是否连接
    bool isRobotEnabled = false; //机器人默认不上电

    bool isAngleDegree = true; //默认角度显示
    bool isPosMM = true; //默认mm显示

    int  currentFrame = FRAME_BASE; // 默认为base系


};
#endif // ROCOSMAINWINDOW_H

#ifndef ROCOSMAINWINDOW_H
#define ROCOSMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QGroupBox>

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

    void on_axesCheckBox_toggled(bool checked);

    void on_speedSlider_valueChanged(int value);

    void on_actionEnabled_clicked();

    void on_actionConnected_clicked();

    void updateJointPos(QVector<double>& jntPos);
    void updateCartPose(QVector<double>& pose);

    void on_meshCheckBox_toggled(bool checked);

    void on_actionScript_triggered();

    void on_actionPlotter_triggered();

    void on_actionAbout_triggered();

    void on_actionAxoView_clicked();

    void on_actionTopView_clicked();

    void on_actionFrontView_clicked();

    void on_actionRightView_clicked();

    void on_actionSetZero_triggered();

    void on_baseFrame_clicked(bool checked);

    void on_flangeFrame_clicked(bool checked);

    void on_toolFrame_clicked(bool checked);

    void on_objectFrame_clicked(bool checked);

    void on_actionAngleRep_triggered();

    void on_actionPosRep_triggered();

//    void on_logEdit_customContextMenuRequested(const QPoint &pos);

    void updateRobotState();

    void on_MoveJ_clicked();
    void on_MoveJ_IK_clicked();
    void on_MoveL_FK_clicked();
    void on_MoveL_clicked();
    void on_MoveSpace0_clicked();

    void on_workingModeBox_currentIndexChanged(int index);

private:
    Ui::RocosMainWindow *ui;

//    QLabel*        timeLabel;
    QElapsedTimer* time;
    QTimer*        timer;

    ConnectDialog* connectDlg = Q_NULLPTR;
    ScriptDialog*  scriptDlg  = Q_NULLPTR;
    PlotDialog*    plotDlg    = Q_NULLPTR;
    AboutDialog*   aboutDlg   = Q_NULLPTR;

    QVector<JointPositionWidget*> jpWdgs;
    QVector<CartesianPositionWidget*> cpWdgs;

    QGroupBox *grpBox;
    bool safeOn = false;
    int      jogDir = 0; //点动方向 默认是BaseT 0 -> BaseR 1 -> FlangeT 2 -> FlangeR 3

public:
    bool isRobotConnected = false; //机器人是否连接
    bool isRobotEnabled = false; //机器人默认不上电

    bool isAngleDegree = true; //默认角度显示
    bool isPosMM = true; //默认mm显示

    int  currentFrame = FRAME_BASE; // 默认为base系

    double r2d = 180.0/M_PI; // 默认是角度显示
    double m2mm = 1000.0;    // 默认是mm显示


};
#endif // ROCOSMAINWINDOW_H

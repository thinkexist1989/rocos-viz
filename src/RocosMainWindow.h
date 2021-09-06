#ifndef ROCOSMAINWINDOW_H
#define ROCOSMAINWINDOW_H

#include <QMainWindow>
#include <ConnectDialog.h> //机器人控制链接

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

private:
    Ui::RocosMainWindow *ui;

    ConnectDialog* connectDlg = Q_NULLPTR;

public:
    bool isRobotConnected = false; //机器人是否连接
    bool isRobotEnabled = false; //机器人默认不上电


};
#endif // ROCOSMAINWINDOW_H

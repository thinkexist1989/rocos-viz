#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>
#include <QTcpSocket>

#include <ConnectDialog.h> //机器人控制链接
#include<Caldialog.h>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QGroupBox>
#include <Protocol.h>

namespace Ui {
class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog( ConnectDialog* ConnectDlg,QWidget *parent = nullptr);
    ~CalibrationDialog();
private slots:

    void on_exitButton_clicked();
    void on_Setpose1Button_clicked();
    void on_Setpose2Button_clicked();
    void on_Setpose3Button_clicked();
    void on_Setpose4Button_clicked();
    void on_Setpose5Button_clicked();
    void on_Setpose6Button_clicked();
    void on_autoButton_clicked();
    void on_manualButton_clicked();
    void on_CalButton_clicked();
    void on_getTool2flangeButton_clicked();
    void on_SetObjectButton1_clicked();
    void on_SetObjectButton2_clicked();
    void on_SetObjectButton3_clicked();
    void on_autoButton_object_clicked();
    void on_manualButton_object_clicked();
    void on_CalButton_object_clicked();
    void on_getObject2flangeButton_clicked();
    
private:
    Ui::CalibrationDialog *ui;
    ConnectDialog* connectDlg = Q_NULLPTR;
    CalDialog* calDlg = Q_NULLPTR;
    double r2d = 180.0/M_PI; // 默认是角度显示
    double m2mm = 1000.0;    // 默认是mm显示
};


#endif // CALIBRATIONDIALOG_H

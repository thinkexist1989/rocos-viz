#ifndef CALDIALOG_H
#define CALDIALOG_H

#include <QDialog>
#include <ConnectDialog.h> //机器人控制链接
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QGroupBox>
#include <Protocol.h>
namespace Ui {
class CalDialog;
}

class CalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalDialog(ConnectDialog* ConnectDlg,QWidget *parent = nullptr);
    ~CalDialog();
    void setRefenceFrame(std::string frame);
private slots:
    void on_exitButton_clicked();
    void updatePoseOut();
    void on_acceptButton_clicked();
    
    

private:
    Ui::CalDialog *ui;
    ConnectDialog* connectDlg = Q_NULLPTR;
    std::string refenceframe="tool";
};

#endif // CALDIALOG_H

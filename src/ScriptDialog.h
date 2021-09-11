#ifndef SCRIPTDIALOG_H
#define SCRIPTDIALOG_H

#include <QDialog>
#include <ConnectDialog.h>

namespace Ui {
class ScriptDialog;
}

class ScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptDialog(QWidget *parent = nullptr);
    ~ScriptDialog();

    void setConnectPtr(ConnectDialog* cdPtr) { connectPtr = cdPtr; }

private slots:
    void on_runButton_clicked();

    void on_stopButton_clicked();

    void on_pauseButton_clicked();

    void on_continueButton_clicked();

private:
    Ui::ScriptDialog *ui;

    QStringList texts;

    ConnectDialog* connectPtr = Q_NULLPTR;
};

#endif // SCRIPTDIALOG_H

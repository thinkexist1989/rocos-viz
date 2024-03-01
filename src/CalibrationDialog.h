#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

namespace Ui {
class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(QWidget *parent = nullptr);
    ~CalibrationDialog();
private slots:
    void on_exitButton_clicked();
private:
    Ui::CalibrationDialog *ui;
};

#endif // CALIBRATIONDIALOG_H

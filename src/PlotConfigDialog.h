#ifndef PLOTCONFIGDIALOG_H
#define PLOTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class PlotConfigDialog;
}

class PlotConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotConfigDialog(QWidget *parent = nullptr);
    ~PlotConfigDialog();

private:
    Ui::PlotConfigDialog *ui;
};

#endif // PLOTCONFIGDIALOG_H

#ifndef CALDIALOG_H
#define CALDIALOG_H

#include <QDialog>

namespace Ui {
class CalDialog;
}

class CalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalDialog(QWidget *parent = nullptr);
    ~CalDialog();

private:
    Ui::CalDialog *ui;
};

#endif // CALDIALOG_H

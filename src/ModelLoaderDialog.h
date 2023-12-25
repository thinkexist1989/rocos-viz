#ifndef MODELLOADERDIALOG_H
#define MODELLOADERDIALOG_H

#include <QDialog>

namespace Ui {
class ModelLoaderDialog;
}

class ModelLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelLoaderDialog(QWidget *parent = nullptr);
    ~ModelLoaderDialog();

private:
    Ui::ModelLoaderDialog *ui;
};

#endif // MODELLOADERDIALOG_H

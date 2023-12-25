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

    QString getCfgFileName();

private slots:
    void on_openFileButton_clicked();

    void on_uploadButton_clicked();

private:
    Ui::ModelLoaderDialog *ui;
    QString cfgFileName;
};

#endif // MODELLOADERDIALOG_H

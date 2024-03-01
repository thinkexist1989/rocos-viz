#include "CalibrationDialog.h"
#include "ui_CalibrationDialog.h"

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}
void AboutDialog::on_exitButton_clicked()
{
    this->close();
}

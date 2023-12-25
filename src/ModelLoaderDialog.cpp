#include "ModelLoaderDialog.h"
#include "ui_ModelLoaderDialog.h"

ModelLoaderDialog::ModelLoaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelLoaderDialog)
{
    ui->setupUi(this);
}

ModelLoaderDialog::~ModelLoaderDialog()
{
    delete ui;
}

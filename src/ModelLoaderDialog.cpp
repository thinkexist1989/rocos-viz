#include "ModelLoaderDialog.h"
#include "ui_ModelLoaderDialog.h"

#include <QFileDialog>

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

void ModelLoaderDialog::on_openFileButton_clicked()
{
    cfgFileName = QFileDialog::getOpenFileName(nullptr, "Configuration File", ".", "YAML (*.yaml)");

    if(cfgFileName.isEmpty()) {
        this->reject();
    }
    else{
        qDebug() << "configuration file: " << cfgFileName;
        this->accept();
    }
}


void ModelLoaderDialog::on_uploadButton_clicked()
{
    emit getRobotModel();
    this->reject();
}

QString ModelLoaderDialog::getCfgFileName()
{
    return cfgFileName;
}


void ModelLoaderDialog::on_removeButton_clicked()
{
    emit removeRobotModel();
    this->reject();
}


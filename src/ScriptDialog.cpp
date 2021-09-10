#include "ScriptDialog.h"
#include "ui_ScriptDialog.h"

ScriptDialog::ScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScriptDialog)
{
    ui->setupUi(this);

    texts << "/hanbing/app/Line0.MOV";
    ui->scriptComboBox->clear();
    ui->scriptComboBox->addItems(texts);
}

ScriptDialog::~ScriptDialog()
{
    delete ui;
}

void ScriptDialog::on_runButton_clicked()
{
    texts << ui->scriptComboBox->currentText();
    texts.removeDuplicates();
    ui->scriptComboBox->clear();
    ui->scriptComboBox->addItems(texts);

    //发送
}

void ScriptDialog::on_stopButton_clicked()
{

}

void ScriptDialog::on_pauseButton_clicked()
{

}

void ScriptDialog::on_continueButton_clicked()
{

}

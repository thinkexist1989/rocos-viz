#include "PlotConfigDialog.h"
#include "src/ui_PlotConfigDialog.h"
#include "ui_PlotConfigDialog.h"

#include <QFileDialog>


PlotConfigDialog::PlotConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotConfigDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
}

PlotConfigDialog::~PlotConfigDialog()
{
    delete ui;
}

void PlotConfigDialog::on_exitButton_clicked()
{
    this->close();
}


void PlotConfigDialog::on_buttonBox_accepted()
{
    qDebug() << "Accept!";
    this->accept();
}


void PlotConfigDialog::on_buttonBox_rejected()
{
    qDebug() << "Cancel!";
    this->reject();
}
void PlotConfigDialog::on_addBtn_clicked()
{


    auto chart = ui->plotTree->currentItem();
    if(chart == nullptr)
        return;
    qDebug() << "Selected chart:" << chart->text(0);

    auto items = ui->dataTree->selectedItems();
    for(auto& item : items) {
        auto it = item->clone();
        qDebug() << item->text(0);
        chart->addChild(it);
    }


    ui->plotTree->expandItem(chart);

    ui->dataTree->clearSelection();



}


void PlotConfigDialog::on_removeBtn_clicked()
{
    auto items = ui->plotTree->selectedItems();

    for(auto& item : items) {
        auto parent_item = item->parent();
        if(parent_item) {
            parent_item->removeChild(item);
        }
    }
}


void PlotConfigDialog::on_toolButton_clicked()
{

}


void PlotConfigDialog::on_dirButton_clicked()
{
    saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly);
    ui->dataSavePathEdit->setText(saveDir);
}


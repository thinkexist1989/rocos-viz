#include "PlotConfigDialog.h"
#include "ui_PlotConfigDialog.h"

PlotConfigDialog::PlotConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotConfigDialog)
{
    ui->setupUi(this);
}

PlotConfigDialog::~PlotConfigDialog()
{
    delete ui;
}

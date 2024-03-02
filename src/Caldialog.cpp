#include "Caldialog.h"
#include "ui_Caldialog.h"
#include <iostream>

CalDialog::CalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalDialog)
{
    ui->setupUi(this);
    std::cout<<"UI is running"<<std::endl;
   
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);
    std::cout<<"UI is running"<<std::endl;
   
}

CalDialog::~CalDialog()
{
    delete ui;
}

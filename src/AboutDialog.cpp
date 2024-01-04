#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->textBrowser->setFont(QFont("Alibaba PuHuiTi 3.0", 11, QFont::Bold));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_exitButton_clicked()
{
    this->close();
}


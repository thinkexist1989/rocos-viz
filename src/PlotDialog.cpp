#include "PlotDialog.h"
#include "ui_PlotDialog.h"
#include <QDebug>
#include <QtMath>
#include <QFileDialog>

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)

{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    time = new QElapsedTimer;

    //设置Record按钮动画
    movie = new QMovie(this);
    movie->setFileName("://res/skinny.gif");
    connect(movie, &QMovie::frameChanged, [=](){ui->recordButton->setIcon(movie->currentPixmap());});

    init_joint_space();


}

PlotDialog::~PlotDialog()
{
    delete ui;
}

void PlotDialog::init_joint_space()
{
    /****** 显示曲线按钮文字更改 *******/
    ui->dispCheck1->setText(tr("J1"));
    ui->dispCheck2->setText(tr("J2"));
    ui->dispCheck3->setText(tr("J3"));
    ui->dispCheck4->setText(tr("J4"));
    ui->dispCheck5->setText(tr("J5"));
    ui->dispCheck6->setText(tr("J6"));
    ui->dispCheck7->setText(tr("J7"));
    ui->dispCheck7->setVisible(true); //启用


}

void PlotDialog::init_cartesian_space()
{
    /****** 显示曲线按钮文字更改 *******/
    ui->dispCheck1->setText(tr("X"));
    ui->dispCheck2->setText(tr("Y"));
    ui->dispCheck3->setText(tr("Z"));
    ui->dispCheck4->setText(tr("Roll"));
    ui->dispCheck5->setText(tr("Pitch"));
    ui->dispCheck6->setText(tr("Yaw"));
    ui->dispCheck7->setText(tr(""));
    ui->dispCheck7->setVisible(false); //禁用

}

void PlotDialog::update_charts()
{

}

void PlotDialog::on_recordButton_clicked()
{
    if(isRecording) { //正在记录，要停止
        ui->recordButton->setText(tr("Start Recording"));
        movie->stop();

        if(isSaveData) { // 是否保存数据
            jntPosFile->close();
            cartPoseFile->close();
        }
    }
    else { //还未记录，要开始记录
        ui->recordButton->setText(tr("Stop Recording"));
        movie->start();

        timestamp = QDateTime::currentDateTime().toString("yyyyMMddTHHmmss");

        if(isSaveData) { // 是否保存数据
            jntPosFile = new QFile(saveDir + "/" + "jntPos" + timestamp + ".csv");
            jntPosFile->open(QIODevice::ReadWrite | QIODevice::Text);
            jntPosFile->write("t , j1 , j2 , j3 , j4 , j5 , j6 , j7\n");

            cartPoseFile = new QFile(saveDir + "/" + "cartPose" + timestamp + ".csv");
            cartPoseFile->open(QIODevice::ReadWrite | QIODevice::Text);
            cartPoseFile->write("t , X , Y , Z , r , p , y\n");
        }

    }

    isRecording = !isRecording;
}

void PlotDialog::on_typeComboBox_currentIndexChanged(int index)
{
    if(index == 0) {
        init_joint_space(); // 关节空间
    }
    else if(index == 1) {
        init_cartesian_space(); //笛卡尔空间
    }
}

void PlotDialog::on_fitButton_clicked()
{

}

void PlotDialog::on_posCheck_stateChanged(int)
{

}

void PlotDialog::on_velCheck_stateChanged(int)
{

}

void PlotDialog::on_accCheck_stateChanged(int)
{

}

void PlotDialog::on_jerkCheck_stateChanged(int)
{

}

void PlotDialog::on_scaleCheck_stateChanged(int arg1)
{
    if(arg1 > 0) { // checked值为2
        isAutoScale = true;
    }
    else{
        isAutoScale = false;
    }
}

void PlotDialog::getJointPositions(QVector<double> &jntPos)
{

}

void PlotDialog::getCartPose(QVector<double> &pose)
{

}

void PlotDialog::on_saveCheck_stateChanged(int arg1)
{

}

void PlotDialog::on_dirButton_clicked()
{
    saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly);
    ui->dataSavePathEdit->setText(saveDir);
}

void PlotDialog::on_exitButton_clicked()
{
    this->close();
}


#include "PlotWidget.h"
#include "ui_PlotWidget.h"
#include <QDebug>
#include <QtMath>
#include <QFileDialog>
#include <QLegendMarker>

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)

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


    QLineSeries *series = new QLineSeries;
    series->setName("Test");
//    series->append(0, 6);
//    series->append(2, 4);
//    series->append(3, 8);
//    series->append(7, 3);
//    series->append(10, 5);
    for(int i = 0; i < 100; i++) {
        series->append(i, QRandomGenerator::global()->generateDouble());
    }

    series->setUseOpenGL(true);

    QFont titleFont("Alibaba PuHuiTi 3.0", 11, QFont::Bold);
    QFont axisFont("Alibaba PuHuiTi 3.0", 11);

    QChart *chart = new QChart;
    chart->setTheme(QChart::ChartThemeLight);
    chart->addSeries(series);

//    chart->setTitleFont(titleFont);
//    chart->setTitle("My Chart Test");

    chart->createDefaultAxes();

//    QValueAxis *axisX = new QValueAxis;
//    axisX->setTitleText("X Axis");
//    axisX->setLabelsFont(axisFont);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis;
//    axisY->setTitleText("Y Axis");
//    axisY->setLabelsFont(axisFont);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);

    QLegend* legend = chart->legend();
    legend->setVisible(true);
    legend->setAlignment(Qt::AlignTop);

    QList<QLegendMarker*> markers = legend->markers();
    markers[0]->setLabel("Great!!");



    ui->plotWidget->setRenderHint(QPainter::Antialiasing);
//    ui->plotWidget->setDragMode(QChartView::ScrollHandDrag);
//    ui->plotWidget->setInteractive(true);
    ui->plotWidget->setRubberBand(QChartView::RectangleRubberBand);
    ui->plotWidget->setChart(chart);



}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::init_joint_space()
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

void PlotWidget::init_cartesian_space()
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

void PlotWidget::update_charts()
{

}

void PlotWidget::on_recordButton_clicked()
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

void PlotWidget::on_typeComboBox_currentIndexChanged(int index)
{
    if(index == 0) {
        init_joint_space(); // 关节空间
    }
    else if(index == 1) {
        init_cartesian_space(); //笛卡尔空间
    }
}

void PlotWidget::on_fitButton_clicked()
{

}

void PlotWidget::on_posCheck_stateChanged(int)
{

}

void PlotWidget::on_velCheck_stateChanged(int)
{

}

void PlotWidget::on_accCheck_stateChanged(int)
{

}

void PlotWidget::on_jerkCheck_stateChanged(int)
{

}

void PlotWidget::on_scaleCheck_stateChanged(int arg1)
{
    if(arg1 > 0) { // checked值为2
        isAutoScale = true;
    }
    else{
        isAutoScale = false;
    }
}

void PlotWidget::getJointPositions(QVector<double> &jntPos)
{

}

void PlotWidget::getCartPose(QVector<double> &pose)
{

}

void PlotWidget::on_saveCheck_stateChanged(int arg1)
{

}

void PlotWidget::on_dirButton_clicked()
{
    saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly);
    ui->dataSavePathEdit->setText(saveDir);
}


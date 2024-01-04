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


    addPlot();


    QList<QLineSeries* > data;

    for(int num = 0; num < 7; num++) {
        QLineSeries *series = new QLineSeries;
        series->setName("J"+ QString::number(num));
        series->setUseOpenGL(true);
        for(int i = 0; i < 100; i++) {
            series->append(i, QRandomGenerator::global()->generateDouble());
        }

        data.push_back(series);
        addSeries(0, series);
    }





//    QValueAxis *axisX = new QValueAxis;
//    axisX->setTitleText("Time");
//    axisX->setLabelsFont(axisFont);
//    chart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis;
//    axisY->setTitleText("Value");
//    axisY->setLabelsFont(axisFont);
//    chart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);

//    QChart *jntPosChart = new QChart;
//    jntPosChart->setTheme(QChart::ChartThemeLight);
//    for(auto& series : data)
//        jntPosChart->addSeries(series);

//    jntPosChart->setTitleFont(titleFont);
//    jntPosChart->setTitle("Joint Position");



//    QChart *endPosChart = new QChart;
//    endPosChart->setTheme(QChart::ChartThemeLight);
////    endPosChart->addSeries(series);

//    endPosChart->setTitleFont(titleFont);
//    endPosChart->setTitle("End Position");

//    endPosChart->createDefaultAxes();

//    ui->endPosPlot->setRenderHint(QPainter::Antialiasing);
//    ui->endPosPlot->setChart(endPosChart);



//    QChart *jntTorChart = new QChart;
//    jntTorChart->setTheme(QChart::ChartThemeLight);
////    jntTorChart->addSeries(series);

//    jntTorChart->setTitleFont(titleFont);
//    jntTorChart->setTitle("Joint Torque");

//    jntTorChart->createDefaultAxes();

//    ui->jntTorPlot->setRenderHint(QPainter::Antialiasing);
//    ui->jntTorPlot->setChart(jntTorChart);



//    QChart *endTorChart = new QChart;
//    endTorChart->setTheme(QChart::ChartThemeLight);
////    endTorChart->addSeries(series);

//    endTorChart->setTitleFont(titleFont);
//    endTorChart->setTitle("End Torque");

//    endTorChart->createDefaultAxes();

//    ui->endTorPlot->setRenderHint(QPainter::Antialiasing);
//    ui->endTorPlot->setChart(endTorChart);


}

PlotWidget::~PlotWidget()
{
    delete ui;
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

void PlotWidget::on_fitButton_clicked()
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

void PlotWidget::handleMarkerClicked()
{
    auto marker = qobject_cast<QLegendMarker*>(sender());
    Q_ASSERT(marker);

    switch (marker->type()) {
    case QXYLegendMarker::LegendMarkerTypeXY: {
        marker->series()->setVisible(!marker->series()->isVisible());
        marker->setVisible(true);
        qreal alpha = 1.0;
        if(!marker->series()->isVisible())
            alpha = 0.5;

        QColor color;
        QBrush brush = marker->labelBrush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setLabelBrush(brush);

        brush = marker->brush();
        color = brush.color();
        color.setAlphaF(alpha);
        brush.setColor(color);
        marker->setBrush(brush);

        QPen pen = marker->pen();
        color = pen.color();
        color.setAlphaF(alpha);
        pen.setColor(color);
        marker->setPen(pen);

        break;
    }
    default:
        break;
    }
}

void PlotWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    const auto& pos = event->pos();

    for(auto plot : plots) {
        if(plot->geometry().contains(pos)) {
            is2x2 = !is2x2;

            for(auto plot : plots) {
                plot->setVisible(is2x2);
            }

            plot->setVisible(true);

            return;

        }


    }

}

void PlotWidget::connectMarkers(QChart *chart)
{
    const auto& markers = chart->legend()->markers();
    for(QLegendMarker* marker : markers) {
        QObject::disconnect(marker, &QLegendMarker::clicked, this, &PlotWidget::handleMarkerClicked);
        QObject::connect(marker, &QLegendMarker::clicked, this, &PlotWidget::handleMarkerClicked);
    }

}

QChartView *PlotWidget::addPlot()
{
    if(plots.size() >= 4) {
        qCritical() << "You can add up to 4 view!";
        return nullptr;
    }

    QChart* chart = new QChart;
    chart->setTheme(QChart::ChartThemeBlueIcy);
    chart->setTitleFont(titleFont);
    chart->setTitle("Chart " + QString::number(plots.size() + 1));


    QChartView* plot = new QChartView;
    plots.append(plot);
    ui->gridPlot->addWidget(plot, (plots.size() - 1) / 2, (plots.size()-1) % 2);

    plot->setRenderHint(QPainter::Antialiasing);
    plot->setChart(chart);

    return plot;

}

void PlotWidget::addSeries(QChartView *plot, QXYSeries *series)
{
    plot->chart()->addSeries(series);
    connectMarkers(plot->chart());
    plot->chart()->createDefaultAxes();
}

void PlotWidget::addSeries(int index, QXYSeries *series)
{
    addSeries(plots[index], series);
}

void PlotWidget::on_addChart_clicked() {
    addPlot();
}

void PlotWidget::on_removeChart_clicked() {
    removePlot(plots.size() - 1);
}

void PlotWidget::removePlot(int index) {
    removePlot(plots[index]);
}

void PlotWidget::removePlot(QChartView *plot) {
    plots.removeOne(plot);
    plot->deleteLater();
}


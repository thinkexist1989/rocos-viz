#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QWidget>
#include <QMovie>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QDateTime>
#include <QButtonGroup>
#include <QFile>
#include <QtCharts>

#define TYPE_JOINT_SPACE 0 // Joint Space
#define TYPE_CARTESIAN_SPACE 1

namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();

    void init_joint_space();
    void init_cartesian_space();

    void update_charts(); //配置显示什么曲线


    QChartView* addPlot();
    void removePlot(int index); // remove one plot (chartview)
    void removePlot(QChartView* plot); // remove one plot (chartview)

    void addSeries(QChartView* plot, QXYSeries* series); // add one curve
    void addSeries(int index, QXYSeries* series); // add one curve


public slots:
    void on_recordButton_clicked();

    void on_fitButton_clicked();

    void on_scaleCheck_stateChanged(int arg1);

    void getJointPositions(QVector<double>& jntPos); // 处理关节位置
    void getCartPose(QVector<double>& pose); // 处理笛卡尔空间位置

private slots:
    void on_saveCheck_stateChanged(int arg1);

    void on_dirButton_clicked();

    void on_addChart_clicked();
    void on_removeChart_clicked();

    void handleMarkerClicked();

private:
    Ui::PlotWidget *ui;

    QMovie* movie;

    QTimer* timer;

    QButtonGroup* plotDispBtnGrp;

    bool isRecording = false;
    bool isSaveData = false;
    bool isAutoScale = true;

    int dispType = TYPE_JOINT_SPACE;

    QElapsedTimer* time;

    QString timestamp; //用于记录保存文件时间的时间戳
    QString saveDir = "./";  //保存记录文件的路径

    QFile* jntPosFile;
    QFile* cartPoseFile;

    bool is2x2 {true};

    QList<QChartView* > plots; // chartviews, max 4

    QFont titleFont {"Alibaba PuHuiTi 3.0", 11, QFont::Bold};
    QFont labelFont {"Alibaba PuHuiTi 3.0", 11};


//    void mousePressEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    void connectMarkers(QChart *chart);



};

#endif // PLOTDIALOG_H

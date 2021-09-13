#include "PlotDialog.h"
#include "ui_PlotDialog.h"
#include <QDebug>
#include <QtMath>

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog),
    colors(vtkNamedColors::New()),
    view(vtkContextView::New()),
    jntPosTable(vtkTable::New()),
    cartPosTable(vtkTable::New()),
//    velTable(vtkTable::New()),
//    accTable(vtkTable::New()),
//    jerkTable(vtkTable::New()),
    chartMat(vtkChartMatrix::New()),
    posChart(vtkChartXY::New()),    //position
//    posPlots(7, vtkPlotLine::New()),
    velChart(vtkChartXY::New()),    //velocity
//    velPlots(7,vtkPlotLine::New()),
    accChart(vtkChartXY::New()),    //acceleration
//    accPlots(7, vtkPlotLine::New()),
    jerkChart(vtkChartXY::New())   //jerk
//    jerkPlots(7,vtkPlotLine::New())
{
    ui->setupUi(this);

    //设置Record按钮动画
    movie = new QMovie(this);
    movie->setFileName("://res/skinny.gif");
    connect(movie, &QMovie::frameChanged, [=](){ui->recordButton->setIcon(movie->currentPixmap());});

    // Set up a 2D scene, add an XY chart to it
    //设置view的RenderWindow为plotWidget，即可展示contextView了，不需要再设置renderer
    view->SetRenderWindow(ui->plotWidget->GetRenderWindow());
    //设置view的背景
    view->GetRenderer()->GradientBackgroundOn(); //开启渐变
    view->GetRenderer()->SetBackground(colors->GetColor3d("Gainsboro").GetData());
    view->GetRenderer()->SetBackground2(colors->GetColor3d("DarkGray").GetData());

    /***Table格式：t x y z r p y  ***/
    //第1列为时间t
    vtkNew<vtkFloatArray> arrTc;    arrTc->SetName("t");    cartPosTable->AddColumn(arrTc);
    //第2列为笛卡尔x
    vtkNew<vtkFloatArray> arrX;    arrX->SetName("x");    cartPosTable->AddColumn(arrX);
    //第3列为笛卡尔y
    vtkNew<vtkFloatArray> arrY;    arrY->SetName("y");    cartPosTable->AddColumn(arrY);
    //第4列为笛卡尔z
    vtkNew<vtkFloatArray> arrZ;    arrZ->SetName("z");    cartPosTable->AddColumn(arrZ);
    //第5列为笛卡尔roll
    vtkNew<vtkFloatArray> arrRoll;    arrRoll->SetName("roll");    cartPosTable->AddColumn(arrRoll);
    //第6列为笛卡尔pitch
    vtkNew<vtkFloatArray> arrPitch;    arrPitch->SetName("pitch");    cartPosTable->AddColumn(arrPitch);
    //第7列为笛卡尔yaw
    vtkNew<vtkFloatArray> arrYaw;    arrYaw->SetName("yaw");    cartPosTable->AddColumn(arrYaw);

    /***Table格式：t j1 j2 j3 j4 j5 j6 j7  ***/
    //第1列为时间t
    vtkNew<vtkFloatArray> arrTj;    arrTj->SetName("t");    jntPosTable->AddColumn(arrTj);
    //第2列为关节1
    vtkNew<vtkFloatArray> arrJ1;    arrJ1->SetName("j1");    jntPosTable->AddColumn(arrJ1);
    //第3列为关节2
    vtkNew<vtkFloatArray> arrJ2;    arrJ2->SetName("j2");    jntPosTable->AddColumn(arrJ2);
    //第4列为关节3
    vtkNew<vtkFloatArray> arrJ3;    arrJ3->SetName("j3");    jntPosTable->AddColumn(arrJ3);
    //第5列为关节4
    vtkNew<vtkFloatArray> arrJ4;    arrJ4->SetName("j4");    jntPosTable->AddColumn(arrJ4);
    //第6列为关节5
    vtkNew<vtkFloatArray> arrJ5;    arrJ5->SetName("j5");    jntPosTable->AddColumn(arrJ5);
    //第7列为关节6
    vtkNew<vtkFloatArray> arrJ6;    arrJ6->SetName("j6");    jntPosTable->AddColumn(arrJ6);
    //第8列为关节7
    vtkNew<vtkFloatArray> arrJ7;    arrJ7->SetName("j7");    jntPosTable->AddColumn(arrJ7);


    //设置chart的样式
    posChart->SetTitle("Position");
    posChart->GetTitleProperties()->SetFontSize(20);
    posChart->GetTitleProperties()->SetBold(true);
    posChart->GetTitleProperties()->SetItalic(true);
    posChart->SetShowLegend(true);
    posChart->GetLegend()->GetBrush()->SetOpacity(0);
//    posChart->SetAutoAxes(true);
    velChart->SetTitle("Velocity");
    velChart->GetTitleProperties()->SetFontSize(20);
    velChart->GetTitleProperties()->SetBold(true);
    velChart->GetTitleProperties()->SetItalic(true);
    velChart->SetShowLegend(true);
    velChart->GetLegend()->GetBrush()->SetOpacity(0);

    accChart->SetTitle("Acceleration");
    accChart->GetTitleProperties()->SetFontSize(20);
    accChart->GetTitleProperties()->SetBold(true);
    accChart->GetTitleProperties()->SetItalic(true);
    accChart->SetShowLegend(true);
    accChart->GetLegend()->GetBrush()->SetOpacity(0);

    jerkChart->SetTitle("Jerk");
    jerkChart->GetTitleProperties()->SetFontSize(20);
    jerkChart->GetTitleProperties()->SetBold(true);
    jerkChart->GetTitleProperties()->SetItalic(true);
    jerkChart->SetShowLegend(true);
    jerkChart->GetLegend()->GetBrush()->SetOpacity(0);



    //设置pos chart中的plots
    posPlots.resize(7);
    for(int i = 0; i < posPlots.size(); i++) {
        posPlots[i] = posChart->AddPlot(vtkChart::LINE);
        posPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
    }

    //设置vel chart中的plots
    velPlots.resize(7);
    for(int i = 0; i < velPlots.size(); i++) {
        velPlots[i] = velChart->AddPlot(vtkChart::LINE);
        velPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
    }

    //设置vel chart中的plots
    accPlots.resize(7);
    for(int i = 0; i < accPlots.size(); i++) {
        accPlots[i] = accChart->AddPlot(vtkChart::LINE);
        accPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
    }

    //设置vel chart中的plots
    jerkPlots.resize(7);
    for(int i = 0; i < jerkPlots.size(); i++) {
        jerkPlots[i] = jerkChart->AddPlot(vtkChart::LINE);
        jerkPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
    }



    chartMat = vtkChartMatrix::New();
    view->GetScene()->AddItem(chartMat);

    chartMat->SetSize(vtkVector2i(1,1)); //默认显示4个chart
    chartMat->SetGutter(vtkVector2f(50.0, 50.0)); //图表之间的间距

    chartMat->SetChart(vtkVector2i(0, 0), posChart); //添加pos chart

    newData = vtkVariantArray::New();

//    newData->SetValue(0 , ii++);
    newData->SetNumberOfValues(8);
    newData->SetValue(0, 0);
    jntPosTable->InsertNextRow(newData);

    newData->SetValue(0 , 0);
    jntPosTable->InsertNextRow(newData);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        newData->SetValue(0 , ii);
        newData->SetValue(1 , (double)rand()/double(RAND_MAX));
        ii += 0.1;
        jntPosTable->InsertNextRow(newData);

//        jntPosTable->Modified();
//        posChart->RecalculateBounds(); //一调用便会刷新显示

        posChart->GetAxis(vtkAxis::BOTTOM)->SetRange((ii - 3)>0?(ii-3):0, ii);
//        posChart->GetAxis(vtkAxis::LEFT)->SetRange(-M_PI, M_PI);

        if(jntPosTable->GetNumberOfRows() > 30) {
//            jntPosTable->RemoveRow(0);
//            posChart->GetAxis(vtkAxis::BOTTOM)->SetRange(ii - 3, ii);
//            posChart->GetAxis(vtkAxis::LEFT)->SetRange(-2, 2);
//            posChart->Update();
//            posChart->GetAxis(vtkAxis::BOTTOM)->SetMaximum(ii);
        }
        jntPosTable->Modified();
//        posChart->Modified();
        ui->plotWidget->GetRenderWindow()->Render();

    });

    plotDispBtnGrp = new QButtonGroup(this);
    plotDispBtnGrp->setExclusive(false); //不互斥
    plotDispBtnGrp->addButton(ui->dispCheck1, 0);
    plotDispBtnGrp->addButton(ui->dispCheck2, 1);
    plotDispBtnGrp->addButton(ui->dispCheck3, 2);
    plotDispBtnGrp->addButton(ui->dispCheck4, 3);
    plotDispBtnGrp->addButton(ui->dispCheck5, 4);
    plotDispBtnGrp->addButton(ui->dispCheck6, 5);
    plotDispBtnGrp->addButton(ui->dispCheck7, 6);
    connect(plotDispBtnGrp, QOverload<int, bool>::of(&QButtonGroup::buttonToggled),
            [=](int id, bool checked){
                qDebug() << "Clicked: " << id << "; state: " << checked;

            });


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
    ui->dispCheck7->setDisabled(true); //禁用
}

void PlotDialog::update_charts()
{
    chartMat->ClearItems();

    int num = 0;
    if(ui->posCheck->isChecked()) {
        num++;
    }
    if(ui->velCheck->isChecked()) {
        num++;
    }
    if(ui->accCheck->isChecked()) {
        num++;
    }
    if(ui->jerkCheck->isChecked()) {
        num++;
    }

    if(num == 4) { //如果4个都显示，则以2*2显示
        chartMat->SetSize(vtkVector2i(2,2));
        chartMat->SetChart(vtkVector2i(0, 0), accChart); //添加acc chart
        chartMat->SetChart(vtkVector2i(0, 1), posChart); //添加pos chart
        chartMat->SetChart(vtkVector2i(1, 0), jerkChart); //添加jerk chart
        chartMat->SetChart(vtkVector2i(1, 1), velChart); //添加vel chart


        qDebug() << "num of charts: " << chartMat->GetNumberOfItems();

    }
    else { //否则以num * 1显示
        chartMat->SetSize(vtkVector2i(num,1));
        int i = 0;
        if(ui->posCheck->isChecked()) {
            chartMat->SetChart(vtkVector2i(i, 0), posChart);
            i++;
        }
        if(ui->velCheck->isChecked()) {
            chartMat->SetChart(vtkVector2i(i, 0), velChart);
            i++;
        }
        if(ui->accCheck->isChecked()) {
            chartMat->SetChart(vtkVector2i(i, 0), accChart);
            i++;
        }
        if(ui->jerkCheck->isChecked()) {
            chartMat->SetChart(vtkVector2i(i, 0), jerkChart);
            i++;
        }

    }

    ui->plotWidget->GetRenderWindow()->Render();

}

void PlotDialog::on_recordButton_clicked()
{
    if(isRecording) { //正在记录，要停止
        ui->recordButton->setText(tr("Start Recording"));
        movie->stop();
        timer->stop();
    }
    else { //还未记录，要开始记录
        ui->recordButton->setText(tr("Stop Recording"));
        movie->start();
        timer->start(100);

//        posChart->GetAxis(vtkAxis::LEFT)->SetRange(-M_PI, M_PI);
//        ui->plotWidget->GetRenderWindow()->Render();
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
//    posChart->GetAxis(vtkAxis::LEFT)->SetRange(-M_PI, M_PI);
    posChart->RecalculateBounds(); //一调用便会刷新显示
    ui->plotWidget->GetRenderWindow()->Render();
}

void PlotDialog::on_posCheck_stateChanged(int)
{
    update_charts();
}

void PlotDialog::on_velCheck_stateChanged(int)
{
    update_charts();
}

void PlotDialog::on_accCheck_stateChanged(int)
{
    update_charts();
}

void PlotDialog::on_jerkCheck_stateChanged(int)
{
    update_charts();
}

#include "PlotDialog.h"
#include "ui_PlotDialog.h"
#include <QDebug>
#include <QtMath>
#include <QFileDialog>

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

    time = new QTime;

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
    vtkNew<vtkFloatArray> arrTc;    arrTc->SetName("t");           cartPosTable->AddColumn(arrTc); //第1列为时间t
    vtkNew<vtkFloatArray> arrX;    arrX->SetName("x");             cartPosTable->AddColumn(arrX); //第2列为笛卡尔x
    vtkNew<vtkFloatArray> arrY;    arrY->SetName("y");             cartPosTable->AddColumn(arrY); //第3列为笛卡尔y
    vtkNew<vtkFloatArray> arrZ;    arrZ->SetName("z");             cartPosTable->AddColumn(arrZ); //第4列为笛卡尔z
    vtkNew<vtkFloatArray> arrRoll;    arrRoll->SetName("roll");    cartPosTable->AddColumn(arrRoll); //第5列为笛卡尔roll
    vtkNew<vtkFloatArray> arrPitch;    arrPitch->SetName("pitch"); cartPosTable->AddColumn(arrPitch); //第6列为笛卡尔pitch
    vtkNew<vtkFloatArray> arrYaw;    arrYaw->SetName("yaw");       cartPosTable->AddColumn(arrYaw); //第7列为笛卡尔yaw

    /***Table格式：t j1 j2 j3 j4 j5 j6 j7  ***/
    vtkNew<vtkFloatArray> arrTj;    arrTj->SetName("t");     jntPosTable->AddColumn(arrTj);     //第1列为时间t
    vtkNew<vtkFloatArray> arrJ1;    arrJ1->SetName("j1");    jntPosTable->AddColumn(arrJ1);    //第2列为关节1
    vtkNew<vtkFloatArray> arrJ2;    arrJ2->SetName("j2");    jntPosTable->AddColumn(arrJ2);    //第3列为关节2
    vtkNew<vtkFloatArray> arrJ3;    arrJ3->SetName("j3");    jntPosTable->AddColumn(arrJ3);    //第4列为关节3
    vtkNew<vtkFloatArray> arrJ4;    arrJ4->SetName("j4");    jntPosTable->AddColumn(arrJ4);    //第5列为关节4
    vtkNew<vtkFloatArray> arrJ5;    arrJ5->SetName("j5");    jntPosTable->AddColumn(arrJ5);    //第6列为关节5
    vtkNew<vtkFloatArray> arrJ6;    arrJ6->SetName("j6");    jntPosTable->AddColumn(arrJ6);    //第7列为关节6
    vtkNew<vtkFloatArray> arrJ7;    arrJ7->SetName("j7");    jntPosTable->AddColumn(arrJ7);    //第8列为关节7


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

    init_joint_space(); //初始化关节空间

    chartMat = vtkChartMatrix::New();
    view->GetScene()->AddItem(chartMat);

    chartMat->SetSize(vtkVector2i(1,1)); //默认显示4个chart
    chartMat->SetGutter(vtkVector2f(50.0, 50.0)); //图表之间的间距

    chartMat->SetChart(vtkVector2i(0, 0), posChart); //添加pos chart

    jntNewData = vtkVariantArray::New();

//    newData->SetValue(0 , ii++);
    jntNewData->SetNumberOfValues(8);
    jntPosTable->InsertNextRow(jntNewData);
    jntPosTable->InsertNextRow(jntNewData);

    cartNewData = vtkVariantArray::New();
    cartNewData->SetNumberOfValues(7);
    cartPosTable->InsertNextRow(cartNewData);
    cartPosTable->InsertNextRow(cartNewData);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        jntNewData->SetValue(0 , t);
        jntNewData->SetValue(1 , (double)rand()/double(RAND_MAX));
        t += 0.1;
        jntPosTable->InsertNextRow(jntNewData);

//        jntPosTable->Modified();
//        posChart->RecalculateBounds(); //一调用便会刷新显示

        posChart->GetAxis(vtkAxis::BOTTOM)->SetRange((t - 3)>0?(t-3):0, t);
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
                if(checked) {
                    posChart->AddPlot(posPlots[id]);
                }
                else {
                    posChart->RemovePlotInstance(posPlots[id]);
                }
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
    ui->dispCheck7->setVisible(true); //启用


    /***** 更改chart链接的数据 *******/
    //设置pos chart中的plots
    posChart->ClearPlots();
    posPlots.resize(7);
    for(int i = 0; i < posPlots.size(); i++) {
        posPlots[i] = posChart->AddPlot(vtkChart::LINE);
        posPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
    }
    posChart->RecalculateBounds();


//    //设置vel chart中的plots
//    velChart->ClearPlots();
//    velPlots.resize(7);
//    for(int i = 0; i < velPlots.size(); i++) {
//        velPlots[i] = velChart->AddPlot(vtkChart::LINE);
//        velPlots[i]->SetInputData(jntVelTable, 0, i+1); //初始化显示关节位置
//    }
//    velChart->RecalculateBounds();


//    //设置vel chart中的plots
//    accChart->ClearPlots();
//    accPlots.resize(7);
//    for(int i = 0; i < accPlots.size(); i++) {
//        accPlots[i] = accChart->AddPlot(vtkChart::LINE);
//        accPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
//    }
//    accChart->RecalculateBounds();


//    //设置vel chart中的plots
//    jerkChart->ClearPlots();
//    jerkPlots.resize(7);
//    for(int i = 0; i < jerkPlots.size(); i++) {
//        jerkPlots[i] = jerkChart->AddPlot(vtkChart::LINE);
//        jerkPlots[i]->SetInputData(jntPosTable, 0, i+1); //初始化显示关节位置
//    }
//    jerkChart->RecalculateBounds();


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

    //设置pos chart中的plots
    posChart->ClearPlots();
    posPlots.resize(6);
    for(int i = 0; i < posPlots.size(); i++) {
        posPlots[i] = posChart->AddPlot(vtkChart::LINE);
        posPlots[i]->SetInputData(cartPosTable, 0, i+1); //初始化显示关节位置
    }
    posChart->RecalculateBounds();


    //设置vel chart中的plots
    velChart->ClearPlots();
    velPlots.resize(6);
    for(int i = 0; i < velPlots.size(); i++) {
        velPlots[i] = velChart->AddPlot(vtkChart::LINE);
        velPlots[i]->SetInputData(cartPosTable, 0, i+1); //初始化显示关节位置
    }
    velChart->RecalculateBounds();


    //设置vel chart中的plots
    accChart->ClearPlots();
    accPlots.resize(6);
    for(int i = 0; i < accPlots.size(); i++) {
        accPlots[i] = accChart->AddPlot(vtkChart::LINE);
        accPlots[i]->SetInputData(cartPosTable, 0, i+1); //初始化显示关节位置
    }
    accChart->RecalculateBounds();


    //设置vel chart中的plots
    jerkChart->ClearPlots();
    jerkPlots.resize(6);
    for(int i = 0; i < jerkPlots.size(); i++) {
        jerkPlots[i] = jerkChart->AddPlot(vtkChart::LINE);
        jerkPlots[i]->SetInputData(cartPosTable, 0, i+1); //初始化显示关节位置
    }
    jerkChart->RecalculateBounds();


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

        if(isSaveData) { // 是否保存数据
            jntPosFile->close();
            cartPoseFile->close();
        }
//        timer->stop(); //TODO: for test
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

        time->restart(); //重新启动计时

        jntPosTable->SetNumberOfRows(2);
        cartPosTable->SetNumberOfRows(2);

        //        timer->start(100); //TODO: for test

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
    velChart->RecalculateBounds();
    accChart->RecalculateBounds();
    jerkChart->RecalculateBounds();

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

void PlotDialog::on_scaleCheck_stateChanged(int arg1)
{
//    qDebug() << "Auto Scale: " << arg1;
    if(arg1 > 0) { // checked值为2
        isAutoScale = true;
    }
    else{
        isAutoScale = false;
    }
}

void PlotDialog::getJointPositions(QVector<double> &jntPos)
{
    if(!isRecording)
        return;

    double t = time->elapsed()/1000.0;

    jntNewData->SetValue(0 , t);
    for(int i = 0; i < jntPos.size(); i++) {
        jntNewData->SetValue(i+1 , jntPos[i]);
    }

    jntPosTable->InsertNextRow(jntNewData);

    if(isSaveData) { //记录数据到 /your_path/jntPos20210916T143500.csv
        QByteArray ba;
        ba.append(QString("%1 , ").arg(t));
        for(int i = 0; i < jntPos.size(); i++) {
            ba.append(QString("%1 , ").arg(jntPos[i]));
        }
        ba.append("\n");

        jntPosFile->write(ba);
    }

    if(dispType == TYPE_JOINT_SPACE)
        if(isAutoScale)
            posChart->GetAxis(vtkAxis::BOTTOM)->SetRange((t - 3)>0?(t-3):0, t);

    jntPosTable->Modified();
    ui->plotWidget->GetRenderWindow()->Render();
}

void PlotDialog::getCartPose(QVector<double> &pose)
{
    if(!isRecording)
        return;

    double t = time->elapsed()/1000.0;

    cartNewData->SetValue(0 , t);
    for(int i = 0; i < pose.size(); i++) {
        cartNewData->SetValue(i+1 , pose[i]);
    }

    cartPosTable->InsertNextRow(cartNewData);

    if(isSaveData) { //记录数据到 /your_path/cartPose20210916T143500.csv
        QByteArray ba;
        ba.append(QString("%1 , ").arg(t));
        for(int i = 0; i < pose.size(); i++) {
            ba.append(QString("%1 , ").arg(pose[i]));
        }
        ba.append("\n");

        cartPoseFile->write(ba);
    }

    if(dispType == TYPE_CARTESIAN_SPACE)
        if(isAutoScale)
            posChart->GetAxis(vtkAxis::BOTTOM)->SetRange((t - 3)>0?(t-3):0, t);

    cartPosTable->Modified();
    ui->plotWidget->GetRenderWindow()->Render();

}

void PlotDialog::on_saveCheck_stateChanged(int arg1)
{
//    qDebug() << "Save Data: " << arg1;
    if(arg1 > 0) { // checked值为2
        isSaveData = true;
        ui->dataSavePathEdit->setEnabled(true);
        ui->dirButton->setEnabled(true);
    }
    else{
        isSaveData = false;
        ui->dataSavePathEdit->setEnabled(false);
        ui->dirButton->setEnabled(false);
    }

}

void PlotDialog::on_dirButton_clicked()
{
    saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly);
    ui->dataSavePathEdit->setText(saveDir);
}

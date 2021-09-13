#include "PlotDialog.h"
#include "ui_PlotDialog.h"

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog),
    colors(vtkNamedColors::New()),
    view(vtkContextView::New()),
    posTable(vtkTable::New()),
    velTable(vtkTable::New()),
    accTable(vtkTable::New()),
    jerkTable(vtkTable::New()),
    chartMat(vtkChartMatrix::New()),
    posChart(vtkChartXY::New()),     //position
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

    /***Table格式：t x y z r p y j1 j2 j3 j4 j5 j6 j7 ***/
    //第1列为时间t
    vtkNew<vtkFloatArray> arrt;    arrt->SetName("t");    posTable->AddColumn(arrt);
    //第2列为笛卡尔x
    vtkNew<vtkFloatArray> arrX;    arrX->SetName("x");    posTable->AddColumn(arrX);
    //第3列为笛卡尔y
    vtkNew<vtkFloatArray> arrY;    arrY->SetName("y");    posTable->AddColumn(arrY);
    //第4列为笛卡尔z
    vtkNew<vtkFloatArray> arrZ;    arrZ->SetName("z");    posTable->AddColumn(arrZ);
    //第5列为笛卡尔roll
    vtkNew<vtkFloatArray> arrRoll;    arrRoll->SetName("roll");    posTable->AddColumn(arrRoll);
    //第6列为笛卡尔pitch
    vtkNew<vtkFloatArray> arrPitch;    arrPitch->SetName("pitch");    posTable->AddColumn(arrPitch);
    //第7列为笛卡尔yaw
    vtkNew<vtkFloatArray> arrYaw;    arrYaw->SetName("yaw");    posTable->AddColumn(arrYaw);
    //第8列为关节1
    vtkNew<vtkFloatArray> arrJ1;    arrJ1->SetName("j1");    posTable->AddColumn(arrJ1);
    //第9列为关节2
    vtkNew<vtkFloatArray> arrJ2;    arrJ2->SetName("j2");    posTable->AddColumn(arrJ2);
    //第10列为关节3
    vtkNew<vtkFloatArray> arrJ3;    arrJ3->SetName("j3");    posTable->AddColumn(arrJ3);
    //第11列为关节4
    vtkNew<vtkFloatArray> arrJ4;    arrJ4->SetName("j4");    posTable->AddColumn(arrJ4);
    //第12列为关节5
    vtkNew<vtkFloatArray> arrJ5;    arrJ5->SetName("j5");    posTable->AddColumn(arrJ5);
    //第13列为关节6
    vtkNew<vtkFloatArray> arrJ6;    arrJ6->SetName("j6");    posTable->AddColumn(arrJ6);
    //第14列为关节7
    vtkNew<vtkFloatArray> arrJ7;    arrJ7->SetName("j7");    posTable->AddColumn(arrJ7);

    posPlots.resize(7);
    for(int i = 0; i < posPlots.size(); i++) {
        posPlots[i] = posChart->AddPlot(vtkChart::LINE);
        posPlots[i]->SetInputData(posTable, 0, i+7); //初始化显示关节位置
    }
    posChart->SetTitle("Position");
    posChart->GetTitleProperties()->SetFontSize(20);
    posChart->GetTitleProperties()->SetBold(true);
    posChart->GetTitleProperties()->SetItalic(true);
    posChart->SetShowLegend(true);
    posChart->GetLegend()->GetBrush()->SetOpacity(0);
    posChart->SetAutoAxes(true);



    chartMat = vtkChartMatrix::New();
    view->GetScene()->AddItem(chartMat);

    chartMat->SetSize(vtkVector2i(1,1)); //默认显示4个chart
    chartMat->SetGutter(vtkVector2f(50.0, 50.0)); //图表之间的间距

    chartMat->SetChart(vtkVector2i(0, 0), posChart); //添加pos chart

//    chartMat->SetChart(vtkVector2i(1, 1), velChart); //添加vel chart
//    chartMat->SetChart(vtkVector2i(0, 0), accChart); //添加acc chart
//    chartMat->SetChart(vtkVector2i(1, 0), jerkChart); //添加jerk chart


//    int numPoints = 42;
//    float inc = 7.5 / (numPoints - 1);
//    table->SetNumberOfRows(numPoints);
////    table->InsertNextRow()
//    for (int i = 0; i < numPoints; ++i)
//    {
//      table->SetValue(i, 0, i * inc);
//      table->SetValue(i, 1, cos(i * inc));
//      table->SetValue(i, 2, sin(i * inc));
//      table->SetValue(i, 3, sin(i * inc) + 0.5);
//      table->SetValue(i, 4, tan(i * inc));
//    }

    newData = vtkVariantArray::New();
//    newData->SetNumberOfTuples(14);
//    newData->Allocate(14);
//    newData->SetValue(0 , ii++);
    newData->SetNumberOfValues(14);
    newData->SetValue(0, ii++);
    posTable->InsertNextRow(newData);

    newData->SetValue(0 , ii++);
    posTable->InsertNextRow(newData);


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        newData->SetValue(0 , ii++);
        newData->SetValue(7 , (double)rand()/double(RAND_MAX));
        posTable->InsertNextRow(newData);
        posTable->Modified();
        posChart->RecalculateBounds();
    });


    // Add multiple line plots, setting the colors etc
      // lower left plot, a point chart
//      vtkChart* chart = matrix->GetChart(vtkVector2i(0, 0));
//      vtkPlot* plot = chart->AddPlot(vtkChart::POINTS);
//      plot->SetInputData(table, 0, 1);
//      dynamic_cast<vtkPlotPoints*>(plot)->SetMarkerStyle(vtkPlotPoints::DIAMOND);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("sea_green").GetRed(),
//                     colors->GetColor3ub("sea_green").GetGreen(),
//                     colors->GetColor3ub("sea_green").GetBlue(), 255);

//      // upper left plot, a point chart
//      chart = matrix->GetChart(vtkVector2i(0, 1));
//      plot = chart->AddPlot(vtkChart::POINTS);
//      plot->SetInputData(table, 0, 2);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("rose_madder").GetRed(),
//                     colors->GetColor3ub("rose_madder").GetGreen(),
//                     colors->GetColor3ub("rose_madder").GetBlue(), 255);


////      legend->GetLabelProperties()->SetBackgroundColor(colors->GetColor3d("dark_orange").GetData());
////      legend->GetLabelProperties()->SetBackgroundOpacity(1);
////      legend->SetOpacity(1);
////      legend->GetBrush()->SetOpacity(0.5);
//      plot = chart->AddPlot(vtkChart::LINE);
//      plot->SetInputData(table, 0, 3);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("dark_orange").GetRed(),
//                     colors->GetColor3ub("dark_orange").GetGreen(),
//                     colors->GetColor3ub("dark_orange").GetBlue(), 255);

//      // upper right plot, a bar and point chart
//      chart = matrix->GetChart(vtkVector2i(1, 1));
//      plot = chart->AddPlot(vtkChart::BAR);
//      plot->SetInputData(table, 0, 4);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("burnt_sienna").GetRed(),
//                     colors->GetColor3ub("burnt_sienna").GetGreen(),
//                     colors->GetColor3ub("burnt_sienna").GetBlue(), 255);

//      plot = chart->AddPlot(vtkChart::POINTS);
//      plot->SetInputData(table, 0, 1);
//      dynamic_cast<vtkPlotPoints*>(plot)->SetMarkerStyle(vtkPlotPoints::CROSS);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("rose_madder").GetRed(),
//                     colors->GetColor3ub("rose_madder").GetGreen(),
//                     colors->GetColor3ub("rose_madder").GetBlue(), 255);

//      // Lower right plot, a line chart

//      chart = matrix->GetChart(vtkVector2i(1, 0));
//      plot = chart->AddPlot(vtkChart::LINE);
//      plot->SetInputData(table, 0, 3);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("dark_orange").GetRed(),
//                     colors->GetColor3ub("dark_orange").GetGreen(),
//                     colors->GetColor3ub("dark_orange").GetBlue(), 255);

//      plot = chart->AddPlot(vtkChart::LINE);
//      plot->SetInputData(table, 0, 3);
//      plot->GetXAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->GetYAxis()->GetGridPen()->SetColorF(
//          colors->GetColor3d("warm_grey").GetData());
//      plot->SetColor(colors->GetColor3ub("royal_blue").GetRed(),
//                     colors->GetColor3ub("royal_blue").GetGreen(),
//                     colors->GetColor3ub("royal_blue").GetBlue(), 255);


}

PlotDialog::~PlotDialog()
{
    delete ui;
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
    }

    isRecording = !isRecording;
}

void PlotDialog::on_typeComboBox_currentIndexChanged(int index)
{

}

#include <RocosMainWindow.h>
#include "./ui_RocosMainWindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

RocosMainWindow::RocosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RocosMainWindow)
{
    ui->setupUi(this);

    //给Log增加一个右键菜单clear，清除之前记录
    ui->logEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    //记录程序开始运行的时间
    time = new QTime;
    time->start();

    QTime tt(0,0);
    timeLabel = new QLabel("Current Time: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "    Program is running: " + tt.addMSecs(time->elapsed()).toString() + " ", this);
    QFont font;
    font.setFamily("Arial");
    font.setBold(true);
    font.setPointSize(11);
    timeLabel->setFont(font);
    ui->statusbar->setFont(font);
    ui->statusbar->addPermanentWidget(timeLabel);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        QTime tt(0,0);
        timeLabel->setText("Current Time: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "    Program is running: " + tt.addMSecs(time->elapsed()).toString());
    });
    timer->start(500);

    connectDlg = new ConnectDialog(this);
    scriptDlg  = new ScriptDialog(this);
    plotDlg    = new PlotDialog(this);
    aboutDlg   = new AboutDialog(this);

    scriptDlg->setConnectPtr(connectDlg); //将connectDlg指针传给script

    /********Joint Position Widgets********/
    ui->Joint1PosWidget->setName("#1 ");
    ui->Joint1PosWidget->setId(0);
    connect(ui->Joint1PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint1PosWidget);

    ui->Joint2PosWidget->setName("#2 ");
    ui->Joint2PosWidget->setId(1);
    connect(ui->Joint2PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint2PosWidget);

    ui->Joint3PosWidget->setName("#3 ");
    ui->Joint3PosWidget->setId(2);
    connect(ui->Joint3PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint3PosWidget);

    ui->Joint4PosWidget->setName("#4 ");
    ui->Joint4PosWidget->setId(3);
    connect(ui->Joint4PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint4PosWidget);

    ui->Joint5PosWidget->setName("#5 ");
    ui->Joint5PosWidget->setId(4);
    connect(ui->Joint5PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint5PosWidget);

    ui->Joint6PosWidget->setName("#6 ");
    ui->Joint6PosWidget->setId(5);
    connect(ui->Joint6PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint6PosWidget);

    ui->Joint7PosWidget->setName("#7 ");
    ui->Joint7PosWidget->setId(6);
    connect(ui->Joint7PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint7PosWidget);

    /********Cartesian Position Widgets********/
    ui->cartesianXWidget->setName("X");
    ui->cartesianXWidget->setId(0);
    connect(ui->cartesianXWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianXWidget);

    ui->cartesianYWidget->setName("Y");
    ui->cartesianYWidget->setId(1);
    connect(ui->cartesianYWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianYWidget);

    ui->cartesianZWidget->setName("Z");
    ui->cartesianZWidget->setId(2);
    connect(ui->cartesianZWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianZWidget);

    ui->cartesianRollWidget->setName("r");
    ui->cartesianRollWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianRollWidget->setId(3);
    connect(ui->cartesianRollWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianRollWidget);

    ui->cartesianPitchWidget->setName("p");
    ui->cartesianPitchWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianPitchWidget->setId(4);
    connect(ui->cartesianPitchWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianPitchWidget);

    ui->cartesianYawWidget->setName("y");
    ui->cartesianYawWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianYawWidget->setId(5);
    connect(ui->cartesianYawWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir){
                connectDlg->cartesianJogging(currentFrame, id, dir);
            });
    cpWdgs.push_back(ui->cartesianYawWidget);

    /********Speed Scaling Widgets********/
    ui->speedPercent->setText(tr("25%"));

    connect(connectDlg, &ConnectDialog::jointPositions, this, &RocosMainWindow::updateJointPos); //关节位置更新
    connect(connectDlg, &ConnectDialog::cartPose, this, &RocosMainWindow::updateCartPose); //笛卡尔位置更新
    connect(connectDlg, &ConnectDialog::speedScaling, this, [=](double f100){ ui->speedSlider->setValue(f100*10);}); //更新速度缩放因子

    connect(connectDlg, &ConnectDialog::jointPositions, plotDlg, &PlotDialog::getJointPositions); //关节位置更新
    connect(connectDlg, &ConnectDialog::cartPose, plotDlg, &PlotDialog::getCartPose); //笛卡尔位置更新

    connect(connectDlg, &ConnectDialog::logging, this, [=](QByteArray& ba){
//        ui->logBrowser->insertPlainText(QTime::currentTime().toString("[HH:mm:ss.zzz] ") + QString(ba) + "\n");
//        ui->logBrowser->
        ui->logEdit->moveCursor(QTextCursor::End);
        QString str = QTime::currentTime().toString("[HH:mm:ss.zzz] ") + QString(ba) + "\n";
        ui->logEdit->appendPlainText(str.simplified());

    });

}

RocosMainWindow::~RocosMainWindow()
{
    delete ui;
}


void RocosMainWindow::on_pushButton_clicked()
{
    QVector<double> jntRads;
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
//    this->ui->visualWidget->setJointPos(jntRads);

    updateJointPos(jntRads);
}

void RocosMainWindow::on_axesCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) {
        this->ui->visualWidget->setJointAxesVisibility(true);
    }
    else {
        this->ui->visualWidget->setJointAxesVisibility(false);
    }
}

void RocosMainWindow::on_speedSlider_valueChanged(int value)
{
    double f = value / 1000.0;
    ui->speedPercent->setText(tr("%1%").arg((int)(f * 100)));

    ui->Joint1PosWidget->setFactor(f);
    ui->Joint2PosWidget->setFactor(f);
    ui->Joint3PosWidget->setFactor(f);
    ui->Joint4PosWidget->setFactor(f);
    ui->Joint5PosWidget->setFactor(f);
    ui->Joint6PosWidget->setFactor(f);
    ui->Joint7PosWidget->setFactor(f);

    ui->cartesianXWidget->setFactor(f);
    ui->cartesianYWidget->setFactor(f);
    ui->cartesianZWidget->setFactor(f);
    ui->cartesianRollWidget->setFactor(f);
    ui->cartesianPitchWidget->setFactor(f);
    ui->cartesianYawWidget->setFactor(f);

    connectDlg->setJointSpeedScaling(f*100);
}
void RocosMainWindow::on_actionEnabled_triggered()
{
    isRobotEnabled = connectDlg->getRobotEnabled();

    connectDlg->setRobotEnabled(!isRobotEnabled);

    isRobotEnabled = connectDlg->getRobotEnabled();

    if(isRobotEnabled) {
        ui->actionEnabled->setIcon(QIcon(":/res/switchon.png"));
    }
    else{
        ui->actionEnabled->setIcon(QIcon(":/res/switchoff.png"));
    }
}

void RocosMainWindow::on_actionConnected_triggered()
{
    if(!isRobotConnected) {
        int res = connectDlg->exec();
        if(res == QDialog::Accepted){
    //        qDebug() << "Connected!";
            isRobotConnected = true;
            ui->actionConnected->setIcon(QIcon(":/res/connected.png"));
        }
        else{
            isRobotConnected = false;
            ui->actionConnected->setIcon(QIcon(":/res/disconnected.png"));
        }
    }
    else {
        connectDlg->connectedToRobot(false);
        isRobotConnected = false;
        ui->actionConnected->setIcon(QIcon(":/res/disconnected.png"));
    }


}

void RocosMainWindow::updateJointPos(QVector<double> &jntPos)
{
    for(int i = 0; i < jntPos.size(); i++) {
        jpWdgs[i]->updateJointPosition(jntPos[i]); //更新关节位置
    }

    jntPos.resize(7); //TODO: 这句话实际需要屏蔽
    ui->visualWidget->setJointPos(jntPos);
}

void RocosMainWindow::updateCartPose(QVector<double> &pose)
{
    if(pose.size() != 6) {
        return;
    }

    for(int i = 0; i < cpWdgs.size(); i++) {
        cpWdgs[i]->updateVal(pose[i]);
    }
}

void RocosMainWindow::on_meshCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked) {
        this->ui->visualWidget->setMeshVisibility(true);
    }
    else {
        this->ui->visualWidget->setMeshVisibility(false);
    }
}

void RocosMainWindow::on_actionScript_triggered()
{
//    isRobotEnabled = false;
//    ui->actionEnabled->setIcon(QIcon(":/res/switchoff.png"));

    scriptDlg->show();
}

void RocosMainWindow::on_actionPlotter_triggered()
{
    plotDlg->show();
}

void RocosMainWindow::on_actionAbout_triggered()
{
    aboutDlg->show();
}

void RocosMainWindow::on_actionAxoView_triggered()
{
    ui->visualWidget->setAxoView();
}

void RocosMainWindow::on_actionTopView_triggered()
{
    ui->visualWidget->setZAxisView();
}

void RocosMainWindow::on_actionFrontView_triggered()
{
    ui->visualWidget->setXAxisView();
}

void RocosMainWindow::on_actionRightView_triggered()
{
    ui->visualWidget->setYAxisView();
}

void RocosMainWindow::on_actionSetZero_triggered()
{
    auto ret = QMessageBox::warning(this, tr("Zero Calibration"), tr("The current joint position will set as ZERO. Are you sure?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

    if(ret == QMessageBox::Ok) {
        connectDlg->setZeroCalibration();
    }

}

void RocosMainWindow::on_baseFrame_clicked(bool checked)
{
    if(checked) {
        currentFrame = FRAME_BASE;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (BASE)"));
    }
}

void RocosMainWindow::on_flangeFrame_clicked(bool checked)
{
    if(checked) {
        currentFrame = FRAME_FLANGE;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (FLANGE)"));
    }
}

void RocosMainWindow::on_actionAngleRep_triggered()
{
    isAngleDegree = !isAngleDegree;

    int angleRep = isAngleDegree ? ANGLE_DEGREE : ANGLE_RADIUS;

    for(auto& jpWdg : jpWdgs) {
        jpWdg->setAngleRep(angleRep);
    }
    ui->cartesianRollWidget->setAngleRep(angleRep);
    ui->cartesianPitchWidget->setAngleRep(angleRep);
    ui->cartesianYawWidget->setAngleRep(angleRep);

    if(isAngleDegree) {
        ui->actionAngleRep->setIcon(QIcon(":/res/degree.png"));
    }
    else{
        ui->actionAngleRep->setIcon(QIcon(":/res/radius.png"));
    }
}

void RocosMainWindow::on_actionPosRep_triggered()
{
    isPosMM = !isPosMM;

    int posRep = isPosMM ? POS_MM : POS_M;

    ui->cartesianXWidget->setPosRep(posRep);
    ui->cartesianYWidget->setPosRep(posRep);
    ui->cartesianZWidget->setPosRep(posRep);

    if(isPosMM) {
        ui->actionPosRep->setIcon(QIcon(":/res/mm.png"));
    }
    else{
        ui->actionPosRep->setIcon(QIcon(":/res/m.png"));
    }


}

void RocosMainWindow::on_logEdit_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos); //这个pos显示的位置不对

    QMenu* menu = new QMenu(this);
    QAction *action = new QAction(tr("clear"), this);
    connect(action, &QAction::triggered, ui->logEdit, &QPlainTextEdit::clear);

    menu->addAction(action);
    menu->move(cursor().pos());
    menu->show();

}

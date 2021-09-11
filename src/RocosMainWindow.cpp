#include <RocosMainWindow.h>
#include "./ui_RocosMainWindow.h"

#include <QDebug>

RocosMainWindow::RocosMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RocosMainWindow)
{
    ui->setupUi(this);

    connectDlg = new ConnectDialog(this);
    scriptDlg  = new ScriptDialog(this);
    plotDlg    = new PlotDialog(this);
    aboutDlg   = new AboutDialog(this);

    scriptDlg->setConnectPtr(connectDlg); //将connectDlg指针传给script

    /********Joint Position Widgets********/
    ui->Joint1PosWidget->setName("#1 JOINT");
    ui->Joint1PosWidget->setId(1);
    connect(ui->Joint1PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint1PosWidget);

    ui->Joint2PosWidget->setName("#2 JOINT");
    ui->Joint2PosWidget->setId(2);
    connect(ui->Joint2PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint2PosWidget);

    ui->Joint3PosWidget->setName("#3 JOINT");
    ui->Joint3PosWidget->setId(3);
    connect(ui->Joint3PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint3PosWidget);

    ui->Joint4PosWidget->setName("#4 JOINT");
    ui->Joint4PosWidget->setId(4);
    connect(ui->Joint4PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint4PosWidget);

    ui->Joint5PosWidget->setName("#5 JOINT");
    ui->Joint5PosWidget->setId(5);
    connect(ui->Joint5PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint5PosWidget);

    ui->Joint6PosWidget->setName("#6 JOINT");
    ui->Joint6PosWidget->setId(6);
    connect(ui->Joint6PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint6PosWidget);

    ui->Joint7PosWidget->setName("#7 JOINT");
    ui->Joint7PosWidget->setId(7);
    connect(ui->Joint7PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir){
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint7PosWidget);

    /********Cartesian Position Widgets********/
    ui->cartesianXWidget->setName("X");
    cpWdgs.push_back(ui->cartesianXWidget);

    ui->cartesianYWidget->setName("Y");
    cpWdgs.push_back(ui->cartesianYWidget);

    ui->cartesianZWidget->setName("Z");
    cpWdgs.push_back(ui->cartesianZWidget);

    ui->cartesianRollWidget->setName("R");
    cpWdgs.push_back(ui->cartesianRollWidget);

    ui->cartesianPitchWidget->setName("P");
    cpWdgs.push_back(ui->cartesianPitchWidget);

    ui->cartesianYawWidget->setName("Y");
    cpWdgs.push_back(ui->cartesianYawWidget);

    /********Speed Scaling Widgets********/
    ui->speedPercent->setText(tr("25%"));

    connect(connectDlg, &ConnectDialog::jointPositions, this, &RocosMainWindow::updateJointPos); //关节位置更新
    connect(connectDlg, &ConnectDialog::cartPose, this, &RocosMainWindow::updateCartPose); //笛卡尔位置更新
    connect(connectDlg, &ConnectDialog::speedScaling, this, [=](double f100){ ui->speedSlider->setValue(f100*10);}); //更新速度缩放因子

}

RocosMainWindow::~RocosMainWindow()
{
    delete ui;
}


void RocosMainWindow::on_pushButton_clicked()
{
    std::vector<double> jntRads;
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    jntRads.push_back(rand()/double(RAND_MAX));
    this->ui->visualWidget->setJointPos(jntRads);
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
        ui->cartesianXWidget->updateVal(pose[i]);
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

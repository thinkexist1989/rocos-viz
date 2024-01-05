#include <RocosMainWindow.h>
#include "./ui_RocosMainWindow.h"

#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QStyledItemDelegate>

RocosMainWindow::RocosMainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::RocosMainWindow) {
    ui->setupUi(this);

    //给Log增加一个右键菜单clear，清除之前记录
    //ui->logEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
    ui->workingModeBox->setItemDelegate(itemDelegate);

    /********记录程序开始运行的时间********/
    time = new QElapsedTimer;
    time->start();

    QTime tt(0, 0);
    ui->timerLabel->setText("CURRENT TIME: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    ui->runningLabel->setText("RUNNING TIME: " + tt.addMSecs(time->elapsed()).toString());
//    QFont font;
//    font.setFamily("Arial");
//    font.setBold(true);
//    font.setPointSize(11);
//    timeLabel->setFont(font);
//    ui->statusbar->setFont(font);
//    ui->statusbar->addPermanentWidget(timeLabel);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        QTime tt(0, 0);
        ui->timerLabel->setText("CURRENT TIME: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        ui->runningLabel->setText("RUNNING TIME: " + tt.addMSecs(time->elapsed()).toString());
    });
    timer->start(500); // 每500ms更新一次时间

    //////////////////////////////////////////////////////////

    connectDlg = new ConnectDialog(this);
    scriptDlg = new ScriptDialog(this);
    aboutDlg = new AboutDialog(this);
    modelLoaderDlg = new ModelLoaderDialog(this);
    plotConfigDlg = new PlotConfigDialog(this);

    connect(modelLoaderDlg, &ModelLoaderDialog::getRobotModel, connectDlg, &ConnectDialog::getRobotModel);
    connect(modelLoaderDlg, &ModelLoaderDialog::removeRobotModel, this, [=](){ui->visualWidget->removeRobotModel();});



    scriptDlg->setConnectPtr(connectDlg); //将connectDlg指针传给script


    /********Joint Position Widgets********/
    ui->Joint1PosWidget->setName("#1 ");
    ui->Joint1PosWidget->setId(0);
    connect(ui->Joint1PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint1PosWidget);

    ui->Joint2PosWidget->setName("#2 ");
    ui->Joint2PosWidget->setId(1);
    connect(ui->Joint2PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint2PosWidget);

    ui->Joint3PosWidget->setName("#3 ");
    ui->Joint3PosWidget->setId(2);
    connect(ui->Joint3PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint3PosWidget);

    ui->Joint4PosWidget->setName("#4 ");
    ui->Joint4PosWidget->setId(3);
    connect(ui->Joint4PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint4PosWidget);

    ui->Joint5PosWidget->setName("#5 ");
    ui->Joint5PosWidget->setId(4);
    connect(ui->Joint5PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint5PosWidget);

    ui->Joint6PosWidget->setName("#6 ");
    ui->Joint6PosWidget->setId(5);
    connect(ui->Joint6PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint6PosWidget);

    ui->Joint7PosWidget->setName("#7 ");
    ui->Joint7PosWidget->setId(6);
    connect(ui->Joint7PosWidget, &JointPositionWidget::jointPositionJogging,
            this, [=](int id, int dir) {
                connectDlg->jointJogging(id, dir);
            });
    jpWdgs.push_back(ui->Joint7PosWidget);

    /********Cartesian Position Widgets********/
    ui->cartesianXWidget->setName("X");
    ui->cartesianXWidget->setId(0);
    connect(ui->cartesianXWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 0, id, dir);
            });
    cpWdgs.push_back(ui->cartesianXWidget);

    ui->cartesianYWidget->setName("Y");
    ui->cartesianYWidget->setId(1);
    connect(ui->cartesianYWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 1, id, dir);
            });
    cpWdgs.push_back(ui->cartesianYWidget);

    ui->cartesianZWidget->setName("Z");
    ui->cartesianZWidget->setId(2);
    connect(ui->cartesianZWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 2, id, dir);
            });
    cpWdgs.push_back(ui->cartesianZWidget);

    ui->cartesianRollWidget->setName("r");
    ui->cartesianRollWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianRollWidget->setId(3);
    connect(ui->cartesianRollWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 3, id, dir);
            });
    cpWdgs.push_back(ui->cartesianRollWidget);

    ui->cartesianPitchWidget->setName("p");
    ui->cartesianPitchWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianPitchWidget->setId(4);
    connect(ui->cartesianPitchWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 4, id, dir);
            });
    cpWdgs.push_back(ui->cartesianPitchWidget);

    ui->cartesianYawWidget->setName("y");
    ui->cartesianYawWidget->setAngleRep(ANGLE_DEGREE);
    ui->cartesianYawWidget->setId(5);
    connect(ui->cartesianYawWidget, &CartesianPositionWidget::cartesianJogging,
            this, [=](int id, int dir) {
                connectDlg->cartesianJogging(currentFrame + 5, id, dir);
            });
    cpWdgs.push_back(ui->cartesianYawWidget);

    /********Speed Scaling Widgets********/
    ui->speedPercent->setText(tr("25%"));


//    connect(connectDlg, &ConnectDialog::speedScaling, this, [=](double f100){ ui->speedSlider->setValue(f100*10);}); //更新速度缩放因子

    /////////////////////////////////////////////////////
    connect(connectDlg, &ConnectDialog::showRobot, this, [=](QString path){
        ui->visualWidget->displayModelFromYaml(path.toStdString());
    });

    connect(connectDlg, &ConnectDialog::jointPositions, this, &RocosMainWindow::updateJointPos); //关节位置更新
    connect(connectDlg, &ConnectDialog::cartPose, this, &RocosMainWindow::updateCartPose); //笛卡尔位置更新
    connect(connectDlg, &ConnectDialog::logging, this, [=](QByteArray &ba) {
//        ui->logBrowser->insertPlainText(QTime::currentTime().toString("[HH:mm:ss.zzz] ") + QString(ba) + "\n");
//        ui->logBrowser->
//        ui->logEdit->moveCursor(QTextCursor::End);
//        QString str = QTime::currentTime().toString("[HH:mm:ss.zzz] ") + QString(ba) + "\n";
//        ui->logEdit->appendPlainText(str.simplified());

    });

    /********connect信号处理********/
    connect(connectDlg, &ConnectDialog::connectState, this, [=](bool isConnected) {
        if (isConnected) {
            ui->actionConnected->setIcon(QIcon(":/res/connected.png"));
        } else {
            ui->actionConnected->setIcon(QIcon(":/res/disconnected.png"));
        }
    });

    /*********RobotState数据处理******/
    connect(connectDlg, &ConnectDialog::newStateComming, this, &RocosMainWindow::updateRobotState);

    ui->plotWidget->setConnectPtr(connectDlg); //保存指针
    connect(connectDlg, &ConnectDialog::newStateComming, ui->plotWidget, &PlotWidget::handleNewState);

    ///////////////////////////////////////////////////////

    /*********** Joystick **************/
    grpBox = ui->cartesianGroupBox; //默认是BaseT 0 -> BaseR 1 -> FlangeT 2 -> FlangeR 3


}

RocosMainWindow::~RocosMainWindow() {
    delete ui;
}


void RocosMainWindow::on_pushButton_clicked() {
    QVector<double> jntRads;
    jntRads.push_back(rand() / double(RAND_MAX));
    jntRads.push_back(rand() / double(RAND_MAX));
    jntRads.push_back(rand() / double(RAND_MAX));
    jntRads.push_back(rand() / double(RAND_MAX));
    jntRads.push_back(rand() / double(RAND_MAX));
    jntRads.push_back(rand() / double(RAND_MAX));
//    jntRads.push_back(rand()/double(RAND_MAX));
//    this->ui->visualWidget->setJointPos(jntRads);

    updateJointPos(jntRads);
}

void RocosMainWindow::on_axesCheckBox_toggled(bool checked) {
    if (checked) {
        this->ui->visualWidget->setJointAxesVisibility(true);
    } else {
        this->ui->visualWidget->setJointAxesVisibility(false);
    }
}

void RocosMainWindow::on_speedSlider_valueChanged(int value) {
    double f = value / 1000.0;
    ui->speedPercent->setText(tr("%1%").arg((int) (f * 100)));

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

    connectDlg->setJointSpeedScaling(f);
}

void RocosMainWindow::on_actionEnabled_clicked() {

    if (isRobotEnabled) {
        connectDlg->powerOff();
    } else {
        connectDlg->powerOn();
    }
}

void RocosMainWindow::on_actionConnected_clicked() {
    if (connectDlg->isConnected()) {
        connectDlg->shutdown();
    } else {
        connectDlg->exec();
    }

}

void RocosMainWindow::on_meshCheckBox_toggled(bool checked) {
    if (checked) {
        this->ui->visualWidget->setMeshVisibility(true);
    } else {
        this->ui->visualWidget->setMeshVisibility(false);
    }
}

void RocosMainWindow::on_actionScript_clicked() {
//    isRobotEnabled = false;
//    ui->actionEnabled->setIcon(QIcon(":/res/switchoff.png"));

    scriptDlg->exec();
}

void RocosMainWindow::on_actionPlotter_clicked() {
//    plotDlg->show();
    static int idx = 0;
    if(idx == 0)
        idx = 1;
    else
        idx = 0;

    ui->stackedWidget->setCurrentIndex(idx);
}

void RocosMainWindow::on_actionAbout_clicked() {
    aboutDlg->exec();
}

void RocosMainWindow::on_actionAxoView_clicked() {
    ui->visualWidget->setAxoView();
}

void RocosMainWindow::on_actionTopView_clicked() {
    ui->visualWidget->setZAxisView();
}

void RocosMainWindow::on_actionFrontView_clicked() {
    ui->visualWidget->setXAxisView();
}

void RocosMainWindow::on_actionRightView_clicked() {
    ui->visualWidget->setYAxisView();
}

void RocosMainWindow::on_baseFrame_clicked(bool checked) {
    if (checked) {
        currentFrame = FRAME_BASE;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (BASE)"));
    }
}

void RocosMainWindow::on_flangeFrame_clicked(bool checked) {
    if (checked) {
        currentFrame = FRAME_FLANGE;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (FLANGE)"));
    }
}

void RocosMainWindow::on_toolFrame_clicked(bool checked) {
    if (checked) {
        currentFrame = FRAME_TOOL;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (TOOL)"));
    }
}

void RocosMainWindow::on_objectFrame_clicked(bool checked) {
    if (checked) {
        currentFrame = FRAME_OBJECT;
        ui->cartesianGroupBox->setTitle(tr("Cartesian Space (OBJECT)"));
    }
}


void RocosMainWindow::on_actionAngleRep_triggered() {
    isAngleDegree = !isAngleDegree;

    int angleRep = isAngleDegree ? ANGLE_DEGREE : ANGLE_RADIUS;

    r2d = (angleRep == ANGLE_DEGREE) ? (180.0 / M_PI) : 1.0; // TODO：这个给precision control使用的，后续要封装起来


    for (auto &jpWdg: jpWdgs) {
        jpWdg->setAngleRep(angleRep);
    }
    ui->cartesianRollWidget->setAngleRep(angleRep);
    ui->cartesianPitchWidget->setAngleRep(angleRep);
    ui->cartesianYawWidget->setAngleRep(angleRep);

    if (isAngleDegree) {
        ui->actionAngleRep->setIcon(QIcon(":/res/degree.png"));
    } else {
        ui->actionAngleRep->setIcon(QIcon(":/res/radius.png"));
    }
}

void RocosMainWindow::on_actionPosRep_triggered() {
    isPosMM = !isPosMM;

    int posRep = isPosMM ? POS_MM : POS_M;

    m2mm = (posRep == POS_MM) ? 1000.0 : 1.0;

    ui->cartesianXWidget->setPosRep(posRep);
    ui->cartesianYWidget->setPosRep(posRep);
    ui->cartesianZWidget->setPosRep(posRep);

    if (isPosMM) {
        ui->actionPosRep->setIcon(QIcon(":/res/mm.png"));
    } else {
        ui->actionPosRep->setIcon(QIcon(":/res/m.png"));
    }


}

//void RocosMainWindow::on_logEdit_customContextMenuRequested(const QPoint &pos) {
//    Q_UNUSED(pos); //这个pos显示的位置不对

////    QMenu *menu = new QMenu(this);
////    QAction *action = new QAction(tr("clear"), this);
////    connect(action, &QAction::triggered, ui->logEdit, &QPlainTextEdit::clear);

////    menu->addAction(action);
////    menu->move(cursor().pos());
////    menu->show();

//}

void RocosMainWindow::updateJointPos(QVector<double> &jntPos) {
    for (int i = 0; i < jntPos.size(); i++) {
        jpWdgs[i]->updateJointPosition(jntPos[i]); //更新关节位置
    }

//    jntPos.resize(7); //TODO: 这句话实际需要屏蔽
    ui->visualWidget->setJointPos(jntPos);
}

void RocosMainWindow::updateCartPose(QVector<double> &pose) {
    if (pose.size() != 6) {
        return;
    }

    for (int i = 0; i < cpWdgs.size(); i++) {
        cpWdgs[i]->updateVal(pose[i]);
    }
}

void RocosMainWindow::updateRobotState() {
    //更新关节位置
    QVector<double> jntPos;
    for (int i = 0; i < connectDlg->getJointNum(); i++) {
        jpWdgs[i]->updateJointPosition(connectDlg->getJointPosition(i)); //更新关节位置
        jntPos.push_back(connectDlg->getJointPosition(i));
    }

//    jntPos.resize(7); //TODO: 这句话实际需要屏蔽
    ui->visualWidget->setJointPos(jntPos);



    //更新Flange系
    auto flange = connectDlg->getFlangePose();
    cpWdgs[0]->updateVal(flange.p.x());
    cpWdgs[1]->updateVal(flange.p.y());
    cpWdgs[2]->updateVal(flange.p.z());

    double roll, pitch, yaw;
    flange.M.GetRPY(roll, pitch, yaw);
    cpWdgs[3]->updateVal(roll);
    cpWdgs[4]->updateVal(pitch);
    cpWdgs[5]->updateVal(yaw);

    //更新使能状态
    int cnt = 0;
    for (int i = 0; i < connectDlg->getJointNum(); ++i) {
        if (connectDlg->getJointStatus(i).contains("Enabled"))
            cnt++;
    }
    if (cnt == connectDlg->getJointNum())
        isRobotEnabled = true;
    else
        isRobotEnabled = false;

    // 更新使能按钮图标
    if (isRobotEnabled) {
        ui->actionEnabled->setIcon(QIcon(":/res/switchon.png"));
    } else {
        ui->actionEnabled->setIcon(QIcon(":/res/switchoff.png"));
    }
}

void RocosMainWindow::on_MoveJ_clicked() {
    qDebug() << "MoveJ";
    QVector<double> q;
    q.push_back(ui->j1->value() / r2d);
    q.push_back(ui->j2->value() / r2d);
    q.push_back(ui->j3->value() / r2d);
    q.push_back(ui->j4->value() / r2d);
    q.push_back(ui->j5->value() / r2d);
    q.push_back(ui->j6->value() / r2d);
    q.push_back(ui->j7->value() / r2d);

    connectDlg->moveJ(q);
}

void RocosMainWindow::on_MoveJ_IK_clicked() {
    qDebug() << "MoveJ_IK";
    QVector<double> pose;
    pose.push_back(ui->x->value() / m2mm);
    pose.push_back(ui->y->value() / m2mm);
    pose.push_back(ui->z->value() / m2mm);
    pose.push_back(ui->roll->value() / r2d);
    pose.push_back(ui->pitch->value() / r2d);
    pose.push_back(ui->yaw->value() / r2d);

    connectDlg->moveJ_IK(pose);
}

void RocosMainWindow::on_MoveL_FK_clicked() {
    qDebug() << "MoveL_FK";
    QVector<double> q;
    q.push_back(ui->j1->value() / r2d);
    q.push_back(ui->j2->value() / r2d);
    q.push_back(ui->j3->value() / r2d);
    q.push_back(ui->j4->value() / r2d);
    q.push_back(ui->j5->value() / r2d);
    q.push_back(ui->j6->value() / r2d);
    q.push_back(ui->j7->value() / r2d);

    connectDlg->moveL_FK(q);
}

void RocosMainWindow::on_MoveL_clicked() {
    qDebug() << "MoveL";
    QVector<double> pose;
    pose.push_back(ui->x->value() / m2mm);
    pose.push_back(ui->y->value() / m2mm);
    pose.push_back(ui->z->value() / m2mm);
    pose.push_back(ui->roll->value() / r2d);
    pose.push_back(ui->pitch->value() / r2d);
    pose.push_back(ui->yaw->value() / r2d);

    connectDlg->moveL(pose);
}

void RocosMainWindow::on_MoveSpace0_clicked() {
    qDebug() << "Move 0-Space";
}


void RocosMainWindow::on_workingModeBox_currentIndexChanged(int index)
{
    qDebug() << "Robot Mode Changing...";
    connectDlg->setRobotWorkMode(index);

}


void RocosMainWindow::on_actionDispModel_clicked()
{
//    ui->visualWidget->displayModelFromYaml("models/talon/config.yaml");

    if(modelLoaderDlg->exec() == QDialog::Accepted) {
        ui->visualWidget->displayModelFromYaml(modelLoaderDlg->getCfgFileName().toStdString());
    }



}


void RocosMainWindow::on_actionPlotterConfig_clicked()
{
    if(plotConfigDlg->exec() == QDialog::Accepted) {
        ui->plotWidget->processTree(plotConfigDlg->tree);
    }
}


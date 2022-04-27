#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

#include <QDebug>

#include <Protocol.h> //通讯协议

const int POLLING_INTERVAL_MS = 100; // 轮询间隔 100ms

ConnectDialog::ConnectDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConnectDialog) {
    ui->setupUi(this);

    this->setWindowFlag(Qt::FramelessWindowHint);

    ui->ipAddressEdit->setText(ip_address_); //更新界面默认值
    ui->portEdit->setText(QString::number(port_)); //更新界面默认值

    timer_state_ = new QTimer(this);
    connect(timer_state_, &QTimer::timeout, this, &ConnectDialog::getRobotState); //获取机器人状态

}

ConnectDialog::~ConnectDialog() {
    delete ui;
}

void ConnectDialog::on_connectButton_clicked() {
    connectedToRobot(true);
}

void ConnectDialog::on_ipAddressEdit_textChanged(const QString &ip) {
    ip_address_ = ip;
}

void ConnectDialog::on_portEdit_textChanged(const QString &p) {
    port_ = p.toInt();
}

void ConnectDialog::connectedToRobot(bool con) {
    if (con) { //连接机器人
        if (is_connected_)
            return;

        channel_ = grpc::CreateChannel(QString("%1:%2").arg(ip_address_).arg(port_).toStdString(),
                                       grpc::InsecureChannelCredentials());

        stub_ = RobotService::NewStub(channel_);

        //获取一下RobotInfo,顺便测试一下连接否成功
        RobotInfoRequest request;
//    RobotInfoResponse response;
        Status status;

        //之前发现如果退出程序，重启只发送一次会卡住，所以多发几次，但这个问题需要仔细研究一下
        // 后来发现不是这个原因造成的,是由于channel_没有释放造成的
        ClientContext context; //这个只能使用一次，每次请求都需要重新创建
        status = stub_->ReadRobotInfo(&context, request, &robot_info_response_);
//    }

        if (status.ok()) {
            emit connectState(true);
            timer_state_->start(POLLING_INTERVAL_MS); // 如果连接上机器人，则以POLLING_INTERVAL_MS间隔轮询数据
            this->close();
            is_connected_ = true;

            cnt_per_unit_.resize(robot_info_response_.robot_info().joint_infos_size());
            torque_per_unit_.resize(robot_info_response_.robot_info().joint_infos_size());
            load_per_unit_.resize(robot_info_response_.robot_info().joint_infos_size());
            ratio_.resize(robot_info_response_.robot_info().joint_infos_size());
            pos_zero_offset_.resize(robot_info_response_.robot_info().joint_infos_size());

            user_unit_name_.resize(robot_info_response_.robot_info().joint_infos_size());
            torque_unit_name_.resize(robot_info_response_.robot_info().joint_infos_size());
            load_unit_name_.resize(robot_info_response_.robot_info().joint_infos_size());

            jnt_num_ = robot_info_response_.robot_info().joint_infos_size();

            for (int i = 0; i < robot_info_response_.robot_info().joint_infos_size(); ++i) {
                cnt_per_unit_[i] = robot_info_response_.robot_info().joint_infos().at(i).cnt_per_unit();
                torque_per_unit_[i] = robot_info_response_.robot_info().joint_infos().at(i).torque_per_unit();
                //TODO: load_per_unit
                user_unit_name_[i] = QString::fromStdString(
                        robot_info_response_.robot_info().joint_infos().at(i).user_unit_name());
                //TODO: torque_unit_name 及 load_unit_name

                qDebug() << "Joint " << i << ": ";
                qDebug() << "    cnt_per_unit: "
                         << robot_info_response_.robot_info().joint_infos().at(i).cnt_per_unit();
                qDebug() << "    torque_per_unit: "
                         << robot_info_response_.robot_info().joint_infos().at(i).torque_per_unit();
                qDebug() << "    ratio: " << robot_info_response_.robot_info().joint_infos().at(i).ratio();
                qDebug() << "    pos_zero_offset: "
                         << robot_info_response_.robot_info().joint_infos().at(i).pos_zero_offset();
                qDebug() << "    user_unit_name: "
                         << robot_info_response_.robot_info().joint_infos().at(i).user_unit_name().c_str();

            }

        } else {
            emit connectState(false);
            is_connected_ = false;
        }
    } else { //断开连接机器人
        timer_state_->stop();
        while (channel_.use_count())
            channel_.reset();

        is_connected_ = false;
        emit connectState(false);
    }
}

/// \brief 机械臂多关节使能
/// \param enabled
void ConnectDialog::setRobotEnabled(bool enabled) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    if (enabled)
        request.mutable_command()->mutable_enabled();
    else
        request.mutable_command()->mutable_disabled();

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cerr << "Send command Error" << std::endl;
    }
}

void ConnectDialog::setJointSpeedScaling(double factor) {

}

void ConnectDialog::getJointSpeedScaling() {

}

void ConnectDialog::setCartesianSpeedScaling(double factor) {
}

void ConnectDialog::getCartesianSpeedScaling() {

}

void ConnectDialog::setToolSpeedScaling(double factor) {
}

void ConnectDialog::getToolSpeedScaling() {

}

void ConnectDialog::startScript(QString script) {

}

void ConnectDialog::stopScript() {

}

void ConnectDialog::pauseScript() {

}

void ConnectDialog::continueScript() {

}

void ConnectDialog::setZeroCalibration() {

}

void ConnectDialog::getRobotState() {
    RobotStateRequest request;

    if (use_raw_data_) {
        request.set_raw_data(true);
    }

//    RobotStateResponse robot_state_response_;
    robot_state_response_.Clear();

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->ReadRobotState(&context, request, &robot_state_response_);
    if (status.ok()) {
        emit newStateComming(); // 收到了新的机器人状态信息就直接上传

//        std::cout << "JntPos: ";
//        for(int i =0; i < jnt_num_; ++i) {
//            std::cout << getJointPosition(i) << " ; ";
//        }
//        std::cout << std::endl;
//        std::cout << "Joint state size: " << robot_state_response_.robot_state().joint_states_size() << std::endl;
//        std::cout << "Joint state status: " << robot_state_response_.robot_state().joint_states(0).status() << std::endl;
//        std::cout << "slave num: " << robot_state_response_.robot_state().hw_state().slave_num() << std::endl;
//        std::cout << "hw type: " << robot_state_response_.robot_state().hw_state().hw_type() << std::endl;
//        std::cout << "curr cycle time: " << robot_state_response_.robot_state().hw_state().current_cycle_time() << std::endl;
    } else {
        std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
}

void ConnectDialog::jointJogging(int id, int dir) {
    qDebug() << "Joint " << id << " is jogging...";

    RobotCommandRequest request;
    RobotCommandResponse response;

    auto dragging_command = request.mutable_command()->mutable_dragging_command();
    dragging_command->set_flag(static_cast<rocos::DraggingFlag>(id));
    dragging_command->set_dir(static_cast<rocos::DraggingDirection>(dir));
    dragging_command->set_max_speed(1); // TODO: 需要获取关节最大速度
    dragging_command->set_max_acceleration(10); // TODO: 需要获取关节最大加速度

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cerr << "Send command Error" << std::endl;
    }
}

void ConnectDialog::cartesianJogging(int frame, int freedom, int dir) {

}

bool ConnectDialog::event(QEvent *event) {
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            this->close();
        }
    }

    return QDialog::event(event);
}

void ConnectDialog::powerOn() {
    RobotCommandRequest request;
    RobotCommandResponse response;

    request.mutable_command()->mutable_enabled();

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cerr << "Send command Error" << std::endl;
    }
}

void ConnectDialog::powerOff() {
    RobotCommandRequest request;
    RobotCommandResponse response;

    request.mutable_command()->mutable_disabled();

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

void ConnectDialog::shutdown() {
    timer_state_->stop();
    while (channel_.use_count())
        channel_.reset();

    is_connected_ = false;
    emit connectState(false);
}

void ConnectDialog::powerOn(int id) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    request.mutable_command()->mutable_single_axis_command()->mutable_enabled()->set_id(id);

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

void ConnectDialog::powerOff(int id) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    request.mutable_command()->mutable_single_axis_command()->mutable_disabled()->set_id(id);

    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cerr << "Send command Error" << std::endl;
    }
}

QString ConnectDialog::getHardwareType() {
    switch (robot_state_response_.robot_state().hw_state().hw_type()) {
        case rocos::HardwareState::HardwareType::HardwareState_HardwareType_HW_TYPE_UNKNOWN:
            return tr("Unknown");
        case rocos::HardwareState::HardwareType::HardwareState_HardwareType_HW_TYPE_SIM:
            return tr("Simulation");
        case rocos::HardwareState::HardwareType::HardwareState_HardwareType_HW_TYPE_ETHERCAT:
            return tr("EtherCAT");
        default:
            return "";
    }
}

QString ConnectDialog::getJointStatus(int id) {
    switch (robot_state_response_.robot_state().joint_states(id).status()) {
        case rocos::JointState_Status::JointState_Status_STATUS_FAULT:
            return tr("Fault");
        case rocos::JointState_Status::JointState_Status_STATUS_DISABLED:
            return tr("Disabled");
        case rocos::JointState_Status::JointState_Status_STATUS_ENABLED:
            return tr("Enabled");
        default:
            return "";
    }
}

void ConnectDialog::moveJ(QVector<double> q) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    auto movej = request.mutable_command()->mutable_motion_command()->mutable_move_j();
    for(int i = 0; i < jnt_num_; ++i) {
        qDebug() << "q[" << i << "]: " << q[i];
        movej->mutable_q()->add_data(q[i]);
    }
    movej->set_speed(vj_);
    movej->set_acceleration(aj_);
    movej->set_time(0);
    movej->set_radius(0);
    movej->set_asynchronous(true);


    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

void ConnectDialog::moveL(QVector<double> pose) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    auto movel = request.mutable_command()->mutable_motion_command()->mutable_move_l();
    movel->mutable_pose()->mutable_position()->set_x(pose[0]);
    movel->mutable_pose()->mutable_position()->set_y(pose[1]);
    movel->mutable_pose()->mutable_position()->set_z(pose[2]);

    auto rotation = Rotation::RPY(pose[3], pose[4], pose[5]);
    double x,y,z,w;
    rotation.GetQuaternion(x,y,z,w);
    movel->mutable_pose()->mutable_rotation()->set_x(x);
    movel->mutable_pose()->mutable_rotation()->set_y(y);
    movel->mutable_pose()->mutable_rotation()->set_z(z);
    movel->mutable_pose()->mutable_rotation()->set_w(w);

    movel->set_speed(vc_);
    movel->set_acceleration(ac_);
    movel->set_time(0);
    movel->set_radius(0);
    movel->set_asynchronous(true);


    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

void ConnectDialog::moveJ_IK(QVector<double> pose) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    auto movej_ik = request.mutable_command()->mutable_motion_command()->mutable_move_j_ik();
    movej_ik->mutable_pose()->mutable_position()->set_x(pose[0]);
    movej_ik->mutable_pose()->mutable_position()->set_y(pose[1]);
    movej_ik->mutable_pose()->mutable_position()->set_z(pose[2]);

    auto rotation = Rotation::RPY(pose[3], pose[4], pose[5]);
    double x,y,z,w;
    rotation.GetQuaternion(x,y,z,w);
    movej_ik->mutable_pose()->mutable_rotation()->set_x(x);
    movej_ik->mutable_pose()->mutable_rotation()->set_y(y);
    movej_ik->mutable_pose()->mutable_rotation()->set_z(z);
    movej_ik->mutable_pose()->mutable_rotation()->set_w(w);

    movej_ik->set_speed(vj_);
    movej_ik->set_acceleration(aj_);
    movej_ik->set_time(0);
    movej_ik->set_radius(0);
    movej_ik->set_asynchronous(true);


    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

void ConnectDialog::moveL_FK(QVector<double> q) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    auto movel_fk = request.mutable_command()->mutable_motion_command()->mutable_move_l_fk();
    for(int i = 0; i < jnt_num_; ++i) {
        movel_fk->mutable_q()->add_data(q[i]);
    }
    movel_fk->set_speed(vc_);
    movel_fk->set_acceleration(ac_);
    movel_fk->set_time(0);
    movel_fk->set_radius(0);
    movel_fk->set_asynchronous(true);


    ClientContext context; //这个只能使用一次，每次请求都需要重新创建
    Status status = stub_->WriteRobotCommmand(&context, request, &response);

    if (status.ok()) {
//        std::cout << "Send command Ok" << std::endl;
    } else {
        std::cout << "Send command Error" << std::endl;
    }
}

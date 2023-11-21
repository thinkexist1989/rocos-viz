

// #include <QMainWindow>
// #include <QLabel>
// #include <QTimer>
// #include <QTime>
// #include <QGroupBox>
#include <Eigen/Dense>
#include <QDialog>
#include <QTcpSocket>
#include <QTimer>

#include <grpcpp/grpcpp.h>
#include "robot_service.grpc.pb.h"

#include <kdl/frames.hpp>
#include <kdl/frames_io.hpp>

#define POSITION_MODE 0
#define EE_ADMIT_TEACH_MODE 1
#define JNT_ADMIT_TEACH_MODE 2
#define JNT_IMP_MODE 3
#define CART_IMP_MODE 4

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using rocos::RobotService;
using rocos::RobotStateRequest;
using rocos::RobotStateResponse;

using rocos::RobotCommand;
using rocos::RobotCommandRequest;
using rocos::RobotCommandResponse;

using rocos::RobotInfoRequest;
using rocos::RobotInfoResponse;

using KDL::Frame;
using KDL::JntArray;
using KDL::Rotation;
using KDL::Vector;

    QString ip_address_{"127.0.0.1"}; //!< IP地址
    int port_{30001}; //!< IP端口号
    bool is_connected_{false}; //!<　是否连接标志位

    QTimer *timer_state_; //!<　定时器，用于定期获取机器人状态信息

    std::unique_ptr<RobotService::Stub> stub_; //!< grpc存根
    std::shared_ptr<Channel> channel_;  //!< gRPC Channel

    RobotInfoResponse robot_info_response_;   //!< 机器人信息回复
    RobotStateResponse robot_state_response_; //!< 机器人状态回复

    double factor_ {0.25}; //!< 机器人运行速度缩放比例
    double max_jnt_speed_ {1}; //!< 关节空间运动最大速度
    double max_cart_speed_ {0.5}; //!< 笛卡尔空间运动最大速度

    int jnt_num_ {0};
    double  vj_ {0.1};
    double  vc_ {0.1};
    double  aj_ {1.4};
    double  ac_ {1.4};

    bool use_raw_data_{false};         //!< 是否使用原始类型数据

    QVector<double> cnt_per_unit_;     //!< 用户单位对应脉冲数
    QVector<double> torque_per_unit_;  //!< 力矩单位对应脉冲数
    QVector<double> load_per_unit_;    //!< 力矩传感器单位对应脉冲数
    QVector<int32_t> pos_zero_offset_; //!< 位置零位偏移
    QVector<double> ratio_;            //!< 减速比
    QVector<QString> user_unit_name_;  //!< 用户单位名称字符串
    QVector<QString> torque_unit_name_;//!< 力矩单位名称字符串
    QVector<QString> load_unit_name_;  //!< 力矩传感器单位名称字符串
  

void connectedToRobot(bool con , QString ip_address_ , int port_) {


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
        std::cout << "status:" << status.ok() << std::endl;
         if (status.ok()) {
            
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
            qDebug() << "Failed to connect to robot: " << status.error_code() << ": " << status.error_message().c_str();
            is_connected_ = false;
        }
    } 
QString getJointStatus(int id) {
    switch (robot_state_response_.robot_state().joint_states(id).status()) {
        case rocos::JointState_Status::JointState_Status_STATUS_FAULT:
            return "Fault";
        case rocos::JointState_Status::JointState_Status_STATUS_DISABLED:
            return "Disabled";
        case rocos::JointState_Status::JointState_Status_STATUS_ENABLED:
            return"Enabled";
        default:
            return "";
    }
}

void powerOn() {
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

void powerOff() {
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

void shutdown() {
    timer_state_->stop();
    while (channel_.use_count())
        channel_.reset();

    is_connected_ = false;
    
}
 double getJointPosition(int id) {

        //获取一下RobotInfo,顺便测试一下连接否成功
        RobotStateRequest request;
//    RobotInfoResponse response;
        Status status;

        //之前发现如果退出程序，重启只发送一次会卡住，所以多发几次，但这个问题需要仔细研究一下
        // 后来发现不是这个原因造成的,是由于channel_没有释放造成的
        ClientContext context; //这个只能使用一次，每次请求都需要重新创建
        status = stub_->ReadRobotState(&context, request, &robot_state_response_);
    
        return robot_state_response_.robot_state().joint_states(id).position();
    }

    void moveJ(QVector<double> q) {
    RobotCommandRequest request;
    RobotCommandResponse response;

    auto movej = request.mutable_command()->mutable_motion_command()->mutable_move_j();
    for(int i = 0; i < jnt_num_; ++i) {
        qDebug() << "q[" << i << "]: " << q[i];
        movej->mutable_q()->add_data(q[i]);
    }
//    movej->set_speed(vj_);
//    movej->set_acceleration(aj_);
    movej->set_speed(max_jnt_speed_ * factor_);
    movej->set_acceleration(max_jnt_speed_ * factor_ * 1);
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



int main()
{
    
    connectedToRobot(true, ip_address_, port_);
    powerOn();
     std::cout << "hello world" << std::endl;
    // QString joint_status = getJointStatus(0);

    // std::cout << joint_status.toStdString() << std::endl;
    powerOff();
     std::cout << "hello world" << std::endl;
    getJointPosition(0);
    QVector<double> q={0,0,0,0,0,0,1.5707};
    moveJ(q);
    std::cout << "hello world" << std::endl;
    return 0;
    
}
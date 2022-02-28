#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QTimer>

#include <Eigen/Dense>

#include <grpcpp/grpcpp.h>
#include "robot_service.grpc.pb.h"

#include <kdl/frames.hpp>
#include <kdl/frames_io.hpp>

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

namespace Ui {
    class ConnectDialog;
}

class ConnectDialog : public QDialog {
Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);

    ~ConnectDialog();

    void on_connectButton_clicked();

    void on_ipAddressEdit_textChanged(const QString &ip);

    void on_portEdit_textChanged(const QString &p);

public slots:
    /**
     * 获取硬件主站相关信息
     */

    //! \brief 获取硬件信息
    //! \return 硬件信息字符串，Unknown/Simulation/EtherCAT
    QString getHardwareType();

    //! \brief 获取最短循环周期
    //! \return 最短循环时间[ms]
    inline double getMinCyclicTime() const { return robot_state_response_.robot_state().hw_state().min_cycle_time(); }

    //! \brief 获取最大循环周期
    //! \return 最大循环时间[ms]
    inline double getMaxCyclicTime() const { return robot_state_response_.robot_state().hw_state().max_cycle_time(); }

    //! \brief 获取当前循环周期
    //! \return 当前魂环时间[ms]
    inline double
    getCurrCyclicTime() const { return robot_state_response_.robot_state().hw_state().current_cycle_time(); }

    inline int getJointNum() const { return robot_state_response_.robot_state().joint_states_size(); }

    /////////////////////////////////////////////////////////
    ///////////////     获取关节状态       ///////////////////
    /////////////////////////////////////////////////////////

    /// \brief 获取关节当前状态，上电/下电/错误
    /// \param id 关节ID（From 0）
    /// \return
    QString getJointStatus(int id);

    inline QString getJointName(int id) const {
        return QString{robot_state_response_.robot_state().joint_states(id).name().c_str()};
    }

    inline double getJointPosition(int id) const {
        return robot_state_response_.robot_state().joint_states(id).position();
    }

    inline double getJointVelocity(int id) const {
        return robot_state_response_.robot_state().joint_states(id).velocity();
    }

    inline double getJointTorque(int id) const {
        return robot_state_response_.robot_state().joint_states(id).acceleration();
    }

    inline double getJointLoad(int id) const { return robot_state_response_.robot_state().joint_states(id).load(); }

    /////////////////////////////////////////////////////////
    ///////////////     获取关节信息       ///////////////////
    /////////////////////////////////////////////////////////

    //! \brief 获取关节用户单位对应脉冲数
    //! \param id 关节ID（From 0）
    //! \return  cnt_per_unit
    inline double getJointCntPerUnit(int id) const {
        return robot_info_response_.robot_info().joint_infos().at(id).cnt_per_unit();
    }

    //!
    //! \param id
    //! \return
    inline double getJointTorquePerUnit(int id) const {
        return robot_info_response_.robot_info().joint_infos().at(id).torque_per_unit();
    }

    //!
    //! \param id
    //! \return
    inline double getJointRatio(int id) const { return robot_info_response_.robot_info().joint_infos().at(id).ratio(); }

    //!
    //! \param id
    //! \return
    inline int32_t getJointPosZeroOffset(int id) const {
        return robot_info_response_.robot_info().joint_infos().at(id).pos_zero_offset();
    }

    //!
    //! \param id
    //! \return
    inline QString getJointUserUnitName(int id) const {
        return QString{robot_info_response_.robot_info().joint_infos().at(id).user_unit_name().c_str()};
    }

    /////////////////////////////////////////////////////////
    ///////////////      运动学函数        ///////////////////
    /////////////////////////////////////////////////////////

    //!
    //! \return
    inline Frame getEndPose() const {
        return Frame();
    }



public slots:

    void powerOn();

    void powerOff();

    void powerOn(int id);

    void powerOff(int id);

//    void setJointMode(int id, int mode);

    void shutdown(); // 断开和机器人连接

//    void setSync(int sync);


//    //////////Single Axis Move/////////////////
//    void moveSingleAxis(int id, double pos, double max_vel = -1, double max_acc = -1, double max_jerk = -1,
//                        double least_time = -1);
//
//    void stopSingleAxis(int id);
//
//    //////////Multi Axis Move/////////////////
//    void moveMultiAxis(const QVector<double> &pos, const QVector<double> &max_vel, const QVector<double> &max_acc,
//                       const QVector<double> &max_jerk, double least_time = -1);
//
//    void stopMultiAxis();



    void connectedToRobot(bool con);  //连接到机器人

    void setRobotEnabled(bool enabled); //操作：机器人是否上电

    void getRobotState(); // 操作：发送GET_INFO

    void setJointSpeedScaling(double factor); //关节速度
    void getJointSpeedScaling(); //获取关节速度

    void setCartesianSpeedScaling(double factor); //笛卡尔速度
    void getCartesianSpeedScaling(); //获取 笛卡尔速度

    void setToolSpeedScaling(double factor); // 工具速度
    void getToolSpeedScaling(); //获取工具速度

    void startScript(QString script);

    void stopScript();

    void pauseScript();

    void continueScript();

    void setZeroCalibration();

public:
    inline bool isConnected() { return is_connected_; }  // 是否已经连接
//    inline bool getRobotEnabled() { return isRobotEnabled; } // TODO: 获取Enable状态

    void jointJogging(int id, int dir); //关节点动
    void cartesianJogging(int frame, int freedom, int dir); //笛卡尔点动
    void jogging(int frame, int freedom, int dir); //两种点动可以合在一起

signals:
    void jointPositions(QVector<double> &jntPos); //TODO(yang luo): 解析到关节位置，发送 信号, 已废弃, new state comming中包含了机器人各种状态信息
    void cartPose(QVector<double> &pose); // TODO(yang luo): 解析到笛卡尔空间位置，发送 信号, 已废弃, new state comming中包含了机器人各种状态信息
//    void speedScaling(double f100); // TODO(yang luo): 速度缩放因数 25.0,已废弃,规划的scaling直接在rocos-viz中完成
    void logging(QByteArray &ba); //TODO(yang luo): 返回的日志信息,暂时还未用到

    void newStateComming(void); // 机器人状态更新信号, 如果收到了机器人的状态信息就通知主界面更新

    void connectState(bool isConnected); // 连接状态信号, 通知与机器人rpc连接状态

private:
    bool event(QEvent *event) override;

    Ui::ConnectDialog *ui;

    QTcpSocket *tcpSocket = Q_NULLPTR; //连接机器人控制器

//    bool isRobotEnabled = false; //机器人默认不上电

    QString ip_address_{"192.168.0.194"};
    int port_{30001};
    bool is_connected_{false};

    QTimer *timer_state_;

    std::unique_ptr<RobotService::Stub> stub_; //grpc存根
    std::shared_ptr<Channel> channel_;

    RobotInfoResponse robot_info_response_;
    RobotStateResponse robot_state_response_;

private:
    bool use_raw_data_{false};

    QVector<double> cnt_per_unit_;
    QVector<double> torque_per_unit_;
    QVector<double> load_per_unit_;
    QVector<int32_t> pos_zero_offset_;
    QVector<double> ratio_;
    QVector<QString> user_unit_name_;
    QVector<QString> torque_unit_name_;
    QVector<QString> load_unit_name_;

};

#endif // CONNECTDIALOG_H

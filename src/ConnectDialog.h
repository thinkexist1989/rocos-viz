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

using rocos::RobotModel;
using rocos::LinkMeshFile;
using rocos::LinkMeshPath;

using KDL::Frame;
using KDL::JntArray;
using KDL::Rotation;
using KDL::Vector;

namespace Ui {
    class ConnectDialog;
}

class ConnectDialog : public QDialog {
Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = nullptr);

    ~ConnectDialog();

public slots:

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
    //! \return 当前循环时间[ms]
    inline double
    getCurrCyclicTime() const { return robot_state_response_.robot_state().hw_state().current_cycle_time(); }

    inline int getJointNum() {
        jnt_num_ = robot_state_response_.robot_state().joint_states_size();
        return jnt_num_;
    }

    /////////////////////////////////////////////////////////
    ///////////////     获取关节状态       ///////////////////
    /////////////////////////////////////////////////////////

    /// \brief 获取关节当前状态，上电/下电/错误
    /// \param id 关节ID（From 0）
    /// \return
    QString getJointStatus(int id);

    ///
    /// \param id
    /// \return
    inline QString getJointName(int id) const {
        return QString{robot_state_response_.robot_state().joint_states(id).name().c_str()};
    }

    ///
    /// \param id
    /// \return
    inline double getJointPosition(int id) const {
        return robot_state_response_.robot_state().joint_states(id).position();
    }

    ///
    /// \param id
    /// \return
    inline double getJointVelocity(int id) const {
        return robot_state_response_.robot_state().joint_states(id).velocity();
    }

    ///
    /// \param id
    /// \return
    inline double getJointTorque(int id) const {
        return robot_state_response_.robot_state().joint_states(id).acceleration();
    }

    ///
    /// \param id
    /// \return
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
    inline double getJointRatio(int id) const {
        return robot_info_response_.robot_info().joint_infos().at(id).ratio();
    }

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

    //! 获取Flange空间位姿
    inline Frame getFlangePose() const {
        auto pose = robot_state_response_.robot_state().flange_state().pose();
        auto rot = Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                                        pose.rotation().w());
        auto pos = Vector(pose.position().x(), pose.position().y(), pose.position().z());

        return Frame{rot, pos};
    }

    inline double getFlangeX() const {
        return robot_state_response_.robot_state().flange_state().pose().position().x();
    }
    inline double getFlangeY() const {
        return robot_state_response_.robot_state().flange_state().pose().position().y();
    }
    inline double getFlangeZ() const {
        return robot_state_response_.robot_state().flange_state().pose().position().z();
    }
    inline double getFlangeRX() const {
        double rx, ry, rz;
        auto pose = robot_state_response_.robot_state().flange_state().pose();
        Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                             pose.rotation().w()).GetRPY(rx, ry, rz);
        return rx;
    }

    inline double getFlangeRY() const {
        double rx, ry, rz;
        auto pose = robot_state_response_.robot_state().flange_state().pose();
        Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                             pose.rotation().w()).GetRPY(rx, ry, rz);
        return ry;
    }

    inline double getFlangeRZ() const {
        double rx, ry, rz;
        auto pose = robot_state_response_.robot_state().flange_state().pose();
        Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                             pose.rotation().w()).GetRPY(rx, ry, rz);
        return rz;
    }

    //! 获取Flange空间位姿
    inline Frame getToolPose() const {
         auto pose = robot_state_response_.robot_state().tool_state().pose();
        auto rot = Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                                        pose.rotation().w());
        auto pos = Vector(pose.position().x(), pose.position().y(), pose.position().z());

        return Frame{rot, pos};
    }

    //! 获取Object空间位姿
    inline Frame getObjectPose() const {
         auto pose = robot_state_response_.robot_state().obj_state().pose();
        auto rot = Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                                        pose.rotation().w());
        auto pos = Vector(pose.position().x(), pose.position().y(), pose.position().z());

        return Frame{rot, pos};
    }
    inline Frame getPoseOut() const {
        auto pose = robot_state_response_.robot_state().pose_out().pose();
        auto rot = Rotation::Quaternion(pose.rotation().x(), pose.rotation().y(), pose.rotation().z(),
                                        pose.rotation().w());
        auto pos = Vector(pose.position().x(), pose.position().y(), pose.position().z());

        return Frame{rot, pos};
    }

    //! 获取Base空间位姿(倒置安装或者装载在移动平台上,Base会变化)
    inline Frame getBasePose() const {
        return Frame();
    }

public slots:
    /////////////////////////////////////////////////////////
    ///////////////     关节操作函数       ///////////////////
    /////////////////////////////////////////////////////////

    //!< 所有关节上电
    void powerOn();

    //!< 所有关节下电
    void powerOff();

    //!< 单关节上电
    void powerOn(int id);

    //!< 单关节下电
    void powerOff(int id);

    //!< 等同于powerOn和powerOff操作
    void setRobotEnabled(bool enabled);

    /////////////////////////////////////////////////////////
    ///////////////   机器人控制器连接     ///////////////////
    /////////////////////////////////////////////////////////

    //!< 连接到机器人
    void connectedToRobot(bool con, bool autoLoadModel);

    //!< 断开和机器人连接
    void shutdown();

    void getRobotModel();

//    void setJointMode(int id, int mode);
//    void setSync(int sync);
//    //////////Single Axis Move/////////////////
//    void moveSingleAxis(int id, double pos, double max_vel = -1, double max_acc = -1, double max_jerk = -1,
//                        double least_time = -1);
//    void stopSingleAxis(int id);
//    //////////Multi Axis Move/////////////////
//    void moveMultiAxis(const QVector<double> &pos, const QVector<double> &max_vel, const QVector<double> &max_acc,
//                       const QVector<double> &max_jerk, double least_time = -1);
//    void stopMultiAxis();



    /*! \brief 获取机器人状态
     *
     * 这个函数配合QTimer使用,定时调用次函数获取机器人状态信息,
     * 并在获取成功后,发布newStateComming信号,之后可以调用各种
     * getXXXX()获取相关机器人状态信息.
     *
     */
    void getRobotState();

    //!< 关节速度
    void setJointSpeedScaling(double factor);

    //!< 获取关节速度
    void getJointSpeedScaling();

    //!< 笛卡尔速度
    void setCartesianSpeedScaling(double factor);

    //!< 获取 笛卡尔速度
    void getCartesianSpeedScaling();

    //!< 工具速度
    void setToolSpeedScaling(double factor);

    //!< 获取工具速度
    void getToolSpeedScaling();

    /////////////////////////////////////////////////////////
    ///////////////    机器人脚本控制      ///////////////////
    /////////////////////////////////////////////////////////

    void startScript(QString script);

    void stopScript();

    void pauseScript();

    void continueScript();


    //! 机器人运动学
    void moveJ(QVector<double> q);
    void moveJ_IK(QVector<double> pose);

    void moveL(QVector<double> pose);
    void setpose(int id,QVector<double> pose);
    void calibration();
    void setToolPose();
    void moveL_FK(QVector<double> q);

public:
    //! 机器人是否已经连接
    inline bool isConnected() { return is_connected_; }
//    inline bool getRobotEnabled() { return isRobotEnabled; } //!< TODO: 获取Enable状态

    void jointJogging(int id, int dir); //!< 关节点动
    void cartesianJogging(int frame, int freedom, int dir); //!< 笛卡尔点动
    void jogging(int frame, int freedom, int dir); //!< 两种点动可以合在一起

    void setRobotWorkMode(int mode); //!< 设置机器人工作模式

signals:

    //!< TODO(yang luo): 解析到关节位置，发送 信号, 已废弃, newstatecomming中包含了机器人各种状态信息
    void jointPositions(QVector<double> &jntPos);

    //!< TODO(yang luo): 解析到笛卡尔空间位置，发送 信号, 已废弃, new state comming中包含了机器人各种状态信息
    void cartPose(QVector<double> &pose);

    //!< TODO(yang luo): 速度缩放因数 25.0,已废弃,规划的scaling直接在rocos-viz中完成
    void speedScaling(double f100);

    //!< TODO(yang luo): 返回的日志信息,暂时还未用到
    void logging(QByteArray &ba);

    //!< 机器人状态更新信号, 如果收到了机器人的状态信息就通知主界面更新
    void newStateComming(void);

    //!< 连接状态信号, 通知与机器人rpc连接状态
    void connectState(bool isConnected);

    void showRobot(QString path);

private slots:
    void on_exitButton_clicked();

private:
    bool event(QEvent *event) override; //!< 重写事件相应函数,窗口失去焦点自动关闭

    Ui::ConnectDialog *ui;

//    bool isRobotEnabled = false; //!< 机器人默认不上电

    QString ip_address_{"192.168.0.194"}; //!< IP地址
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

private:
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

};

#endif // CONNECTDIALOG_H

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <cstdint>
#include <memory>

#include <QByteArray>
#include <QDialog>
#include <QJsonObject>
#include <QTimer>
#include <QVector>

#include <Eigen/Dense>

#include <kdl/frames.hpp>
#include <kdl/frames_io.hpp>

#define POSITION_MODE 0
#define EE_ADMIT_TEACH_MODE 1
#define JNT_ADMIT_TEACH_MODE 2
#define JNT_IMP_MODE 3
#define CART_IMP_MODE 4

class HttpRobotClient;

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
    inline double getMinCyclicTime() const { return robot_state_cache_.hardware.cycle_time_min; }

    //! \brief 获取最大循环周期
    //! \return 最大循环时间[ms]
    inline double getMaxCyclicTime() const { return robot_state_cache_.hardware.cycle_time_max; }

    //! \brief 获取当前循环周期
    //! \return 当前循环时间[ms]
    inline double getCurrCyclicTime() const { return robot_state_cache_.hardware.cycle_time_avg; }

    inline int getJointNum() {
        jnt_num_ = robot_state_cache_.joint_states.size() > 0
                   ? robot_state_cache_.joint_states.size()
                   : joint_info_cache_.size();
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
        return isValidJointState(id) ? robot_state_cache_.joint_states[id].name : QString();
    }

    ///
    /// \param id
    /// \return
    inline double getJointPosition(int id) const {
        return isValidJointState(id) ? robot_state_cache_.joint_states[id].position : 0.0;
    }

    ///
    /// \param id
    /// \return
    inline double getJointVelocity(int id) const {
        return isValidJointState(id) ? robot_state_cache_.joint_states[id].velocity : 0.0;
    }

    ///
    /// \param id
    /// \return
    inline double getJointTorque(int id) const {
        return isValidJointState(id) ? robot_state_cache_.joint_states[id].torque : 0.0;
    }

    ///
    /// \param id
    /// \return
    inline double getJointLoad(int id) const {
        return isValidJointState(id) ? robot_state_cache_.joint_states[id].load : 0.0;
    }

    /////////////////////////////////////////////////////////
    ///////////////     获取关节信息       ///////////////////
    /////////////////////////////////////////////////////////

    //! \brief 获取关节用户单位对应脉冲数
    //! \param id 关节ID（From 0）
    //! \return  cnt_per_unit
    inline double getJointCntPerUnit(int id) const {
        return isValidJointInfo(id) ? joint_info_cache_[id].cnt_per_unit : 0.0;
    }

    //!
    //! \param id
    //! \return
    inline double getJointTorquePerUnit(int id) const {
        return isValidJointInfo(id) ? joint_info_cache_[id].torque_per_unit : 0.0;
    }

    //!
    //! \param id
    //! \return
    inline double getJointRatio(int id) const {
        return isValidJointInfo(id) ? joint_info_cache_[id].ratio : 0.0;
    }

    //!
    //! \param id
    //! \return
    inline int32_t getJointPosZeroOffset(int id) const {
        return isValidJointInfo(id) ? joint_info_cache_[id].pos_zero_offset : 0;
    }

    //!
    //! \param id
    //! \return
    inline QString getJointUserUnitName(int id) const {
        return isValidJointInfo(id) ? joint_info_cache_[id].user_unit_name : QString();
    }

    /////////////////////////////////////////////////////////
    ///////////////      运动学函数        ///////////////////
    /////////////////////////////////////////////////////////

    //! 获取Flange空间位姿
    inline Frame getFlangePose() const {
        return poseToFrame(robot_state_cache_.flange_pose);
    }

    inline double getFlangeX() const {
        return robot_state_cache_.flange_pose.x;
    }
    inline double getFlangeY() const {
        return robot_state_cache_.flange_pose.y;
    }
    inline double getFlangeZ() const {
        return robot_state_cache_.flange_pose.z;
    }
    inline double getFlangeRX() const {
        double rx, ry, rz;
        poseToFrame(robot_state_cache_.flange_pose).M.GetRPY(rx, ry, rz);
        return rx;
    }

    inline double getFlangeRY() const {
        double rx, ry, rz;
        poseToFrame(robot_state_cache_.flange_pose).M.GetRPY(rx, ry, rz);
        return ry;
    }

    inline double getFlangeRZ() const {
        double rx, ry, rz;
        poseToFrame(robot_state_cache_.flange_pose).M.GetRPY(rx, ry, rz);
        return rz;
    }

    //! 获取Flange空间位姿
    inline Frame getToolPose() const {
        return poseToFrame(robot_state_cache_.tool_pose);
    }

    //! 获取Object空间位姿
    inline Frame getObjectPose() const {
        return poseToFrame(robot_state_cache_.object_pose);
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
    struct JointInfoCache {
        QString name;
        double cnt_per_unit = 0.0;
        double torque_per_unit = 0.0;
        double ratio = 0.0;
        int pos_zero_offset = 0;
        QString user_unit_name;
    };

    struct JointStateCache {
        QString name;
        double position = 0.0;
        double velocity = 0.0;
        double acceleration = 0.0;
        double torque = 0.0;
        double load = 0.0;
        QString status;
    };

    struct PoseCache {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double qx = 0.0;
        double qy = 0.0;
        double qz = 0.0;
        double qw = 1.0;
    };

    struct HardwareCache {
        QString type;
        double cycle_time_min = 0.0;
        double cycle_time_avg = 0.0;
        double cycle_time_max = 0.0;
        int slave_count = 0;
    };

    struct RobotStateCache {
        QVector<JointStateCache> joint_states;
        PoseCache flange_pose;
        PoseCache tool_pose;
        PoseCache object_pose;
        HardwareCache hardware;
    };

    struct ModelLinkDownload {
        QJsonObject link;
        QString mesh_path;
        QString saved_mesh_name;
        QString temp_file_path;
    };

    struct ModelDownloadContext {
        QString cfg_file_path;
        QString robot_dir_path;
        QVector<ModelLinkDownload> links;
        int pending = 0;
        bool failed = false;
        QString error;
    };

    struct ModelWriteResult {
        ModelWriteResult(bool success, const QString &message) : ok(success), error(message) {}
        bool ok;
        QString error;
    };

    bool event(QEvent *event) override; //!< 重写事件相应函数,窗口失去焦点自动关闭
    bool postEmpty(const QString &path);
    bool postJsonCommand(const QString &path, const QJsonObject &body);
    void handleCommandResponse(const QString &path, const QJsonObject &data);
    bool parseMoveResponse(const QJsonObject &data, QString *error);
    void queryMoveStatus();
    bool parseTaskStatus(const QJsonObject &data, QString *error);
    bool parseRobotInfo(const QJsonObject &data, QString *error);
    bool parseRobotState(const QJsonObject &data, QString *error);
    bool isValidJointInfo(int id) const;
    bool isValidJointState(int id) const;
    static Frame poseToFrame(const PoseCache &pose);
    static ModelWriteResult writeModelFiles(const ModelDownloadContext &context);

    Ui::ConnectDialog *ui;

//    bool isRobotEnabled = false; //!< 机器人默认不上电

    QString ip_address_{"192.168.0.194"}; //!< IP地址
    int port_{30001}; //!< IP端口号
    bool is_connected_{false}; //!<　是否连接标志位

    QTimer *timer_state_; //!<　定时器，用于定期获取机器人状态信息
    QTimer *timer_move_status_; //!< 定时查询异步运动任务状态
    std::unique_ptr<HttpRobotClient> http_client_;

    QVector<JointInfoCache> joint_info_cache_;
    RobotStateCache robot_state_cache_;

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
    bool is_polling_state_{false};
    bool is_loading_model_{false};
    bool is_polling_move_status_{false};
    int state_failure_count_{0};
    QString last_move_task_id_;
    QString last_move_status_;
    QString last_move_message_;
    int last_move_result_{0};
};

#endif // CONNECTDIALOG_H

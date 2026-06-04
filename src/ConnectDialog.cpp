#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

#include "HttpRobotClient.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QMetaObject>
#include <QPointer>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QUrlQuery>

const int POLLING_INTERVAL_MS = 20;

namespace {
const int FRAME_TOOL = 100;
const int FRAME_FLANGE = 200;
const int FRAME_OBJECT = 300;
const int FRAME_BASE = 400;
const int MAX_STATE_FAILURES = 10;
const int MOVE_STATUS_INTERVAL_MS = 200;

double numberValue(const QJsonObject &object, const QString &key, double fallback = 0.0) {
    return object.value(key).toDouble(fallback);
}

int intValue(const QJsonObject &object, const QString &key, int fallback = 0) {
    return object.value(key).toInt(fallback);
}

QString stringValue(const QJsonObject &object, const QString &key, const QString &fallback = QString()) {
    return object.value(key).toString(fallback);
}

QString statusName(const QJsonValue &value) {
    if (value.isUndefined() || value.isNull()) {
        return "UNKNOWN";
    }
    if (value.isString()) {
        return value.toString();
    }

    switch (value.toInt(-1)) {
        case 0:
            return "DISABLED";
        case 1:
            return "FAULT";
        case 2:
            return "ENABLED";
        default:
            return QString("STATUS_%1").arg(value.toInt(-1));
    }
}

QString hardwareTypeName(const QJsonValue &value) {
    if (value.isString()) {
        return value.toString();
    }

    switch (value.toInt(0)) {
        case 1:
            return "SIMULATION";
        case 2:
            return "ETHERCAT";
        default:
            return "UNKNOWN";
    }
}

QJsonArray doubleArray(const QVector<double> &values, int count = -1) {
    QJsonArray array;
    const int size = static_cast<int>(values.size());
    const int n = count < 0 ? size : std::min(count, size);
    for (int i = 0; i < n; ++i) {
        array.append(values[i]);
    }
    return array;
}

QJsonObject vector3Object(double x, double y, double z) {
    QJsonObject object;
    object["x"] = x;
    object["y"] = y;
    object["z"] = z;
    return object;
}

QJsonObject poseObjectFromRpy(const QVector<double> &pose) {
    QJsonObject pose_object;
    if (pose.size() < 6) {
        pose_object["position"] = vector3Object(0.0, 0.0, 0.0);
        QJsonObject orientation;
        orientation["x"] = 0.0;
        orientation["y"] = 0.0;
        orientation["z"] = 0.0;
        orientation["w"] = 1.0;
        pose_object["orientation"] = orientation;
        return pose_object;
    }

    pose_object["position"] = vector3Object(pose[0], pose[1], pose[2]);

    auto rotation = Rotation::RPY(pose[3], pose[4], pose[5]);
    double x, y, z, w;
    rotation.GetQuaternion(x, y, z, w);

    QJsonObject orientation;
    orientation["x"] = x;
    orientation["y"] = y;
    orientation["z"] = z;
    orientation["w"] = w;
    pose_object["orientation"] = orientation;

    return pose_object;
}

QString directionName(int dir) {
    if (dir > 0) {
        return "POSITIVE";
    }
    if (dir < 0) {
        return "NEGATIVE";
    }
    return "NONE";
}

QString jointFlagName(int id) {
    if (id < 0) {
        id = 0;
    }
    if (id > 6) {
        id = 6;
    }
    return QString("J%1").arg(id);
}

QString cartesianFlagName(int frame) {
    QString prefix = "BASE";
    int freedom = frame;

    if (frame >= FRAME_BASE) {
        prefix = "BASE";
        freedom = frame - FRAME_BASE;
    } else if (frame >= FRAME_OBJECT) {
        prefix = "OBJECT";
        freedom = frame - FRAME_OBJECT;
    } else if (frame >= FRAME_FLANGE) {
        prefix = "FLANGE";
        freedom = frame - FRAME_FLANGE;
    } else if (frame >= FRAME_TOOL) {
        prefix = "TOOL";
        freedom = frame - FRAME_TOOL;
    }

    static const char *names[] = {"X", "Y", "Z", "ROLL", "PITCH", "YAW"};
    if (freedom < 0 || freedom > 5) {
        freedom = 0;
    }
    return QString("%1_%2").arg(prefix, QString(names[freedom]));
}

QString workModeName(int mode) {
    switch (mode) {
        case POSITION_MODE:
            return "position";
        case EE_ADMIT_TEACH_MODE:
            return "ee_admit_teach";
        case JNT_ADMIT_TEACH_MODE:
            return "jnt_admit_teach";
        case JNT_IMP_MODE:
            return "jnt_imp";
        case CART_IMP_MODE:
            return "cart_imp";
        default:
            return "position";
    }
}

QJsonObject nestedObject(const QJsonObject &object, const QStringList &keys) {
    for (const auto &key : keys) {
        const auto value = object.value(key);
        if (value.isObject()) {
            return value.toObject();
        }
    }
    return QJsonObject();
}

QString linkTypeName(const QJsonValue &value) {
    if (value.isString()) {
        return value.toString().toLower();
    }

    switch (value.toInt(0)) {
        case 1:
            return "fixed";
        case 2:
            return "revolute";
        case 3:
            return "prismatic";
        case 4:
            return "continuous";
        default:
            return "unknown";
    }
}

void writeVector(QTextStream &stream, const QString &yaml_key, const QJsonObject &object) {
    stream << "    " << yaml_key << ": ["
           << numberValue(object, "x") << ", "
           << numberValue(object, "y") << ", "
           << numberValue(object, "z") << "]\n";
}

bool isMoveCommandPath(const QString &path) {
    return path == "/api/move/joint" || path == "/api/move/linear"
           || path == "/api/move/joint_ik" || path == "/api/move/linear_fk";
}

bool isTerminalMoveStatus(const QString &status) {
    const QString normalized = status.toUpper();
    return normalized == "COMPLETED" || normalized == "FAILED" || normalized == "STOPPED";
}

QString contentDispositionFileName(const QList<QNetworkReply::RawHeaderPair> &headers) {
    QString header;
    for (const auto &pair : headers) {
        if (QString::fromLatin1(pair.first).compare("Content-Disposition", Qt::CaseInsensitive) == 0) {
            header = QString::fromUtf8(pair.second);
            break;
        }
    }
    if (header.isEmpty()) {
        return QString();
    }

    const QStringList parts = header.split(';');
    for (const QString &part : parts) {
        const QString trimmed = part.trimmed();
        if (trimmed.startsWith("filename*=")) {
            const QString value = trimmed.mid(QString("filename*=").size());
            const int marker = value.indexOf("''");
            return QUrl::fromPercentEncoding(value.mid(marker >= 0 ? marker + 2 : 0).toUtf8());
        }
        if (trimmed.startsWith("filename=")) {
            QString value = trimmed.mid(QString("filename=").size()).trimmed();
            if (value.startsWith('"') && value.endsWith('"') && value.size() >= 2) {
                value = value.mid(1, value.size() - 2);
            }
            return value;
        }
    }
    return QString();
}

QString safeFileName(const QString &name, const QString &fallback) {
    const QString candidate = QFileInfo(name).fileName();
    if (!candidate.isEmpty() && candidate != "." && candidate != "..") {
        return candidate;
    }
    return QFileInfo(fallback).fileName();
}
}

ConnectDialog::ConnectDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConnectDialog) {
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlag(Qt::FramelessWindowHint);

    ui->ipAddressEdit->setText(ip_address_);
    ui->portEdit->setText(QString::number(port_));

    timer_state_ = new QTimer(this);
    connect(timer_state_, &QTimer::timeout, this, &ConnectDialog::getRobotState);

    timer_move_status_ = new QTimer(this);
    connect(timer_move_status_, &QTimer::timeout, this, &ConnectDialog::queryMoveStatus);
}

ConnectDialog::~ConnectDialog() {
    if (timer_state_) {
        timer_state_->stop();
    }
    if (timer_move_status_) {
        timer_move_status_->stop();
    }
    delete ui;
}

void ConnectDialog::on_connectButton_clicked() {
    bool isAutoLoad = ui->autoLoadCheck->isChecked();
    connectedToRobot(true, isAutoLoad);
}

void ConnectDialog::on_ipAddressEdit_textChanged(const QString &ip) {
    ip_address_ = ip;
}

void ConnectDialog::on_portEdit_textChanged(const QString &p) {
    port_ = p.toInt();
}

void ConnectDialog::connectedToRobot(bool con, bool autoLoadModel) {
    if (!con) {
        shutdown();
        return;
    }

    if (is_connected_ || http_client_) {
        return;
    }

    http_client_.reset(new HttpRobotClient(ip_address_, port_, this));
    http_client_->setTimeouts(1000, 1000, 500);
    http_client_->getJson("/api/robot/info", [this, autoLoadModel](bool ok, const QJsonObject &data, const QString &error) {
        if (!http_client_) {
            return;
        }

        QString parse_error;
        if (!ok || !parseRobotInfo(data, &parse_error)) {
            qWarning() << "Failed to connect to robot:" << (ok ? parse_error : error);
            http_client_.reset();
            is_connected_ = false;
            emit connectState(false);
            return;
        }

        is_connected_ = true;
        state_failure_count_ = 0;
        emit connectState(true);
        timer_state_->start(POLLING_INTERVAL_MS);
        close();

        if (autoLoadModel) {
            getRobotModel();
        }
    });
}

void ConnectDialog::setRobotEnabled(bool enabled) {
    if (enabled) {
        powerOn();
    } else {
        powerOff();
    }
}

void ConnectDialog::setJointSpeedScaling(double factor) {
    std::cout << "Change joint speed scale to " << factor << std::endl;
    factor_ = factor;
}

void ConnectDialog::getJointSpeedScaling() {

}

void ConnectDialog::setCartesianSpeedScaling(double factor) {
    factor_ = factor;
}

void ConnectDialog::getCartesianSpeedScaling() {

}

void ConnectDialog::setToolSpeedScaling(double factor) {
    factor_ = factor;
}

void ConnectDialog::getToolSpeedScaling() {

}

void ConnectDialog::startScript(QString script) {
    Q_UNUSED(script);
}

void ConnectDialog::stopScript() {

}

void ConnectDialog::pauseScript() {

}

void ConnectDialog::continueScript() {

}

void ConnectDialog::getRobotState() {
    if (!is_connected_ || !http_client_ || is_polling_state_) {
        return;
    }

    is_polling_state_ = true;
    http_client_->getJson("/api/robot/state", [this](bool ok, const QJsonObject &data, const QString &error) {
        is_polling_state_ = false;
        if (!is_connected_ || !http_client_) {
            return;
        }

        QString parse_error;
        if (ok && parseRobotState(data, &parse_error)) {
            state_failure_count_ = 0;
            emit newStateComming();
            return;
        }

        ++state_failure_count_;
        qWarning() << "Failed to read robot state:" << (ok ? parse_error : error);
        if (state_failure_count_ >= MAX_STATE_FAILURES) {
            shutdown();
        }
    });
}

void ConnectDialog::jointJogging(int id, int dir) {
    if (!is_connected_) {
        return;
    }

    if (dir == 0) {
        postEmpty("/api/drag/stop");
        return;
    }

    QJsonObject body;
    body["flag"] = jointFlagName(id);
    body["direction"] = directionName(dir);
    body["max_speed"] = max_jnt_speed_ * factor_;
    body["max_acceleration"] = max_jnt_speed_ * factor_ * 5;
    postJsonCommand("/api/drag/start", body);
}

void ConnectDialog::cartesianJogging(int frame, int freedom, int dir) {
    Q_UNUSED(freedom);
    if (!is_connected_) {
        return;
    }

    if (dir == 0) {
        postEmpty("/api/drag/stop");
        return;
    }

    QJsonObject body;
    body["flag"] = cartesianFlagName(frame);
    body["direction"] = directionName(dir);
    body["max_speed"] = max_cart_speed_ * factor_;
    body["max_acceleration"] = max_cart_speed_ * factor_ * 5;
    postJsonCommand("/api/drag/start", body);
}

void ConnectDialog::jogging(int frame, int freedom, int dir) {
    if (frame == 0) {
        jointJogging(freedom, dir);
    } else {
        cartesianJogging(frame, freedom, dir);
    }
}

void ConnectDialog::setRobotWorkMode(int mode) {
    QJsonObject body;
    body["mode"] = workModeName(mode);
    postJsonCommand("/api/robot/workmode", body);
}

bool ConnectDialog::event(QEvent *event) {
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            close();
        }
    }

    return QDialog::event(event);
}

void ConnectDialog::powerOn() {
    postEmpty("/api/robot/enable");
}

void ConnectDialog::powerOff() {
    postEmpty("/api/robot/disable");
}

void ConnectDialog::shutdown() {
    if (timer_state_) {
        timer_state_->stop();
    }
    if (timer_move_status_) {
        timer_move_status_->stop();
    }
    is_polling_state_ = false;
    is_loading_model_ = false;
    is_polling_move_status_ = false;
    state_failure_count_ = 0;
    last_move_task_id_.clear();
    last_move_status_.clear();
    last_move_message_.clear();
    last_move_result_ = 0;
    http_client_.reset();
    is_connected_ = false;
    emit connectState(false);
}

void ConnectDialog::getRobotModel() {
    if (!is_connected_ || !http_client_ || is_loading_model_) {
        return;
    }

    is_loading_model_ = true;
    http_client_->getJson("/api/robot/model", [this](bool ok, const QJsonObject &data, const QString &error) {
        if (!is_connected_ || !http_client_) {
            return;
        }
        if (!ok) {
            is_loading_model_ = false;
            qWarning() << "Fail to get robot model:" << error;
            return;
        }

        const QString robot_name = data.value("name").toString("robot");
        const QJsonArray links_json = data.value("links").toArray();
        if (links_json.isEmpty()) {
            is_loading_model_ = false;
            qWarning() << "Robot model has no links";
            return;
        }

        QDir model_root("models");
        if (!model_root.exists() && !model_root.mkpath(".")) {
            is_loading_model_ = false;
            qWarning() << "Can not create models directory";
            return;
        }
        if (!model_root.mkpath(robot_name)) {
            is_loading_model_ = false;
            qWarning() << "Can not create robot model directory:" << robot_name;
            return;
        }

        auto context = std::make_shared<ModelDownloadContext>();
        context->robot_dir_path = model_root.filePath(robot_name);
        context->cfg_file_path = QDir(context->robot_dir_path).filePath("config.yaml");

        for (const QJsonValue &link_value : links_json) {
            ModelLinkDownload link;
            link.link = link_value.toObject();
            link.mesh_path = link.link.value("mesh").toString();
            link.saved_mesh_name = QFileInfo(link.mesh_path).fileName();
            context->links.push_back(link);
        }

        auto finalize = [this, context]() {
            if (context->pending > 0) {
                return;
            }

            is_loading_model_ = false;
            if (context->failed) {
                for (const ModelLinkDownload &download : context->links) {
                    if (!download.temp_file_path.isEmpty()) {
                        QFile::remove(download.temp_file_path);
                    }
                }
                qWarning() << "Robot model download failed:" << context->error;
                return;
            }

            QPointer<ConnectDialog> self(this);
            std::thread([self, context]() {
                const ModelWriteResult result = ConnectDialog::writeModelFiles(*context);
                if (!self) {
                    return;
                }
                QMetaObject::invokeMethod(self.data(), [self, context, result]() {
                    if (!self) {
                        return;
                    }
                    if (!result.ok) {
                        qWarning() << "Robot model write failed:" << result.error;
                        return;
                    }
                    emit self->showRobot(context->cfg_file_path);
                }, Qt::QueuedConnection);
            }).detach();
        };

        for (int i = 0; i < context->links.size(); ++i) {
            if (context->links[i].mesh_path.isEmpty()) {
                continue;
            }

            ++context->pending;
            QUrlQuery query;
            query.addQueryItem("path", context->links[i].mesh_path);
            const QString mesh_url = QString("/api/robot/model/mesh?") + query.toString(QUrl::FullyEncoded);

            http_client_->downloadFile(mesh_url, [this, context, finalize, i](bool mesh_ok,
                                                                              const QString &temp_file_path,
                                                                              const QList<QNetworkReply::RawHeaderPair> &headers,
                                                                              const QString &mesh_error) {
                --context->pending;
                if (!mesh_ok || temp_file_path.isEmpty()) {
                    context->failed = true;
                    context->error = mesh_error.isEmpty() ? QString("empty mesh file") : mesh_error;
                    finalize();
                    return;
                }

                context->links[i].saved_mesh_name = safeFileName(contentDispositionFileName(headers),
                                                                 context->links[i].mesh_path);
                context->links[i].temp_file_path = temp_file_path;
                finalize();
            });
        }

        finalize();
    });
}

ConnectDialog::ModelWriteResult ConnectDialog::writeModelFiles(const ModelDownloadContext &context) {
    QDir robot_dir(context.robot_dir_path);
    for (const ModelLinkDownload &download : context.links) {
        if (download.saved_mesh_name.isEmpty()) {
            continue;
        }

        const QString target_path = robot_dir.filePath(download.saved_mesh_name);
        QFile::remove(target_path);
        if (!QFile::rename(download.temp_file_path, target_path)) {
            if (!QFile::copy(download.temp_file_path, target_path)) {
                QFile::remove(download.temp_file_path);
                return {false, QString("Can not save mesh file: %1").arg(target_path)};
            }
            QFile::remove(download.temp_file_path);
        }

        const QFileInfo saved_mesh(target_path);
        if (!saved_mesh.exists() || saved_mesh.size() <= 0) {
            return {false, QString("Saved mesh file is empty: %1").arg(target_path)};
        }
    }

    QFile cfg_file(context.cfg_file_path);
    if (!cfg_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return {false, QString("Can not open %1").arg(context.cfg_file_path)};
    }

    QTextStream cfg_stream(&cfg_file);
    cfg_stream << "robot:\n";
    for (const ModelLinkDownload &download : context.links) {
        const QJsonObject &link = download.link;
        cfg_stream << "  - name: " << stringValue(link, "name", "link") << "\n";
        cfg_stream << "    order: " << intValue(link, "order") << "\n";
        cfg_stream << "    type: " << linkTypeName(link.value("type")) << "\n";
        writeVector(cfg_stream, "translate", nestedObject(link, {"translate", "translation"}));
        writeVector(cfg_stream, "rotate", nestedObject(link, {"rotate", "rotation"}));
        writeVector(cfg_stream, "angleAxis", nestedObject(link, {"axis", "angleAxis", "angle_axis"}));
        writeVector(cfg_stream, "translateLink", nestedObject(link, {"translateLink", "translatelink", "translate_link"}));
        writeVector(cfg_stream, "rotateLink", nestedObject(link, {"rotateLink", "rotatelink", "rotate_link"}));
        if (!download.saved_mesh_name.isEmpty()) {
            cfg_stream << "    mesh: " << download.saved_mesh_name << "\n";
        }
    }
    cfg_file.close();

    return {true, QString()};
}

void ConnectDialog::powerOn(int id) {
    QJsonObject body;
    body["id"] = id;
    postJsonCommand("/api/axis/single/enable", body);
}

void ConnectDialog::powerOff(int id) {
    QJsonObject body;
    body["id"] = id;
    postJsonCommand("/api/axis/single/disable", body);
}

QString ConnectDialog::getHardwareType() {
    const QString type = robot_state_cache_.hardware.type.toUpper();
    if (type == "SIM" || type == "SIMULATION") {
        return tr("Simulation");
    }
    if (type == "ETHERCAT") {
        return tr("EtherCAT");
    }
    if (type.isEmpty() || type == "UNKNOWN") {
        return tr("Unknown");
    }
    return robot_state_cache_.hardware.type;
}

QString ConnectDialog::getJointStatus(int id) {
    if (!isValidJointState(id)) {
        return QString();
    }

    const QString status = robot_state_cache_.joint_states[id].status.toUpper();
    if (status == "FAULT") {
        return tr("Fault");
    }
    if (status == "DISABLED") {
        return tr("Disabled");
    }
    if (status == "ENABLED") {
        return tr("Enabled");
    }
    return robot_state_cache_.joint_states[id].status;
}

void ConnectDialog::moveJ(QVector<double> q) {
    QJsonObject body;
    const int size = static_cast<int>(q.size());
    const int count = jnt_num_ > 0 ? std::min(jnt_num_, size) : size;
    body["joints"] = doubleArray(q, count);
    body["speed"] = max_jnt_speed_ * factor_;
    body["acceleration"] = max_jnt_speed_ * factor_;
    body["time"] = 0.0;
    body["radius"] = 0.0;
    body["asynchronous"] = true;
    postJsonCommand("/api/move/joint", body);
}

void ConnectDialog::moveL(QVector<double> pose) {
    QJsonObject body;
    body["pose"] = poseObjectFromRpy(pose);
    body["speed"] = max_cart_speed_ * factor_;
    body["acceleration"] = max_cart_speed_ * factor_;
    body["time"] = 0.0;
    body["radius"] = 0.0;
    body["asynchronous"] = true;
    postJsonCommand("/api/move/linear", body);
}

void ConnectDialog::moveJ_IK(QVector<double> pose) {
    QJsonObject body;
    body["pose"] = poseObjectFromRpy(pose);
    body["speed"] = max_jnt_speed_ * factor_;
    body["acceleration"] = max_jnt_speed_ * factor_;
    body["time"] = 0.0;
    body["radius"] = 0.0;
    body["asynchronous"] = true;
    postJsonCommand("/api/move/joint_ik", body);
}

void ConnectDialog::moveL_FK(QVector<double> q) {
    QJsonObject body;
    const int size = static_cast<int>(q.size());
    const int count = jnt_num_ > 0 ? std::min(jnt_num_, size) : size;
    body["joints"] = doubleArray(q, count);
    body["speed"] = max_cart_speed_ * factor_;
    body["acceleration"] = max_cart_speed_ * factor_;
    body["time"] = 0.0;
    body["radius"] = 0.0;
    body["asynchronous"] = true;
    postJsonCommand("/api/move/linear_fk", body);
}

void ConnectDialog::on_exitButton_clicked() {
    close();
}

bool ConnectDialog::postEmpty(const QString &path) {
    return postJsonCommand(path, QJsonObject());
}

bool ConnectDialog::postJsonCommand(const QString &path, const QJsonObject &body) {
    if (!is_connected_ || !http_client_) {
        return false;
    }

    http_client_->postJson(path, body, [this, path](bool ok, const QJsonObject &data, const QString &error) {
        if (!is_connected_ || !http_client_) {
            return;
        }
        if (!ok) {
            qWarning() << "HTTP command failed" << path << error;
            return;
        }
        handleCommandResponse(path, data);
    });
    return true;
}

void ConnectDialog::handleCommandResponse(const QString &path, const QJsonObject &data) {
    if (!isMoveCommandPath(path)) {
        return;
    }

    QString error;
    if (!parseMoveResponse(data, &error)) {
        qWarning() << "Invalid move response" << path << error;
    }
}

bool ConnectDialog::parseMoveResponse(const QJsonObject &data, QString *error) {
    if (data.value("task_id").isString()) {
        last_move_task_id_ = data.value("task_id").toString();
        last_move_status_ = data.value("status").toString();
        last_move_message_ = data.value("message").toString();
        last_move_result_ = 0;

        if (last_move_task_id_.isEmpty()) {
            if (error) {
                *error = "MoveResponse task_id is empty";
            }
            return false;
        }

        if (timer_move_status_ && !isTerminalMoveStatus(last_move_status_)) {
            timer_move_status_->start(MOVE_STATUS_INTERVAL_MS);
        }
        return true;
    }

    if (data.value("result").isDouble()) {
        last_move_task_id_.clear();
        last_move_status_.clear();
        last_move_result_ = data.value("result").toInt();
        last_move_message_ = data.value("message").toString();
        if (timer_move_status_) {
            timer_move_status_->stop();
        }
        if (last_move_result_ != 0) {
            qWarning() << "Move command failed result=" << last_move_result_ << last_move_message_;
        }
        return true;
    }

    if (error) {
        *error = "MoveResponse data missing task_id or result";
    }
    return false;
}

void ConnectDialog::queryMoveStatus() {
    if (!is_connected_ || !http_client_ || is_polling_move_status_ || last_move_task_id_.isEmpty()) {
        return;
    }

    QUrlQuery query;
    query.addQueryItem("task_id", last_move_task_id_);
    const QString path = QString("/api/move/status?") + query.toString(QUrl::FullyEncoded);

    is_polling_move_status_ = true;
    http_client_->getJson(path, [this](bool ok, const QJsonObject &data, const QString &error) {
        is_polling_move_status_ = false;
        if (!is_connected_ || !http_client_) {
            return;
        }
        if (!ok) {
            qWarning() << "Failed to query move status:" << error;
            return;
        }

        QString parse_error;
        if (!parseTaskStatus(data, &parse_error)) {
            qWarning() << "Invalid move status response:" << parse_error;
            return;
        }

        if (isTerminalMoveStatus(last_move_status_) && timer_move_status_) {
            timer_move_status_->stop();
        }
    });
}

bool ConnectDialog::parseTaskStatus(const QJsonObject &data, QString *error) {
    if (!data.value("task_id").isString() || data.value("task_id").toString().isEmpty()) {
        if (error) {
            *error = "TaskStatusData missing task_id";
        }
        return false;
    }
    if (!data.value("status").isString()) {
        if (error) {
            *error = "TaskStatusData missing status";
        }
        return false;
    }

    last_move_task_id_ = data.value("task_id").toString();
    last_move_status_ = data.value("status").toString();
    last_move_message_ = data.value("message").toString();
    last_move_result_ = data.value("result").toInt(last_move_result_);

    if (last_move_status_.toUpper() == "FAILED" || last_move_result_ != 0) {
        qWarning() << "Move task failed" << last_move_task_id_
                   << "status=" << last_move_status_
                   << "result=" << last_move_result_
                   << "message=" << last_move_message_;
    }
    return true;
}

bool ConnectDialog::parseRobotInfo(const QJsonObject &data, QString *error) {
    QJsonObject info = data;
    if (data.value("robot_info").isObject()) {
        info = data.value("robot_info").toObject();
    }

    const QJsonValue joint_infos_value = info.value("joint_infos");
    if (!joint_infos_value.isArray()) {
        if (error) {
            *error = "Robot info missing joint_infos array";
        }
        return false;
    }

    const QJsonArray joint_infos = joint_infos_value.toArray();
    if (joint_infos.isEmpty()) {
        if (error) {
            *error = "Robot info joint_infos is empty";
        }
        return false;
    }

    QVector<JointInfoCache> next;
    next.reserve(joint_infos.size());

    for (const QJsonValue &joint_value : joint_infos) {
        if (!joint_value.isObject()) {
            if (error) {
                *error = "Robot info joint entry is not an object";
            }
            return false;
        }
        const QJsonObject joint = joint_value.toObject();
        if (!joint.value("name").isString()) {
            if (error) {
                *error = "Robot info joint missing name";
            }
            return false;
        }
        JointInfoCache cache;
        cache.name = stringValue(joint, "name");
        cache.cnt_per_unit = numberValue(joint, "cnt_per_unit");
        cache.torque_per_unit = numberValue(joint, "torque_per_unit");
        cache.ratio = numberValue(joint, "ratio");
        cache.pos_zero_offset = intValue(joint, "pos_zero_offset", intValue(joint, "zero_offset"));
        cache.user_unit_name = stringValue(joint, "user_unit_name", stringValue(joint, "unit_name"));
        next.push_back(cache);
    }

    joint_info_cache_ = next;
    jnt_num_ = joint_info_cache_.size();
    return true;
}

bool ConnectDialog::parseRobotState(const QJsonObject &data, QString *error) {
    RobotStateCache next;

    const QJsonValue joint_states_value = data.value("joint_states");
    if (!joint_states_value.isArray()) {
        if (error) {
            *error = "Robot state missing joint_states array";
        }
        return false;
    }

    const QJsonArray joint_states = joint_states_value.toArray();
    if (joint_states.isEmpty()) {
        if (error) {
            *error = "Robot state joint_states is empty";
        }
        return false;
    }

    next.joint_states.reserve(joint_states.size());
    for (const QJsonValue &joint_value : joint_states) {
        if (!joint_value.isObject()) {
            if (error) {
                *error = "Robot state joint entry is not an object";
            }
            return false;
        }
        const QJsonObject joint = joint_value.toObject();
        if (!joint.value("name").isString() || !joint.value("position").isDouble()) {
            if (error) {
                *error = "Robot state joint missing name/position";
            }
            return false;
        }
        JointStateCache cache;
        cache.name = stringValue(joint, "name");
        cache.position = numberValue(joint, "position");
        cache.velocity = numberValue(joint, "velocity");
        cache.acceleration = numberValue(joint, "acceleration");
        cache.torque = numberValue(joint, "torque");
        cache.load = numberValue(joint, "load", numberValue(joint, "load_torque"));
        cache.status = statusName(joint.value("status"));
        next.joint_states.push_back(cache);
    }

    auto parsePose = [error](const QJsonValue &pose_value, const QString &name, PoseCache *pose) {
        if (!pose_value.isObject()) {
            if (error) {
                *error = QString("Robot state missing %1 object").arg(name);
            }
            return false;
        }
        const QJsonObject pose_object = pose_value.toObject();
        if (!pose_object.value("position").isObject() || !pose_object.value("orientation").isObject()) {
            if (error) {
                *error = QString("Robot state %1 missing position/orientation").arg(name);
            }
            return false;
        }
        const QJsonObject position = pose_object.value("position").toObject();
        const QJsonObject orientation = pose_object.value("orientation").toObject();
        if (!position.value("x").isDouble() || !position.value("y").isDouble()
            || !position.value("z").isDouble()) {
            if (error) {
                *error = QString("Robot state %1 position missing x/y/z").arg(name);
            }
            return false;
        }
        if (!orientation.value("x").isDouble() || !orientation.value("y").isDouble()
            || !orientation.value("z").isDouble() || !orientation.value("w").isDouble()) {
            if (error) {
                *error = QString("Robot state %1 orientation missing x/y/z/w").arg(name);
            }
            return false;
        }
        pose->x = numberValue(position, "x");
        pose->y = numberValue(position, "y");
        pose->z = numberValue(position, "z");
        pose->qx = numberValue(orientation, "x");
        pose->qy = numberValue(orientation, "y");
        pose->qz = numberValue(orientation, "z");
        pose->qw = numberValue(orientation, "w", 1.0);
        return true;
    };

    const QJsonValue flange_value = data.value("flange").isObject() ? data.value("flange") : data.value("flange_pose");
    const QJsonValue tool_value = data.value("tool").isObject() ? data.value("tool") : data.value("tool_pose");
    const QJsonValue object_value = data.value("object").isObject() ? data.value("object") : data.value("object_pose");
    if (!parsePose(flange_value, "flange", &next.flange_pose)
        || !parsePose(tool_value, "tool", &next.tool_pose)
        || !parsePose(object_value, "object", &next.object_pose)) {
        return false;
    }

    const QJsonValue hardware_value = data.value("hw_state").isObject() ? data.value("hw_state") : data.value("hardware");
    if (!hardware_value.isObject()) {
        if (error) {
            *error = "Robot state missing hardware object";
        }
        return false;
    }

    const QJsonObject hardware = hardware_value.toObject();
    if (!hardware.value("type").isString() && !hardware.value("hw_type").isDouble()) {
        if (error) {
            *error = "Robot state hardware missing type";
        }
        return false;
    }
    next.hardware.type = hardware.value("type").isString()
                         ? stringValue(hardware, "type")
                         : hardwareTypeName(hardware.value("hw_type"));
    next.hardware.cycle_time_avg = numberValue(hardware, "cycle_time_avg",
                                               numberValue(hardware, "current_cycle_time"));
    next.hardware.cycle_time_min = numberValue(hardware, "cycle_time_min",
                                               numberValue(hardware, "min_cycle_time"));
    next.hardware.cycle_time_max = numberValue(hardware, "cycle_time_max",
                                               numberValue(hardware, "max_cycle_time"));
    next.hardware.slave_count = intValue(hardware, "slave_count", intValue(hardware, "slave_num"));

    robot_state_cache_ = next;
    jnt_num_ = robot_state_cache_.joint_states.size();
    return true;
}

bool ConnectDialog::isValidJointInfo(int id) const {
    return id >= 0 && id < joint_info_cache_.size();
}

bool ConnectDialog::isValidJointState(int id) const {
    return id >= 0 && id < robot_state_cache_.joint_states.size();
}

Frame ConnectDialog::poseToFrame(const PoseCache &pose) {
    auto rot = Rotation::Quaternion(pose.qx, pose.qy, pose.qz, pose.qw);
    auto pos = Vector(pose.x, pose.y, pose.z);
    return Frame{rot, pos};
}

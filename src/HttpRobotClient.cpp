#include "HttpRobotClient.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QDir>
#include <QFile>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <QTimer>

HttpRobotClient::HttpRobotClient(const QString &host, int port, QObject *parent)
        : QObject(parent),
          base_url_(QString("http://%1:%2").arg(host).arg(port)) {
}

HttpRobotClient::~HttpRobotClient() = default;

void HttpRobotClient::setTimeouts(int connect_ms, int read_ms, int write_ms) {
    connect_timeout_ms_ = connect_ms;
    read_timeout_ms_ = read_ms;
    write_timeout_ms_ = write_ms;
}

void HttpRobotClient::getJson(const QString &path, JsonCallback callback) {
    QNetworkReply *reply = manager_.get(makeRequest(path));
    attachTimeout(reply, read_timeout_ms_);
    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        handleJsonReply(reply, callback);
    });
}

void HttpRobotClient::postJson(const QString &path, const QJsonObject &body, JsonCallback callback) {
    QNetworkRequest request = makeRequest(path);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = manager_.post(request, payload);
    attachTimeout(reply, write_timeout_ms_);
    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        handleJsonReply(reply, callback);
    });
}

void HttpRobotClient::downloadBinary(const QString &path, BinaryCallback callback) {
    QNetworkReply *reply = manager_.get(makeRequest(path));
    attachTimeout(reply, read_timeout_ms_);
    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        const QByteArray body = reply->isOpen() ? reply->readAll() : QByteArray();
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const auto headers = reply->rawHeaderPairs();
        bool ok = false;
        QByteArray result_body;
        QString error;

        if (reply->error() != QNetworkReply::NoError) {
            error = httpErrorMessage(reply, body);
        } else if (status != 200) {
            error = QString("HTTP status %1: %2").arg(status).arg(QString::fromUtf8(body.left(300)));
        } else {
            ok = true;
            result_body = body;
        }

        reply->deleteLater();
        callback(ok, result_body, headers, error);
    });
}

void HttpRobotClient::downloadFile(const QString &path, FileCallback callback) {
    QTemporaryFile *file = new QTemporaryFile(QDir(QDir::tempPath()).filePath("rocos-viz-mesh-XXXXXX"));
    file->setAutoRemove(false);
    if (!file->open()) {
        const QString error = QString("Can not create temporary download file: %1").arg(file->errorString());
        delete file;
        callback(false, QString(), QList<QNetworkReply::RawHeaderPair>(), error);
        return;
    }

    QNetworkReply *reply = manager_.get(makeRequest(path));
    file->setParent(reply);
    attachTimeout(reply, binary_timeout_ms_);

    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        const QByteArray chunk = reply->isOpen() ? reply->readAll() : QByteArray();
        if (!chunk.isEmpty()) {
            file->write(chunk);
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, file, callback]() {
        const QByteArray remaining = reply->isOpen() ? reply->readAll() : QByteArray();
        if (!remaining.isEmpty()) {
            file->write(remaining);
        }
        file->flush();
        file->close();

        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const auto headers = reply->rawHeaderPairs();
        const QString file_path = file->fileName();

        bool ok = false;
        QString result_path;
        QString error;

        if (reply->error() != QNetworkReply::NoError) {
            QFile::remove(file_path);
            error = httpErrorMessage(reply, QByteArray());
        } else if (status != 200) {
            QFile::remove(file_path);
            error = QString("HTTP status %1").arg(status);
        } else {
            ok = true;
            result_path = file_path;
        }

        reply->deleteLater();
        callback(ok, result_path, headers, error);
    });
}

QNetworkRequest HttpRobotClient::makeRequest(const QString &path) const {
    QUrl url(path);
    if (url.isRelative()) {
        url = base_url_.resolved(url);
    }

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/json");
    return request;
}

void HttpRobotClient::attachTimeout(QNetworkReply *reply, int timeout_ms) const {
    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, reply, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });
    QObject::connect(reply, &QNetworkReply::finished, timer, &QTimer::stop);
    timer->start(timeout_ms);
}

void HttpRobotClient::handleJsonReply(QNetworkReply *reply, JsonCallback callback) const {
    const QByteArray body = reply->isOpen() ? reply->readAll() : QByteArray();
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    JsonResult result{false, QJsonObject(), QString()};

    if (reply->error() != QNetworkReply::NoError) {
        result.error = httpErrorMessage(reply, body);
    } else if (status < 200 || status >= 300) {
        result.error = QString("HTTP status %1: %2").arg(status).arg(QString::fromUtf8(body.left(300)));
    } else {
        result = parseStandardResponse(body);
    }

    reply->deleteLater();
    callback(result.ok, result.data, result.error);
}

HttpRobotClient::JsonResult HttpRobotClient::parseStandardResponse(const QByteArray &body) const {
    QJsonParseError parse_error;
    const QJsonDocument doc = QJsonDocument::fromJson(body, &parse_error);
    if (parse_error.error != QJsonParseError::NoError || !doc.isObject()) {
        return {false, QJsonObject(), QString("Invalid JSON response: %1").arg(parse_error.errorString())};
    }

    const QJsonObject root = doc.object();
    if (!root.contains("success") || !root.value("success").isBool()) {
        return {false, QJsonObject(), "Invalid response envelope: missing boolean success"};
    }

    const bool success = root.value("success").toBool(false);
    const int code = root.value("code").toInt(-1);
    const QString message = root.value("message").toString();
    if (!success) {
        return {false, QJsonObject(), QString("API error code=%1 message=%2").arg(code).arg(message)};
    }

    const QJsonValue data_value = root.value("data");
    return {true, data_value.isObject() ? data_value.toObject() : QJsonObject(), QString()};
}

QString HttpRobotClient::httpErrorMessage(QNetworkReply *reply, const QByteArray &body) const {
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString message = reply->errorString();
    if (!body.isEmpty()) {
        message += QString(": %1").arg(QString::fromUtf8(body.left(300)));
    }
    if (status > 0) {
        message = QString("HTTP status %1, %2").arg(status).arg(message);
    }
    return message;
}

#ifndef HTTPROBOTCLIENT_H
#define HTTPROBOTCLIENT_H

#include <functional>

#include <QByteArray>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QUrl>

class HttpRobotClient : public QObject {
public:
    using JsonCallback = std::function<void(bool ok, const QJsonObject &data, const QString &error)>;
    using BinaryCallback = std::function<void(bool ok,
                                             const QByteArray &body,
                                             const QList<QNetworkReply::RawHeaderPair> &headers,
                                             const QString &error)>;
    using FileCallback = std::function<void(bool ok,
                                            const QString &file_path,
                                            const QList<QNetworkReply::RawHeaderPair> &headers,
                                            const QString &error)>;

    explicit HttpRobotClient(const QString &host, int port, QObject *parent = nullptr);
    ~HttpRobotClient();

    void setTimeouts(int connect_ms, int read_ms, int write_ms);

    void getJson(const QString &path, JsonCallback callback);
    void postJson(const QString &path, const QJsonObject &body, JsonCallback callback);
    void downloadBinary(const QString &path, BinaryCallback callback);
    void downloadFile(const QString &path, FileCallback callback);

private:
    struct JsonResult {
        bool ok;
        QJsonObject data;
        QString error;
    };

    QNetworkRequest makeRequest(const QString &path) const;
    void attachTimeout(QNetworkReply *reply, int timeout_ms) const;
    void handleJsonReply(QNetworkReply *reply, JsonCallback callback) const;
    JsonResult parseStandardResponse(const QByteArray &body) const;
    QString httpErrorMessage(QNetworkReply *reply, const QByteArray &body) const;

    QUrl base_url_;
    QNetworkAccessManager manager_;
    int connect_timeout_ms_{1000};
    int read_timeout_ms_{50};
    int write_timeout_ms_{500};
    int binary_timeout_ms_{300000};
};

#endif // HTTPROBOTCLIENT_H

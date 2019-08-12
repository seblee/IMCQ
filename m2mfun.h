#ifndef M2MFUN_H
#define M2MFUN_H

#include <QWidget>
#include <QtMqtt/QMqttClient>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QTimer>
#include <QMap>

#include "memberinfo.h"

#define HOST_NAME    ".iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define PRODUCT_KEY  "a1BlDo53A1B"

namespace Ui {
class M2MFun;
}

class M2MFun : public QWidget {
    Q_OBJECT

public:

    explicit M2MFun(QWidget *parent = nullptr);
    ~M2MFun();

    void deviceNameSet(QString DN);

private slots:

    void on_pushButtonRefresh_clicked();

    void on_pushButtonConnect_clicked();

    void on_pushButtonSend_clicked();

    void brokerDisconnected();

    void updateState(void);

    void ReplyReadFunc(QNetworkReply *reply);

    void handleTimeOut();

private:

    Ui::M2MFun *ui;

    QMqttClient *client;

    QString Hostname;
    quint16 Port;
    QString Username;
    QString Password;
    QString ClientId;
    QString deviceName;

    QNetworkAccessManager *m_AccessManager;
    QNetworkReply *m_reply;
    QNetworkRequest m_Request;
    QTimer timer;
    QMap<QString, MemberInfo>memberList;

    QString   hmac_md5(QByteArray key,
                       QByteArray baseString);

    QString   hmac_sha1(QByteArray key,
                        QByteArray baseString);

    void      subscribe();

    quint64   get_random_number();

    QUrlQuery getQueryDeviceQurey();

    void      caculateClientPara();

    void      getMessage(const QByteArray    & message,
                         const QMqttTopicName& topic);

    void      stringToHtmlFilter(QString& str);

    void      stringToHtml(QString& str,
                           QColor   crl);
};

#endif // M2MFUN_H

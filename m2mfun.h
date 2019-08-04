#ifndef M2MFUN_H
#define M2MFUN_H

#include <QMainWindow>
#include <QtMqtt/QMqttClient>
#include <QKeyEvent>

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define HOST_NAME ".iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define PRODUCT_KEY "a1BlDo53A1B"
#define PRODUCT_SECRET "7jluWm1zql7bt8qK"
#define DEVICE_SECRET "kkwHzbm3hVbaoepFdx37Exr3fXyDVcPq"

namespace Ui {
class M2MFun;
}

class M2MFun : public QMainWindow {
    Q_OBJECT

public:

    explicit M2MFun(QWidget *parent = nullptr);
    ~M2MFun();

    void deviceNameSet(QString DN);

private:

    Ui::M2MFun *ui;
    QMqttClient *client;
    void keyPressEvent(QKeyEvent *event);
    QString Hostname;
    quint16 Port;
    QString Username;
    QString Password;
    QString ClientId;

    QString deviceName;
    QString hmac_md5(QByteArray key,
                     QByteArray baseString);

    void    subscribe();

    QNetworkAccessManager *m_AccessManager;
    QNetworkReply *m_reply;
    QNetworkRequest m_Request;

    quint64   get_random_number();

    QUrlQuery getQueryDeviceQurey();

private slots:

    void brokerDisconnected();

    void updateState(void);
    void on_pushButtonConnect_clicked();
    void on_pushButtonSend_clicked();
    void on_pushButtonRefresh_clicked();
};

#endif // M2MFUN_H

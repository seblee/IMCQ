#ifndef M2MFUN_H
#define M2MFUN_H

#include <QMainWindow>
#include <QtMqtt/QMqttClient>
#include <QKeyEvent>


#define HOST_NAME ".iot-as-http2.cn-shanghai.aliyuncs.com"
#define PRODUCT_KEY "a1X2bEnP82z"
#define PRODUCT_SECRET "7jluWm1zql7bt8qK"
#define DEVICE_SECRET "ga7XA6KdlEeiPXQPpRbAjOZXwG8ydgSe"

namespace Ui {
class M2MFun;
}

class M2MFun : public QMainWindow {
    Q_OBJECT

public:

    explicit M2MFun(QWidget *parent = nullptr);
    explicit M2MFun(QString DN);
    ~M2MFun();

private:

    Ui::M2MFun *ui;
    QString Hostname;
    quint16 Port;
    QString Username;
    QString Password;
    QString ClientId;
    QString hmac_md5(QByteArray key,
                     QByteArray baseString);
};

#endif // M2MFUN_H

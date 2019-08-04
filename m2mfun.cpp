#include "m2mfun.h"
#include "ui_m2mfun.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QUrlQuery>
#include <QTime>

M2MFun::M2MFun(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::M2MFun),
    client(nullptr)
{
    ui->setupUi(this);
    client = new QMqttClient(this);
    connect(client, &QMqttClient::stateChanged,    this,
            &M2MFun::updateState);

    connect(client, &QMqttClient::connected,       this,
            &M2MFun::subscribe);

    connect(client, &QMqttClient::disconnected,    this,
            &M2MFun::subscribe);

    connect(client, &QMqttClient::messageReceived, this,
            [this](const QByteArray& message,
                   const QMqttTopicName& topic) {
        const QString content = QDateTime::currentDateTime().toString() +
                                QLatin1String(" Received Topic: ") +
                                topic.name() +
                                QLatin1String(" Message: ") +
                                message +
                                QLatin1Char('\n');
        ui->textEditReceive->insertPlainText(content);
    });

    connect(client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString() +
                                QLatin1String(" PingResponse") +
                                QLatin1Char('\n');
        ui->textEditLog->append(content);
    });

    connect(client, &QMqttClient::errorChanged, this, [this]() {
        const QString content = QString::number(client->error());
        ui->statusbar->showMessage(content, 3000);
    });

    updateState();
    qDebug() << tr("parent:");
}

M2MFun::~M2MFun() {
    delete ui;
}

void M2MFun::deviceNameSet(QString DN)
{
    qDebug() << tr("DN:");
    Hostname.clear();
    Username.clear();
    Password.clear();
    ClientId.clear();

    Hostname.append(PRODUCT_KEY);
    Hostname.append(HOST_NAME);
    Port       = 1883;
    deviceName = QString(DN);
    Username   = QString("%1&%2").arg(DN).arg(QString(PRODUCT_KEY));
    ClientId   = QString("ClientM2MB|securemode=2,signmethod=hmacmd5|");

    QByteArray key(DEVICE_SECRET);
    QByteArray baseString(tr("clientId%1deviceName%2productKey%3")
                          .arg(tr("ClientM2MB"))
                          .arg(DN)
                          .arg(PRODUCT_KEY)
                          .toUtf8());
    qDebug() << tr("baseString:") << baseString;
    Password = hmac_md5(key, baseString);
    qDebug() << tr("Hostname:") << Hostname;
    qDebug() << tr("Port:") << QString::number(Port, 10);
    qDebug() << tr("Username:") << Username;
    qDebug() << tr("Password:") << Password;
    qDebug() << tr("ClientId:") << ClientId;

    client->setHostname(Hostname);
    client->setPort(Port);
    client->setUsername(Username);
    client->setPassword(Password);
    client->setClientId(ClientId);
    client->setProtocolVersion(QMqttClient::MQTT_3_1);
}

void M2MFun::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
        this->deleteLater();
    }
}

QString M2MFun::hmac_md5(QByteArray key, QByteArray baseString)
{
    // HMAC-SHA-1 block size, defined in SHA-1 standard
    int blockSize = 64;

    qDebug() << tr("key:") << key;
    qDebug() << tr("baseString:") << baseString;

    // if key is longer than block size (64), reduce key length with md5 compression
    if (key.length() > blockSize) {
        key = QCryptographicHash::hash(key, QCryptographicHash::Md5);
    }

    // initialize inner padding with char "6"
    QByteArray innerPadding(blockSize, char(0x36));

    // initialize outer padding with char "/"
    QByteArray outerPadding(blockSize, char(0x5c));


    // ascii characters 0x36 ("6") and 0x5c ("/") are selected because they have
    // large Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)
    for (int i = 0; i < key.length(); i++) {
        // XOR operation between every byte in key and innerpadding, of key length
        innerPadding[i] = innerPadding[i] ^ key.at(i);

        // XOR operation between every byte in key and outerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i);
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part  = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Md5));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Md5);
    return hashed.toHex().toUpper();
}

void M2MFun::subscribe()
{
    if (client->state() == QMqttClient::Connected)
    {
        auto subscription =  client->subscribe(tr("/%1/%2/user/M2MGET")
                                               .arg(tr(PRODUCT_KEY))
                                               .arg(deviceName));
        qDebug() << tr("subscribe");

        if (!subscription)
        {
            QMessageBox::critical(this,
                                  QLatin1String("Error"),
                                  QLatin1String(
                                      "Could not subscribe. Is there a valid connection?"));
        }
    }
}

quint64 M2MFun::get_random_number()
{
    quint64 srand = QDateTime::currentDateTime().toTime_t();

    quint64 msec = static_cast<quint64>(QTime::currentTime().msec());

    srand *= 1000;
    srand += msec;

    return srand;
}

/***
 * LTAIGjMmXQtgefYP 9o1ujvbf18ESe6mFpmJFpQwsHmLwl2
 *
 */
QUrlQuery M2MFun::getQueryDeviceQurey()
{
    QUrlQuery query;

    query.addQueryItem(tr("Format"),          tr("JSON"));
    query.addQueryItem(tr("Action"),          tr("QueryDevice"));
    query.addQueryItem(tr("ProductKey"),      tr(PRODUCT_KEY));
    query.addQueryItem(tr("PageSize"),        tr("10"));
    query.addQueryItem(tr("CurrentPage"),     tr("1"));
    query.addQueryItem(tr("Version"),         tr("2018-01-20"));
    query.addQueryItem(tr("AccessKeyId"),     tr("LTAIGjMmXQtgefYP"));
    query.addQueryItem(tr("Signature"),       tr("LTAI37ciXEGJxwP5"));
    query.addQueryItem(tr("SignatureMethod"), tr("HMAC-SHA1"));
    query.addQueryItem(tr("Timestamp"),
                       QDateTime::currentDateTime()
                       .toString("YYYY-MM-DDTHH:mm:ssZ"));
    query.addQueryItem(tr("SignatureVersion"), tr("1.0"));
    query.addQueryItem(tr("SignatureNonce"),
                       QString::number(get_random_number(), 10));
    query.addQueryItem(tr("RegionId"), tr("cn-shanghai"));

    qDebug() << tr("query:") << query.toString();


    return query;
}

void M2MFun::brokerDisconnected()
{
    ui->pushButtonConnect->setText(tr("Connect"));
}

void M2MFun::updateState()
{
    const QString content = QDateTime::currentDateTime().toString() +
                            QLatin1String(": State Change") +
                            QString::number(client->state()) +
                            QLatin1Char('\n');

    ui->textEditLog->append(content);
}

void M2MFun::on_pushButtonConnect_clicked()
{
    if (client->state() == QMqttClient::Disconnected)
    {
        qDebug() << tr("Connect");
        ui->pushButtonConnect->setText(tr("Disconnect"));
        client->connectToHostEncrypted();
    }
    else
    {
        ui->pushButtonConnect->setText(tr("Connect"));
        client->disconnectFromHost();
    }
}

void M2MFun::on_pushButtonSend_clicked()
{
    if (client->state() == QMqttClient::Connected)
    {
        //        if (client->publish(QString(tr("/%1/%2/user/M2MPUT")
        //                                    .arg(PRODUCT_KEY)
        //                                    .arg(deviceName)),
        //                            ui->plainTextEditSend->getPaintContext().))
        {
            QMessageBox::critical(this, QLatin1String("Error"), QLatin1String(
                                      "Could not publish message"));
        }
    }
}

void M2MFun::on_pushButtonRefresh_clicked()
{
    QUrl url("https://iot.cn-shanghai.aliyuncs.com");
}

#include "m2mfun.h"
#include "ui_m2mfun.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

M2MFun::M2MFun(QWidget *parent) : QMainWindow(parent), ui(new Ui::M2MFun), client(
        nullptr)
{
    ui->setupUi(this);
    client = new QMqttClient(this);
    connect(client, &QMqttClient::stateChanged,    this, &M2MFun::updateState);
    connect(client, &QMqttClient::connected,       this, &M2MFun::subscribe);
    connect(client, &QMqttClient::disconnected,    this, &M2MFun::subscribe);
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
}

M2MFun::~M2MFun()
{
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
    deviceName =  DN;

    if (memberList.isEmpty())
    {
        on_pushButtonRefresh_clicked();
    } else {
        caculateClientPara();
    }
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

    // if key is longer than block size (64), reduce key length with md5
    // compression
    if (key.length() > blockSize)
    {
        key = QCryptographicHash::hash(key, QCryptographicHash::Md5);
    }

    // initialize inner padding with char "6"
    QByteArray innerPadding(blockSize, char(0x36));

    // initialize outer padding with char "/"
    QByteArray outerPadding(blockSize, char(0x5c));

    // ascii characters 0x36 ("6") and 0x5c ("/") are selected because they have
    // large Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)
    for (int i = 0; i < key.length(); i++)
    {
        // XOR operation between every byte in key and innerpadding, of key
        // length
        innerPadding[i] = innerPadding[i] ^ key.at(i);

        // XOR operation between every byte in key and outerpadding, of key
        // length
        outerPadding[i] = outerPadding[i] ^ key.at(i);
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString
    // ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part  = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Md5));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Md5);
    return hashed.toHex().toUpper();
}

QString M2MFun::hmac_sha1(QByteArray key, QByteArray baseString)
{
    // HMAC-SHA-1 block size, defined in SHA-1 standard
    int blockSize = 64;

    if (key.length() > blockSize)
    {
        // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    // initialize inner padding with char "6"
    QByteArray innerPadding(blockSize, char(0x36));

    // initialize outer padding with char "/"
    QByteArray outerPadding(blockSize, char(0x5c));

    // ascii characters 0x36 ("6") and 0x5c ("/") are selected because they have large
    // Hamming distance (http://en.wikipedia.org/wiki/Hamming_distance)
    for (int i = 0; i < key.length(); i++)
    {
        // XOR operation between every byte in key and innerpadding, of key length
        innerPadding[i] = innerPadding[i] ^ key.at(i);

        // XOR operation between every byte in key and outerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i);
    }

    // result = hash ( outerPadding CONCAT hash ( innerPadding CONCAT baseString ) ).toBase64
    QByteArray total = outerPadding;
    QByteArray part  = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);

    /// 注意——>把字符串hashed转换为Hex，内存中的ASCII码arrayFromHexString
    QByteArray arrayFromHexString = QByteArray::fromHex(hashed.toHex());

    // qDebug() << "hmacSha1内存中的ASCII码 arrayFromHexString \n" <<
    //  arrayFromHexString.toHex();
    return hashed.toBase64();
}

void M2MFun::subscribe()
{
    if (client->state() == QMqttClient::Connected)
    {
        auto subscription =
            client->subscribe(tr("/%1/%2/user/M2MGET").arg(tr(PRODUCT_KEY)).arg(
                                  deviceName));
        qDebug() << tr("subscribe");

        if (!subscription)
        {
            QMessageBox::critical(
                this, QLatin1String("Error"),
                QLatin1String("Could not subscribe. Is there a valid connection?"));
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
 * LTAIejxZIeY54fCN gBjEbIY4fE10sfxKJqoTJ21y4TGpKp
 */
QUrlQuery M2MFun::getQueryDeviceQurey()
{
    QUrlQuery query;

    query.addQueryItem(tr("AccessKeyId"),      tr("LTAIejxZIeY54fCN"));
    query.addQueryItem(tr("Action"),           tr("QueryDevice"));
    query.addQueryItem(tr("CurrentPage"),      tr("1"));
    query.addQueryItem(tr("Format"),           tr("JSON"));
    query.addQueryItem(tr("PageSize"),         tr("10"));
    query.addQueryItem(tr("ProductKey"),       tr(PRODUCT_KEY));
    query.addQueryItem(tr("RegionId"),         tr("cn-shanghai"));
    query.addQueryItem(tr("SignatureMethod"),  tr("HMAC-SHA1"));
    query.addQueryItem(tr("SignatureNonce"),   QString::number(get_random_number(), 10));
    query.addQueryItem(tr("SignatureVersion"), tr("1.0"));
    query.addQueryItem(tr("Timestamp"),
                       QUrl::toPercentEncoding(QDateTime::currentDateTime()
                                               .toUTC()
                                               .toString(Qt::ISODate)
                                               + tr("Z")));
    query.addQueryItem(tr("Version"), tr("2018-01-20"));

    QByteArray stringToSign;
    stringToSign.append("GET");
    stringToSign.append("&");
    stringToSign.append(QUrl::toPercentEncoding("/", "", "/"));
    stringToSign.append("&");
    stringToSign.append(QUrl::toPercentEncoding(query.toString().toUtf8(), "", ""));

    QByteArray sha1 =
        hmac_sha1(QByteArray("gBjEbIY4fE10sfxKJqoTJ21y4TGpKp&"),
                  stringToSign).toUtf8();

    query.addQueryItem("Signature", QUrl::toPercentEncoding(sha1));

    return query;
}

void M2MFun::caculateClientPara()
{
    if (memberList.isEmpty())
    {
        QMessageBox::critical(
            this, QLatin1String("Error"),
            QLatin1String("memberList isEmpty"));
        return;
    }

    if (memberList.contains(deviceName))
    {
        Username = QString("%1&%2")
                   .arg(deviceName)
                   .arg(memberList[deviceName].producekey());
        ClientId = QString("%1|securemode=2,signmethod=hmacmd5|")
                   .arg(memberList[deviceName].deviceid());

        QByteArray key = memberList[deviceName].devicesceret().toUtf8();
        QByteArray baseString(tr("clientId%1deviceName%2productKey%3")
                              .arg(memberList[deviceName].deviceid())
                              .arg(deviceName)
                              .arg(memberList[deviceName].producekey())
                              .toUtf8());
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
    else
    {
        QMessageBox::critical(
            this, QLatin1String("Error"),
            QLatin1String("deviceName is not correct"));
    }
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
        QJsonObject json;
        json.insert(tr("dName"), QJsonValue(ui->labelReceiver->text()));
        json.insert(tr("Message"),
                    QJsonValue(ui->plainTextEditSend->toPlainText()));

        QJsonDocument document;
        document.setObject(json);
        QByteArray message = document.toJson(QJsonDocument::Compact);


        if (client->publish(QString(tr("/%1/%2/user/M2MPUT")
                                    .arg(PRODUCT_KEY)
                                    .arg(deviceName)),
                            message) == -1)
        {
            QMessageBox::critical(this, QLatin1String("Error"),
                                  QLatin1String("Could not publish message"));
        }
    }
}

void M2MFun::on_pushButtonRefresh_clicked()
{
    QUrl url("https://iot.cn-shanghai.aliyuncs.com");

    QUrlQuery query = getQueryDeviceQurey();

    url.setQuery(query);

    // qDebug() << url.toString();

    m_AccessManager = new QNetworkAccessManager(this);

    QSslConfiguration config;
    config.setPeerVerifyDepth(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_2);
    m_Request.setSslConfiguration(config);
    m_Request.setUrl(url);
    m_Request.setRawHeader("Host", url.host().toLocal8Bit());

    m_reply = m_AccessManager->get(m_Request);
    connect(m_AccessManager,
            &QNetworkAccessManager::finished,
            this,
            &M2MFun::ReplyReadFunc);
    disconnect(&timer, &QTimer::timeout, nullptr, nullptr);
    connect(&timer, &QTimer::timeout, this, &M2MFun::handleTimeOut);

    if (timer.isActive())
    {
        timer.stop();
    }
    timer.start(5000);
}

void M2MFun::ReplyReadFunc(QNetworkReply *reply)
{
    if (timer.isActive())
    {
        timer.stop();
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonParseError jsonError;
        QJsonDocument   doucment = QJsonDocument::fromJson(response, &jsonError);
        ui->textEditLog->append(QString::fromUtf8(response));

        if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
        {
            int PageCount;
            int Page;
            int PageSize;
            QString RequestId;
            bool    Success;
            int     Total;

            if (doucment.isObject())                    // JSON 文档为对象
            {
                QJsonObject object = doucment.object(); // 转化为对象
                qDebug() << tr("doucment.isObject()");

                if (object.contains("PageCount"))
                {
                    PageCount = object.value("PageCount").toVariant().toInt();
                    qDebug() << tr("PageCount:") << PageCount;
                }

                if (object.contains("Page"))
                {
                    Page = object.value("Page").toVariant().toInt();
                    qDebug() << tr("Page:") << Page;
                }

                if (object.contains("PageSize"))
                {
                    PageSize = object.value("PageSize").toVariant().toInt();
                    qDebug() << tr("PageSize:") << PageSize;
                }

                if (object.contains("RequestId"))
                {
                    RequestId = object.value("RequestId").toVariant().toString();
                    qDebug() << tr("RequestId:") << RequestId;
                }

                if (object.contains("Success"))
                {
                    Success = object.value("Success").toVariant().toBool();
                    qDebug() << tr("Success:") << Success;
                }

                if (object.contains("Total"))
                {
                    Total = object.value("Total").toVariant().toInt();
                    qDebug() << tr("Total:") << Total;
                }

                if (object.contains("Data"))
                {
                    qDebug() << tr("Data.isObject()") <<
                        object.value("Data").toVariant().toString();

                    QJsonObject dataObject = object.value("Data").toObject();

                    qDebug() << tr("dataObject") << dataObject;

                    if (dataObject.contains("DeviceInfo"))
                    {
                        qDebug() << tr("dataObject contains DeviceInfo");
                        memberList.clear();
                        ui->listWidget->clear();
                        QJsonArray DeviceInfo =
                            dataObject.value("DeviceInfo").toArray();
                        int deviceSize = DeviceInfo.size();

                        for (int i = 0; i < deviceSize; i++)
                        {
                            QJsonObject Device = DeviceInfo.at(i).toObject();

                            QString DeviceId =
                                Device.value("DeviceId").toVariant().toString();
                            QString DeviceName =
                                Device.value("DeviceName").toVariant().
                                toString();
                            QString Nickname =
                                Device.value("Nickname").toVariant().toString();
                            QString ProductKey =
                                Device.value("ProductKey").toVariant().
                                toString();
                            QString DeviceSecret =
                                Device.value("DeviceSecret").toVariant().
                                toString();
                            QString UtcModified =
                                Device.value("UtcModified").toVariant().
                                toString();
                            QString GmtCreate =
                                Device.value("GmtCreate").toVariant().toString();
                            QString UtcCreate =
                                Device.value("UtcCreate").toVariant().toString();
                            QString GmtModified =
                                Device.value("GmtModified").toVariant().
                                toString();
                            QString IotId =
                                Device.value("IotId").toVariant().toString();
                            QString DeviceStatus =
                                Device.value("DeviceStatus").toVariant().
                                toString();

                            memberList.insert(DeviceName,
                                              MemberInfo(DeviceId,
                                                         DeviceName,
                                                         Nickname,
                                                         ProductKey,
                                                         DeviceSecret,
                                                         UtcModified,
                                                         GmtCreate,
                                                         UtcCreate,
                                                         GmtModified,
                                                         IotId,
                                                         DeviceStatus));
                            qDebug() << tr("DeviceName") << DeviceName;
                            QListWidgetItem *Item =
                                new QListWidgetItem(DeviceName, ui->listWidget);
                            ui->listWidget->addItem(Item);
                        }

                        if (ui->listWidget->count() > 0)
                        {
                            ui->listWidget->setCurrentRow(0);
                        }
                        caculateClientPara();
                    }
                }
            }
        }
        else
        {}
    }
    else
    {
        ui->listWidget->addItem(tr("error:") +
                                QString::number(reply->error(), 10));
    }
    m_reply->deleteLater();

    ui->statusbar->showMessage(tr("Refresh Complete"), 3000);
}

void M2MFun::handleTimeOut()
{
    timer.stop();
    m_reply->abort();
    m_reply->deleteLater();
    ui->statusbar->showMessage(tr("Refresh time out"), 3000);
}

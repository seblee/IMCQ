#include "m2mfun.h"
#include "ui_m2mfun.h"
#include <QCryptographicHash>
#include <QDebug>

M2MFun::M2MFun(QWidget *parent) : QMainWindow(parent), ui(new Ui::M2MFun) {
    ui->setupUi(this);
}

M2MFun::M2MFun(QString DN)
{
    Hostname.clear();
    Username.clear();
    Password.clear();
    ClientId.clear();

    Hostname.append(PRODUCT_KEY);
    Hostname.append(HOST_NAME);
    Port     = 1883;
    Username = QString("%1&%2").arg(DN).arg(QString(PRODUCT_KEY));
    ClientId = QString("%1|securemode=2,signmethod=hmacmd5|").arg(DN);

    QByteArray key(DEVICE_SECRET);
    QByteArray baseString(tr("clientId%1deviceName%2productKey%3")
                          .arg(ClientId)
                          .arg(DN)
                          .arg(PRODUCT_KEY)
                          .toUtf8());
    Password = hmac_md5(key, baseString);
    qDebug() << tr("md5:") << Password;
}

M2MFun::~M2MFun() {
    delete ui;
}

QString M2MFun::hmac_md5(QByteArray key, QByteArray baseString)
{
    // HMAC-SHA-1 block size, defined in SHA-1 standard
    int blockSize = 64;

    qDebug() << tr("key:") << key;
    qDebug() << tr("baseString:") << baseString;

    // if key is longer than block size (64), reduce key length with md5
    // compression
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

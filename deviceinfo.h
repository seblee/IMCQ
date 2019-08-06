#ifndef DEVICEINFO_H
#define DEVICEINFO_H
#include <QString>


class DeviceInfo {
public:

    DeviceInfo();
    DeviceInfo(const QString& deviceid,
               const QString& devicename,
               const QString& nickname,
               const QString& producekey,
               const QString& devicesceret,
               const QString& utcmodified,
               const QString& gmtcreate,
               const QString& utccreate,
               const QString& gmtmodified,
               const QString& iotid,
               const QString& devicestatus);

    QString deviceid() const;

    QString devicename()const;

    QString nickname()const;

    QString producekey()const;

    QString devicesceret()const;

    QString utcmodified()const;

    QString gmtcreate()const;

    QString utccreate()const;

    QString gmtmodified()const;

    QString iotid()const;

    QString devicestatus()const;

private:

    QString DeviceId;
    QString DeviceName;
    QString Nickname;
    QString ProductKey;
    QString DeviceSecret;
    QString UtcModified;
    QString GmtCreate;
    QString UtcCreate;
    QString GmtModified;
    QString IotId;
    QString DeviceStatus;
};

#endif // DEVICEINFO_H

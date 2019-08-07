#ifndef MEMBERINFO_H
#define MEMBERINFO_H

#include <QString>

class MemberInfo {
public:

    MemberInfo();
    MemberInfo(const QString& deviceid,
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

    QString devicename() const;

    QString nickname() const;

    QString producekey() const;

    QString devicesceret() const;

    QString utcmodified() const;

    QString gmtcreate() const;

    QString utccreate() const;

    QString gmtmodified() const;

    QString iotid() const;

    QString devicestatus() const;

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

#endif // MEMBERINFO_H

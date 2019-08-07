#include "memberinfo.h"

MemberInfo::MemberInfo()
{}

MemberInfo::MemberInfo(const QString& deviceid,
                       const QString& devicename,
                       const QString& nickname,
                       const QString& producekey,
                       const QString& devicesceret,
                       const QString& utcmodified,
                       const QString& gmtcreate,
                       const QString& utccreate,
                       const QString& gmtmodified,
                       const QString& iotid,
                       const QString& devicestatus) :
    DeviceId(deviceid),
    DeviceName(devicename),
    Nickname(nickname),
    ProductKey(producekey),
    DeviceSecret(devicesceret),
    UtcModified(utcmodified),
    GmtCreate(gmtcreate),
    UtcCreate(utccreate),
    GmtModified(gmtmodified),
    IotId(iotid),
    DeviceStatus(devicestatus)
{}

QString MemberInfo::deviceid() const {
    return DeviceId;
}

QString MemberInfo::devicename() const {
    return DeviceName;
}

QString MemberInfo::nickname() const {
    return Nickname;
}

QString MemberInfo::producekey() const {
    return ProductKey;
}

QString MemberInfo::devicesceret() const {
    return DeviceSecret;
}

QString MemberInfo::utcmodified() const {
    return UtcModified;
}

QString MemberInfo::gmtcreate() const {
    return GmtCreate;
}

QString MemberInfo::utccreate() const {
    return UtcCreate;
}

QString MemberInfo::gmtmodified() const {
    return GmtModified;
}

QString MemberInfo::iotid() const {
    return IotId;
}

QString MemberInfo::devicestatus() const {
    return DeviceStatus;
}

#include "deviceinfo.h"

DeviceInfo::DeviceInfo()
{}

DeviceInfo::DeviceInfo(const QString& deviceid,
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

QString DeviceInfo::deviceid() const {
    return DeviceId;
}

QString DeviceInfo::devicename() const {
    return DeviceName;
}

QString DeviceInfo::nickname() const {
    return Nickname;
}

QString DeviceInfo::producekey() const {
    return ProductKey;
}

QString DeviceInfo::devicesceret() const {
    return DeviceSecret;
}

QString DeviceInfo::utcmodified() const {
    return UtcModified;
}

QString DeviceInfo::gmtcreate() const {
    return GmtCreate;
}

QString DeviceInfo::utccreate() const {
    return UtcCreate;
}

QString DeviceInfo::gmtmodified() const {
    return GmtModified;
}

QString DeviceInfo::iotid() const {
    return IotId;
}

QString DeviceInfo::devicestatus() const {
    return DeviceStatus;
}

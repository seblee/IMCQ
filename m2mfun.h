#ifndef M2MFUN_H
#define M2MFUN_H

#include <QWidget>
#include <QKeyEvent>

namespace Ui {
class M2MFun;
}

class M2MFun : public QWidget {
    Q_OBJECT

public:

    explicit M2MFun(QWidget *parent = nullptr);
    explicit M2MFun(QString deviceName);

    ~M2MFun();

private:

    Ui::M2MFun *ui;
    void keyPressEvent(QKeyEvent *event);
};

#endif // M2MFUN_H

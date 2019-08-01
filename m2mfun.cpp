#include "m2mfun.h"
#include "ui_m2mfun.h"

M2MFun::M2MFun(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::M2MFun)
{
    ui->setupUi(this);
}

M2MFun::~M2MFun()
{
    delete ui;
}

void M2MFun::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}

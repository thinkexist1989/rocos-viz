#include "PositionBar.h"

PositionBar::PositionBar(QWidget *parent) :
    QProgressBar(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    setRange(minPos, maxPos);
    updateDisp();
}

void PositionBar::setMinimum(double val)
{
    minPos = val;
    ((QProgressBar*)this)->setMinimum(minPos * 1000);
}

void PositionBar::setMaximum(double val)
{
    maxPos = val;
    ((QProgressBar*)this)->setMaximum(maxPos * 1000);
}

void PositionBar::setRange(double min, double max)
{
    setMinimum(min);
    setMaximum(max);
}

void PositionBar::setName(const QString &s)
{
    name = s;
    updateDisp();
}

void PositionBar::setValue(double val)
{
    posVal = val;
    ((QProgressBar*)this)->setValue(posVal * 1000);
    updateDisp();

    emit valueChanged(posVal);
}

void PositionBar::updateDisp()
{
    this->setFormat(tr(" %1 : %2").arg(name).arg(posVal, 0 ,'f', 3));
//      this->setFormat(tr("Joint Position: %1").arg(jointPos));
}

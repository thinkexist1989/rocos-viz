#include "JointPositionBar.h"

JointPositionBar::JointPositionBar(QWidget *parent) :
    QProgressBar(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    setRange(minPos, maxPos);
    updateDisp();
}

void JointPositionBar::setMinimum(double val)
{
    minPos = val;
    ((QProgressBar*)this)->setMinimum(minPos * 1000);
}

void JointPositionBar::setMaximum(double val)
{
    maxPos = val;
    ((QProgressBar*)this)->setMaximum(maxPos * 1000);
}

void JointPositionBar::setRange(double min, double max)
{
    setMinimum(min);
    setMaximum(max);
}

void JointPositionBar::setJointNum(int num)
{
    jointNum = num;
    updateDisp();
}

void JointPositionBar::setValue(double val)
{
    jointPos = val;
    ((QProgressBar*)this)->setValue(jointPos * 1000);
    updateDisp();

    emit valueChanged(jointPos);
}

void JointPositionBar::updateDisp()
{
    this->setFormat(tr("Joint %1 Position: %2").arg(jointNum).arg(jointPos));
//      this->setFormat(tr("Joint Position: %1").arg(jointPos));
}

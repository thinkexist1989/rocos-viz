#include "JointPositionBar.h"

JointPositionBar::JointPositionBar(QWidget *parent) :
    QProgressBar(parent)
{
    this->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    updateDisp();
}

void JointPositionBar::setMinimum(double val)
{
    ((QProgressBar*)this)->setMinimum(val * 1000);
}

void JointPositionBar::setMaximum(double val)
{
    ((QProgressBar*)this)->setMaximum(val * 1000);
}

void JointPositionBar::setRange(double min, double max)
{
    setMinimum(min);
    setMaximum(max);
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
    this->setFormat(tr("Joint Position: %1").arg(jointPos));

}

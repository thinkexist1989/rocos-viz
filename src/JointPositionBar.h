#ifndef JOINTPOSITIONBAR_H
#define JOINTPOSITIONBAR_H

#include <QProgressBar>


// corecrt_math_defines.h, line 22:
#define M_PI       3.14159265358979323846   // pi
#define FACTOR     1000    //缩放1000倍到整形显示

class JointPositionBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit JointPositionBar(QWidget *parent = nullptr);

public slots:
    void setMinimum(double val); //为了显示
    void setMaximum(double val);
    void setRange(double min, double max);

    void setValue(double val);
    void updateDisp();

signals:
    void valueChanged(double value);

private:

    double jointPos = 0.6; //用浮点型保存关节位置值

    double minPos = -M_PI;    //为了显示
    double maxPos =  M_PI;

    double minLimit = -M_PI; //真实限位
    double maxLimit =  M_PI;

};

#endif // JOINTPOSITIONBAR_H

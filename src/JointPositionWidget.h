#ifndef JOINTPOSITIONWIDGET_H
#define JOINTPOSITIONWIDGET_H

#include <QWidget>


namespace Ui {
class JointPositionWidget;
}

class JointPositionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JointPositionWidget(QWidget *parent = nullptr);
    ~JointPositionWidget();

private slots:
    void updateJointPosition(double val);

    void on_jointPosValBar_valueChanged(double value);

    void on_jointPosPlusButton_clicked();

    void on_jointPosMinusButton_clicked();

signals:
    void jointPositionJogging(double val); //关节点动

private:
    Ui::JointPositionWidget *ui;

    double jointPos = 0; //用浮点型保存关节位置值
    double step     = 0.001; //每次步进的量


};

#endif // JOINTPOSITIONWIDGET_H

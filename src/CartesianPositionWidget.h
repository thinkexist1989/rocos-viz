#ifndef CARTESIANPOSITIONWIDGET_H
#define CARTESIANPOSITIONWIDGET_H

#include <QWidget>


namespace Ui {
class CartesianPositionWidget;
}

class CartesianPositionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CartesianPositionWidget(QWidget *parent = nullptr);
    ~CartesianPositionWidget();

    void updateDisp();

    void setName(const QString& s);

    void setFactor(double f);

public slots:
    void updateVal(double val);

    void on_cartesianPlusButton_clicked();

    void on_cartesianMinusButton_clicked();

    void on_cartesianPlusButton_pressed();

    void on_cartesianPlusButton_released();

    void on_cartesianMinusButton_pressed();

    void on_cartesianMinusButton_released();

signals:
    void cartesianJogging(double val); //关节点动

private:
    Ui::CartesianPositionWidget *ui;

    QString name;
    double cartesianVal = 0; //用浮点型保存笛卡尔位置值
    double step     = 0.001; //每次步进的量

    uint16_t val = 0; // Progress的值

    int    progressStep = 2000; // 最大步进量
    double factor = 0.5; //speed缩放因子

    QTimer* timerPlus;
    QTimer* timerMinus;

};

#endif // CARTESIANPOSITIONWIDGET_H

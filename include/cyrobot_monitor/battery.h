#ifndef BATTERY_H
#define BATTERY_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Battery; }
QT_END_NAMESPACE

class Battery : public QWidget

{
    Q_OBJECT

public:
    Battery(QWidget *parent = nullptr);
    void inputValue(double value);
    ~Battery();

protected:
    void paintEvent(QPaintEvent *);
    void drawBorder(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawText(QPainter *painter);



private:
    double _currentValue;
    int _margin;
    double _minValue;                //最小值
    double _maxValue;                //最大值
    int _batteryWidth;
    int _batteryHeight;

    QRectF batteryRect;             //电池主体区域
};

#endif // BATTERY_H

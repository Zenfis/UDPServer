#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QWidget>
#include <QSlider>

class DoubleSlider : public QSlider {
    Q_OBJECT

public:
    DoubleSlider(Qt::Orientation orientation, QWidget *parent = 0) : QSlider(orientation, parent) {
        connect(this, SIGNAL(valueChanged(int)),
                this, SLOT(notifyValueChanged(int)));
    }

signals:
    void doubleValueChanged(double value);

public slots:
    void notifyValueChanged(int value) {
        double doubleValue = value / 1000.0;
        emit doubleValueChanged(doubleValue);
    }
};


#endif // DOUBLESLIDER_H

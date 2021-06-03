#ifndef SPINSLIDERBOX_H
#define SPINSLIDERBOX_H

#include <QWidget>

namespace Ui {
class SpinSliderBox;
}

class SpinSliderBox : public QWidget
{
    Q_OBJECT

public:
    explicit SpinSliderBox(QWidget *parent = nullptr);
    ~SpinSliderBox();

    void setValue(int value);
    int getValue();

private:
    Ui::SpinSliderBox *ui;
};

#endif // SPINSLIDERBOX_H

#ifndef MINUTESSECONDSBOX_H
#define MINUTESSECONDSBOX_H

#include <QWidget>

namespace Ui {
class MinutesSecondsBox;
}

class MinutesSecondsBox : public QWidget
{
    Q_OBJECT

public:
    explicit MinutesSecondsBox(QWidget *parent = nullptr);
    ~MinutesSecondsBox();

    void setMunites(int min);
    int getMunites();
    void setSeconds(int sec);
    int getSeconds();

private:
    Ui::MinutesSecondsBox *ui;
};

#endif // MINUTESSECONDSBOX_H

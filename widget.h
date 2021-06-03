#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "roundprogressbar.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define DEFAULT_PROMO_WORKING_TIME 25
#define DEFAULT_PROMO_REST_TIME 5
#define DEFAULT_PROMO_LONG_REST_TIME 10
#define DEFAULT_PROMO_NUMBER 4
#define DEFAULT_TIMER_INTERVAL 1000

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void loadDefaultSettings();
    void loadValuesFromSettings();
    void reloadPromoClock();


private:
    Ui::Widget *ui;
    int _promo_working_time = DEFAULT_PROMO_WORKING_TIME;
    int _promo_rest_time = DEFAULT_PROMO_REST_TIME;
    int _promo_number = DEFAULT_PROMO_NUMBER;
    int _promo_long_rest_time = DEFAULT_PROMO_LONG_REST_TIME;
    bool _is_working_timer = true;
    double _current_stage_timer;
    int _current_max_time;
    int _current_stage = 0;
    int _current_promo_stage;

    RoundProgressBar* _bar;

    void _resetCurrentTimerValue();
    void _resetCurrentMaxTime();
    void _resetCurrentPromoStage();
    void _resetRoundProgressBar();
    void _promofocusFinished();
    void _moveToNextStage();
};
#endif // WIDGET_H

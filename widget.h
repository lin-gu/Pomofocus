#ifndef WIDGET_H
#define WIDGET_H

#include <QMediaPlayer>
#include <QWidget>
#include <QTimer>
#include <QMap>

#include "roundprogressbar.h"
#include "switchcontrol.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define DEFAULT_PROMO_WORKING_TIME_MINUTES 25
#define DEFAULT_PROMO_WORKING_TIME_SECONDS 0
#define DEFAULT_PROMO_REST_TIME_MINUTES 5
#define DEFAULT_PROMO_REST_TIME_SECONDS 0
#define DEFAULT_PROMO_LONG_REST_TIME_MINUTES 10
#define DEFAULT_PROMO_LONG_REST_TIME_SECONDS 0
#define DEFAULT_PROMO_NUMBER 4
#define DEFAULT_TIMER_INTERVAL 1000
#define DEFAULT_AUTO_START_BREAK false
#define DEFAULT_AUTO_START_WORKING false
#define DEFAULT_ALARM_SOUND_VOLUME 80
#define DEFAULT_ALARM_SOUND_REPEAT 1
#define DEFAULT_ALARM_SOUND_NAME "无声音"
#define DEFAULT_TICKING_SOUND_VOLUME 80
#define DEFAULT_TICKING_SOUND_NAME "无声音"

class Widget : public QWidget
{
    Q_OBJECT

public:
    typedef QMap<int, QPair<QString, QString> > SoundMap;

    Widget(QWidget *parent = nullptr);
    ~Widget();

    void init();
    void loadDefaultSettings();
    void loadValuesFromSettings();
    void reloadPromoClock();

    void autoBreakOnToggled(bool bChecked);
    void autoWorkingOnToggled(bool bChecked);

    QString getAlarmSoundPath(QString &sound_name);
    QString getTickingSoundPath(QString &sound_name);

private:
    Ui::Widget *ui;
    RoundProgressBar* _bar;
    SwitchControl * _ts_auto_start_break;
    SwitchControl * _ts_auto_start_working;

    //用于存储设置界面用户指定的参数
    int __pomo_working_time_minutes = DEFAULT_PROMO_WORKING_TIME_MINUTES;
    int __pomo_working_time_seconds = DEFAULT_PROMO_WORKING_TIME_SECONDS;
    int __pomo_rest_time_minutes = DEFAULT_PROMO_REST_TIME_MINUTES;
    int __pomo_rest_time_seconds = DEFAULT_PROMO_REST_TIME_SECONDS;
    int __pomo_long_rest_time_minutes = DEFAULT_PROMO_LONG_REST_TIME_MINUTES;
    int __pomo_long_rest_time_seconds = DEFAULT_PROMO_LONG_REST_TIME_SECONDS;
    int __pomo_number = DEFAULT_PROMO_NUMBER;
    bool __auto_start_break = DEFAULT_AUTO_START_BREAK;
    bool __auto_start_working = DEFAULT_AUTO_START_WORKING;
    QString __alarm_sound_name = DEFAULT_ALARM_SOUND_NAME;
    int __alarm_sound_volume = DEFAULT_ALARM_SOUND_VOLUME;
    int __alarm_sound_repeat = DEFAULT_ALARM_SOUND_REPEAT;
    QString __ticking_sound_name = DEFAULT_TICKING_SOUND_NAME;
    int __ticking_sound_volume = DEFAULT_TICKING_SOUND_VOLUME;


    //临时备份正在运行的设置参数
    int _backup_pomo_working_time_minutes;
    int _backup_pomo_working_time_seconds;
    int _backup_pomo_rest_time_minutes;
    int _backup_pomo_rest_time_seconds;
    int _backup_pomo_long_rest_time_minutes;
    int _backup_pomo_long_rest_time_seconds;
    int _backup_pomo_number;
    bool _backup_auto_start_break;
    bool _backup_auto_start_working;
    QString _backup_alarm_sound_name;
    int _backup_alarm_sound_volume;
    int _backup_alarm_sound_repeat;
    QString _backup_ticking_sound_name;
    int _backup_ticking_sound_volume;

    bool _is_working_timer = true;
    double _current_stage_timer;
    int _current_max_time;
    int _current_stage = 0;
    int _current_promo_stage;

    SoundMap _alarm_sounds_map;
    SoundMap _ticking_sounds_map;

    QMediaPlayer * _alarm_player;
    QMediaPlayer * _ticking_player;


    void _initAlarmSoundControlPanel();
    void _loadDefaultToggleSwitches();
    void _resetCurrentTimerValue();
    void _resetCurrentMaxTime();
    void _resetCurrentPromoStage();
    void _resetRoundProgressBar();
    void _promofocusFinished();
    void _moveToNextStage();
    void _store_current_settings();
    void _load_current_settings();
    void _update_settings_as_current();


    QString _getSoundPath(SoundMap &sound_map, QString &sound_name);
    void _playAlarmSound();
    void _playTickingSound();
    void _shutup();


};
#endif // WIDGET_H

#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QMediaPlayer>
#include <QWidget>
#include <QTimer>
#include <QSound>

#include "Components/roundprogressbar.h"
#include "Components/toggleswitch.h"
#include "Scenes/settingsscene.h"
#include "Includes/SettingsData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define DEFAULT_WINDOW_OPACITY 100
#define MAX_WINDOW_OPACITY 100
#define MIN_WINDOW_OPACITY 1


class MainScene : public QWidget
{
    Q_OBJECT

public:

    MainScene(QWidget *parent = nullptr);
    ~MainScene();

    void init();
    void loadDefaultSettings();
    void loadValuesFromSettings();
    void reloadPromoClock();

    void autoBreakOnToggled(bool bChecked);
    void autoWorkingOnToggled(bool bChecked);

    QString getAlarmSoundPath(QString &sound_name);
    QString getTickingSoundPath(QString &sound_name);

private slots:

    void on_tool_btn_settings_clicked();

    void on_slider_window_opacity_valueChanged(int value);

private:
    Ui::Widget *ui;
    RoundProgressBar* _bar;
    QTimer * _timer;
    SettingsScene * _settingsScene;

    //运行时设置数据
    SettingsData __settings_data;
    //切换到设置界面临时存储正在运行的设置数据
    SettingsData _settings_data_snapshot;

    //设置按钮音频
    QSound *_settingsBtnSound;

    //用于存储设置界面用户指定的参数
    int _backup_current_working_alarm_sound_repeat;
    int _backup_current_rest_alarm_sound_repeat;
    bool _backup_timer_is_active;


    //不是专注就是休息
    bool _is_working_timer = true;
    double _current_stage_timer;
    int _current_max_time;
    //从0开始计数 一个专注或者一个休息结束后 自加1
    //比如4个番茄钟 它的值是：0 1 2 3 4 5 6 7
    int _current_stage = 0;
    //从1开始计数 一个专注加上一个休息结束后 自加1
    //比如4个番茄钟 它的值是：1 2 3 4
    int _current_pomo_stage;
    int _current_working_alarm_sound_repeat;
    int _current_rest_alarm_sound_repeat;

    int _current_window_opacity = DEFAULT_WINDOW_OPACITY;

    QMediaPlayer *_player;
    QMediaPlayer::MediaStatus _player_status = QMediaPlayer::UnknownMediaStatus;

    QString _working_alarm_sound_path;
    QString _rest_alarm_sound_path;
    QString _ticking_sound_path;


    //计时器相关函数
    void _startTimer();
    void _stopTimer();

    void _initAlarmSoundControlPanel();
    void _loadDefaultToggleSwitches();
    void _resetCurrentTimerValue();
    void _resetCurrentMaxTime();
    void _resetCurrentPromoStage();
    void _resetRoundProgressBar();
    void _pomofocusFinished();
    void _moveToNextStage();
    void _store_current_settings();
    void _load_backup_settings();
    void _update_settings_as_backup();



    QString _getSoundPath(SoundMap &sound_map, QString &sound_name);
    void _playWorkingAlarmSound();
    bool _loadWorkingAlarmSound();
    void _playRestAlarmSound();
    bool _loadRestAlarmSound();
    void _playTickingSound();
    bool _loadTickingSound();
    void _shutup();


};
#endif // MAINSCENE_H

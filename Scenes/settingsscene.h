#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H

#include <QWidget>
#include <QSound>

#include "Components/toggleswitch.h"
#include "Includes/SettingsData.h"

namespace Ui {
class SettingsScene;
}

#define MIN_POMO_NUMBER 1
#define MAX_POMO_NUMBER 20

#define DEFAULT_POMO_WORKING_TIME_MINUTES 25
#define DEFAULT_POMO_WORKING_TIME_SECONDS 0
#define DEFAULT_POMO_REST_TIME_MINUTES 5
#define DEFAULT_POMO_REST_TIME_SECONDS 0
#define DEFAULT_POMO_LONG_REST_TIME_MINUTES 10
#define DEFAULT_POMO_LONG_REST_TIME_SECONDS 0
#define DEFAULT_POMO_NUMBER 4
#define DEFAULT_TIMER_INTERVAL 1000
#define DEFAULT_AUTO_START_BREAK false
#define DEFAULT_AUTO_START_WORKING false
#define DEFAULT_WORKING_ALARM_SOUND_VOLUME 80
#define DEFAULT_WORKING_ALARM_SOUND_REPEAT 1
#define DEFAULT_WORKING_ALARM_SOUND_NAME "无声音"
#define DEFAULT_REST_ALARM_SOUND_VOLUME 80
#define DEFAULT_REST_ALARM_SOUND_REPEAT 1
#define DEFAULT_REST_ALARM_SOUND_NAME "无声音"
#define DEFAULT_TICKING_SOUND_VOLUME 80
#define DEFAULT_TICKING_SOUND_NAME "无声音"



class SettingsScene : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScene(QWidget *parent = nullptr);
    ~SettingsScene();

    SettingsData& getSettingsData();
    void setSettingsData(SettingsData& settings);

    QString getWorkingAlarmSoundPath(QString &sound_name);
    QString getRestAlarmSoundPath(QString &sound_name);
    QString getTickingSoundPath(QString &sound_name);

    void shutup();

signals:
    void cancelBtnClicked();

    void applyBtnClicked();

private slots:
    void on_cancel_btn_clicked();
    void on_apply_btn_clicked();

    void autoBreakOnToggled(bool bChecked);
    void autoWorkingOnToggled(bool bChecked);

private:
    Ui::SettingsScene *ui;
    QSound *_applyBtnSound;
    QSound *_cancelBtnSound;

    ToggleSwitch * _ts_auto_start_break;
    ToggleSwitch * _ts_auto_start_working;

    SettingsData _settings_data;

    SoundMap _working_alarm_sounds_map;
    SoundMap _rest_alarm_sounds_map;
    SoundMap _ticking_sounds_map;

    void _loadDefaultSettingsData();
    void _initBasicSettings();
    void _initSoundsSettings();
    void _initDefaultToggleSwitches();
    void _initWorkingAlarmSoundsControlPanel();
    void _initRestAlarmSoundsControlPanel();
    void _initTickingSoundsControlPanel();

    QString _getSoundPath(SoundMap &sound_map, QString &sound_name);



};

#endif // SETTINGSSCENE_H

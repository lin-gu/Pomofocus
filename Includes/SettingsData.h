#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

#include <QString>
#include <QPair>
#include <QMap>

struct SettingsData
{
    int pomo_working_time_minutes;
    int pomo_working_time_seconds;
    int pomo_rest_time_minutes;
    int pomo_rest_time_seconds;
    int pomo_long_rest_time_minutes;
    int pomo_long_rest_time_seconds;
    int pomo_number;
    bool auto_start_break;
    bool auto_start_working;
    QString working_alarm_sound_name;
    int working_alarm_sound_volume;
    int working_alarm_sound_repeat;
    QString rest_alarm_sound_name;
    int rest_alarm_sound_volume;
    int rest_alarm_sound_repeat;
    QString ticking_sound_name;
    int ticking_sound_volume;
};

typedef QMap<int, QPair<QString, QString> > SoundMap;

#endif // SETTINGSDATA_H

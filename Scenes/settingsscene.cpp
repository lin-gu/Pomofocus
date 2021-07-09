#include "settingsscene.h"
#include "ui_settingsscene.h"
#include <QDebug>

SettingsScene::SettingsScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScene)
{
    ui->setupUi(this);
    qDebug() << "进入设置场景";

    _applyBtnSound = new QSound(":/Sounds/button_apply.wav", this);
    _cancelBtnSound = new QSound(":/Sounds/button_cancel.wav", this);

    ui->tabWidget->setCurrentIndex(0);
    //场景设置
    //设置场景标题
    this->setWindowTitle("设置");

    //加载默认设置参数
    _initBasicSettings();

    //加载声音设置
    _initSoundsSettings();

    //选定专注警报声音 就停止播放休息警报声音和时钟声音
    connect(ui->working_alarm_sound_settings, &SoundsControlPanel::soundSelected, [=](){
        ui->rest_alarm_sound_settings->player->stop();
        ui->ticking_sound_settings->player->stop();
    });

    //选定休息警报声音 就停止播放专注警报声音和时钟声音
    connect(ui->rest_alarm_sound_settings, &SoundsControlPanel::soundSelected, [=](){
        ui->working_alarm_sound_settings->player->stop();
        ui->ticking_sound_settings->player->stop();
    });

    //选定时钟声音 就停止播放专注警报声音和休息警报声音
    connect(ui->ticking_sound_settings, &SoundsControlPanel::soundSelected, [=](){
        ui->working_alarm_sound_settings->player->stop();
        ui->rest_alarm_sound_settings->player->stop();
    });
}

SettingsScene::~SettingsScene()
{
    delete ui;
}

void SettingsScene::on_cancel_btn_clicked()
{
        _cancelBtnSound->play();
        this->hide();
        shutup();
        emit this->cancelBtnClicked();
}

void SettingsScene::on_apply_btn_clicked()
{
    _applyBtnSound->play();
    this->hide();
    shutup();
    emit this->applyBtnClicked();
}


SettingsData& SettingsScene::getSettingsData()
{
    _settings_data.pomo_working_time_minutes = ui->box_pomo_working_time_setting->getMunites();
    _settings_data.pomo_working_time_seconds = ui->box_pomo_working_time_setting->getSeconds();
    _settings_data.pomo_rest_time_minutes = ui->box_pomo_rest_time_setting->getMunites();
    _settings_data.pomo_rest_time_seconds = ui->box_pomo_rest_time_setting->getSeconds();
    _settings_data.pomo_long_rest_time_minutes = ui->box_pomo_long_rest_time_setting->getMunites();
    _settings_data.pomo_long_rest_time_seconds = ui->box_pomo_long_rest_time_setting->getSeconds();
    _settings_data.pomo_number = ui->box_pomo_number_setting->getValue();
    _settings_data.working_alarm_sound_name = ui->working_alarm_sound_settings->getSoundName();
    _settings_data.working_alarm_sound_volume = ui->working_alarm_sound_settings->getVolume();
    _settings_data.working_alarm_sound_repeat = ui->working_alarm_sound_settings->getRepeat();
    _settings_data.rest_alarm_sound_name = ui->rest_alarm_sound_settings->getSoundName();
    _settings_data.rest_alarm_sound_volume = ui->rest_alarm_sound_settings->getVolume();
    _settings_data.rest_alarm_sound_repeat = ui->rest_alarm_sound_settings->getRepeat();
    _settings_data.ticking_sound_name = ui->ticking_sound_settings->getSoundName();
    _settings_data.ticking_sound_volume = ui->ticking_sound_settings->getVolume();
    return _settings_data;
}

void SettingsScene::setSettingsData(SettingsData& settings)
{
    ui->box_pomo_working_time_setting->setMunites(settings.pomo_working_time_minutes);
    ui->box_pomo_working_time_setting->setSeconds(settings.pomo_working_time_seconds);
    ui->box_pomo_rest_time_setting->setMunites(settings.pomo_rest_time_minutes);
    ui->box_pomo_rest_time_setting->setSeconds(settings.pomo_rest_time_seconds);
    ui->box_pomo_long_rest_time_setting->setMunites(settings.pomo_long_rest_time_minutes);
    ui->box_pomo_long_rest_time_setting->setSeconds(settings.pomo_long_rest_time_seconds);
    ui->box_pomo_number_setting->setValue(settings.pomo_number);
    _ts_auto_start_break->setToggle(settings.auto_start_break);
    _ts_auto_start_working->setToggle(settings.auto_start_working);
    ui->working_alarm_sound_settings->setSoundName(settings.working_alarm_sound_name);
    ui->working_alarm_sound_settings->setVolume(settings.working_alarm_sound_volume);
    ui->working_alarm_sound_settings->setRepeat(settings.working_alarm_sound_repeat);
    ui->rest_alarm_sound_settings->setSoundName(settings.rest_alarm_sound_name);
    ui->rest_alarm_sound_settings->setVolume(settings.rest_alarm_sound_volume);
    ui->rest_alarm_sound_settings->setRepeat(settings.rest_alarm_sound_repeat);
    ui->ticking_sound_settings->setSoundName(settings.ticking_sound_name);
    ui->ticking_sound_settings->setVolume(settings.ticking_sound_volume);
}


QString SettingsScene::getWorkingAlarmSoundPath(QString &sound_name)
{
    return _getSoundPath(_working_alarm_sounds_map, sound_name);
}

QString SettingsScene::getRestAlarmSoundPath(QString &sound_name)
{
    return _getSoundPath(_working_alarm_sounds_map, sound_name);
}

QString SettingsScene::getTickingSoundPath(QString &sound_name)
{
    return _getSoundPath(_ticking_sounds_map, sound_name);
}

void SettingsScene::shutup()
{
    ui->working_alarm_sound_settings->player->stop();
    ui->rest_alarm_sound_settings->player->stop();
    ui->ticking_sound_settings->player->stop();
}

void SettingsScene::_initBasicSettings()
{
    //设定番茄钟最大和最小个数
    ui->box_pomo_number_setting->setMax(MAX_POMO_NUMBER);
    ui->box_pomo_number_setting->setMin(MIN_POMO_NUMBER);

    //加载默认值到设置界面
    ui->box_pomo_working_time_setting->setMunites(DEFAULT_POMO_WORKING_TIME_MINUTES);
    ui->box_pomo_working_time_setting->setSeconds(DEFAULT_POMO_WORKING_TIME_SECONDS);
    ui->box_pomo_rest_time_setting->setMunites(DEFAULT_POMO_REST_TIME_MINUTES);
    ui->box_pomo_rest_time_setting->setSeconds(DEFAULT_POMO_REST_TIME_SECONDS);
    ui->box_pomo_long_rest_time_setting->setMunites(DEFAULT_POMO_LONG_REST_TIME_MINUTES);
    ui->box_pomo_long_rest_time_setting->setSeconds(DEFAULT_POMO_LONG_REST_TIME_SECONDS);
    ui->box_pomo_number_setting->setValue(DEFAULT_POMO_NUMBER);

    //加载默认值到设置数据
    _loadDefaultSettingsData();

    //加载默认拨动开关
    _initDefaultToggleSwitches();
}

void SettingsScene::_loadDefaultSettingsData()
{
    _settings_data.pomo_working_time_minutes = DEFAULT_POMO_WORKING_TIME_MINUTES;
    _settings_data.pomo_working_time_seconds = DEFAULT_POMO_WORKING_TIME_SECONDS;
    _settings_data.pomo_rest_time_minutes = DEFAULT_POMO_REST_TIME_MINUTES;
    _settings_data.pomo_rest_time_seconds = DEFAULT_POMO_REST_TIME_SECONDS;
    _settings_data.pomo_long_rest_time_minutes = DEFAULT_POMO_LONG_REST_TIME_MINUTES;
    _settings_data.pomo_long_rest_time_seconds = DEFAULT_POMO_LONG_REST_TIME_SECONDS;
    _settings_data.pomo_number = DEFAULT_POMO_NUMBER;
    _settings_data.auto_start_break = DEFAULT_AUTO_START_BREAK;
    _settings_data.auto_start_working = DEFAULT_AUTO_START_WORKING;
    _settings_data.working_alarm_sound_name = DEFAULT_WORKING_ALARM_SOUND_NAME;
    _settings_data.working_alarm_sound_volume = DEFAULT_WORKING_ALARM_SOUND_VOLUME;
    _settings_data.working_alarm_sound_repeat = DEFAULT_WORKING_ALARM_SOUND_REPEAT;
    _settings_data.rest_alarm_sound_name = DEFAULT_REST_ALARM_SOUND_NAME;
    _settings_data.rest_alarm_sound_volume = DEFAULT_REST_ALARM_SOUND_VOLUME;
    _settings_data.rest_alarm_sound_repeat = DEFAULT_REST_ALARM_SOUND_REPEAT;
    _settings_data.ticking_sound_name = DEFAULT_TICKING_SOUND_NAME;
    _settings_data.ticking_sound_volume = DEFAULT_TICKING_SOUND_VOLUME;
}

void SettingsScene::_initDefaultToggleSwitches()
{
    //初始化设置页面的两个拨动开关
    _ts_auto_start_break = new ToggleSwitch(this);
    _ts_auto_start_working = new ToggleSwitch(this);

    // 设置_ts_auto_start_break的状态、样式
    _ts_auto_start_break->setToggle(DEFAULT_AUTO_START_BREAK);
    _ts_auto_start_break->setCheckedColor(QColor(152,180,117));
    _ts_auto_start_break->setBackgroundColor(QColor(157,167,176));

    // 设置_ts_auto_start_working的状态、样式
    _ts_auto_start_working->setToggle(DEFAULT_AUTO_START_WORKING);
    _ts_auto_start_working->setCheckedColor(QColor(152,180,117));
    _ts_auto_start_working->setBackgroundColor(QColor(157,167,176));

    // 加载拨动开关到指定layout 居中对其
    ui->gridLayout_auto_start_break->addWidget(_ts_auto_start_break, 1, 1, Qt::AlignCenter);
    ui->gridLayout_auto_start_working->addWidget(_ts_auto_start_working, 1, 1, Qt::AlignCenter);

    // 连接拖动开关信号槽
    connect(_ts_auto_start_break, &ToggleSwitch::toggled, this, &SettingsScene::autoBreakOnToggled);
    connect(_ts_auto_start_working, &ToggleSwitch::toggled, this, &SettingsScene::autoWorkingOnToggled);
}


void SettingsScene::_initSoundsSettings()
{
    _initWorkingAlarmSoundsControlPanel();
    _initRestAlarmSoundsControlPanel();
    _initTickingSoundsControlPanel();
}

void SettingsScene::_initWorkingAlarmSoundsControlPanel()
{
    //工作闹铃声音资源
    _working_alarm_sounds_map[1] = qMakePair(QString("无声音"), QString(""));
    _working_alarm_sounds_map[2] = qMakePair(QString("军号"), QString("qrc:/Sounds/alarm_army.wav"));
    _working_alarm_sounds_map[3] = qMakePair(QString("时钟"), QString("qrc:/Sounds/alarm_clock.mp3"));
    _working_alarm_sounds_map[4] = qMakePair(QString("急促电子"), QString("qrc:/Sounds/alarm_digital_fast.mp3"));
    _working_alarm_sounds_map[5] = qMakePair(QString("缓慢电子"), QString("qrc:/Sounds/alarm_digital_slow.wav"));
    _working_alarm_sounds_map[6] = qMakePair(QString("苹果雷达"), QString("qrc:/Sounds/alarm_iphone_radar.wav"));
    _working_alarm_sounds_map[7] = qMakePair(QString("提神音乐"), QString("qrc:/Sounds/alarm_music_fast.wav"));
    _working_alarm_sounds_map[8] = qMakePair(QString("舒缓音乐"), QString("qrc:/Sounds/alarm_music_slow.wav"));

    ui->working_alarm_sound_settings->loadSounds(_working_alarm_sounds_map);

    //初始化工作闹铃声音控件的默认值
    QString default_working_alarm_sound_name = DEFAULT_WORKING_ALARM_SOUND_NAME;
    ui->working_alarm_sound_settings->setSoundName(default_working_alarm_sound_name);
    ui->working_alarm_sound_settings->setVolume(DEFAULT_WORKING_ALARM_SOUND_VOLUME);
    ui->working_alarm_sound_settings->setRepeat(DEFAULT_WORKING_ALARM_SOUND_REPEAT);
}

void SettingsScene::_initRestAlarmSoundsControlPanel()
{
    //休息闹铃声音资源
    _rest_alarm_sounds_map[1] = qMakePair(QString("无声音"), QString(""));
    _rest_alarm_sounds_map[2] = qMakePair(QString("军号"), QString("qrc:/Sounds/alarm_army.wav"));
    _rest_alarm_sounds_map[3] = qMakePair(QString("时钟"), QString("qrc:/Sounds/alarm_clock.mp3"));
    _rest_alarm_sounds_map[4] = qMakePair(QString("急促电子"), QString("qrc:/Sounds/alarm_digital_fast.mp3"));
    _rest_alarm_sounds_map[5] = qMakePair(QString("缓慢电子"), QString("qrc:/Sounds/alarm_digital_slow.wav"));
    _rest_alarm_sounds_map[6] = qMakePair(QString("苹果雷达"), QString("qrc:/Sounds/alarm_iphone_radar.wav"));
    _rest_alarm_sounds_map[7] = qMakePair(QString("提神音乐"), QString("qrc:/Sounds/alarm_music_fast.wav"));
    _rest_alarm_sounds_map[8] = qMakePair(QString("舒缓音乐"), QString("qrc:/Sounds/alarm_music_slow.wav"));

    ui->rest_alarm_sound_settings->loadSounds(_rest_alarm_sounds_map);

    //初始化休息闹铃声音控件的默认值
    QString default_rest_alarm_sound_name = DEFAULT_REST_ALARM_SOUND_NAME;
    ui->rest_alarm_sound_settings->setSoundName(default_rest_alarm_sound_name);
    ui->rest_alarm_sound_settings->setVolume(DEFAULT_REST_ALARM_SOUND_VOLUME);
    ui->rest_alarm_sound_settings->setRepeat(DEFAULT_REST_ALARM_SOUND_REPEAT);
}

void SettingsScene::_initTickingSoundsControlPanel()
{
    //时钟声音资源
    _ticking_sounds_map[1] = qMakePair(QString("无声音"), QString(""));
    _ticking_sounds_map[2] = qMakePair(QString("电子钟"), QString("qrc:/Sounds/ticking_e_clock_slow.mp3"));
    _ticking_sounds_map[3] = qMakePair(QString("挂钟"), QString("qrc:/Sounds/ticking_clock_slow.wav"));
    _ticking_sounds_map[4] = qMakePair(QString("秒表钟"), QString("qrc:/Sounds/ticking_clock_fast.mp3"));
    _ticking_sounds_map[5] = qMakePair(QString("机械挂钟"), QString("qrc:/Sounds/ticking_mechanical_wall_clock.wav"));
    _ticking_sounds_map[6] = qMakePair(QString("雨声"), QString("qrc:/Sounds/ticking_rain.mp3"));

    ui->ticking_sound_settings->loadSounds(_ticking_sounds_map);

    //初始化时钟声音控件的默认值
    QString default_ticking_sound_name = DEFAULT_TICKING_SOUND_NAME;
    ui->ticking_sound_settings->setSoundName(default_ticking_sound_name);
    ui->ticking_sound_settings->setVolume(DEFAULT_TICKING_SOUND_VOLUME);
    ui->ticking_sound_settings->hideRepeatLayout();
}

QString SettingsScene::_getSoundPath(SoundMap &sound_map, QString &sound_name)
{
    for(int i=0; i<sound_map.size(); ++i)
    {
        if(sound_map[i].first == sound_name)
            return sound_map[i].second;
    }
    return "";
}

void SettingsScene::autoBreakOnToggled(bool bChecked)
{
    _settings_data.auto_start_break = bChecked;
}

void SettingsScene::autoWorkingOnToggled(bool bChecked)
{
    _settings_data.auto_start_working =  bChecked;
}



#include "widget.h"
#include "ui_widget.h"
#include "switchcontrol.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QSound>
#include <QTimer>
#include <QDebug>
#include <QMediaPlaylist>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //设置图标
    this->setWindowIcon(QIcon(":/Icons/pomo_button_1.png"));

    init();

    // 连接拖动开关信号槽
    connect(_ts_auto_start_break, &SwitchControl::toggled, this, &Widget::autoBreakOnToggled);
    connect(_ts_auto_start_working, &SwitchControl::toggled, this, &Widget::autoWorkingOnToggled);

    //初始化界面按钮音效
    QSound *applyBtnSound = new QSound(":/Sounds/button_apply.wav", this);
    QSound *cancelBtnSound = new QSound(":/Sounds/button_cancel.wav", this);
    QSound *nextBtnSound = new QSound(":/Sounds/button_next.wav", this);
    QSound *pauseBtnSound = new QSound(":/Sounds/button_pause.wav", this);
    QSound *startBtnSound = new QSound(":/Sounds/button_start.wav", this);
    QSound *settingsBtnSound = new QSound(":/Sounds/button_settings.wav", this);


    //默认进入"番茄时钟"页面
    ui->stackedWidget->setCurrentIndex(0);

    //隐藏应用和取消按钮
    ui->settings_apply_btn->hide();
    ui->settings_cancel_btn->hide();

    //点击"番茄时钟"按钮, 切换到"番茄时钟"页面
    connect(ui->tool_btn_clock, &QToolButton::clicked, [=](){
        _shutup();
    });

    //点击"设置"按钮, 切换到"设置"页面
    connect(ui->tool_btn_settings, &QToolButton::clicked, [=](){
        //播放设置按键点击音
        settingsBtnSound->play();
        //停止播放一切声音
        _shutup();

        ui->stackedWidget->setCurrentIndex(1);
        _store_current_settings();
        //隐藏番茄钟按钮
        ui->tool_btn_clock->hide();
        ui->settings_apply_btn->show();
        ui->settings_cancel_btn->show();
    });

    //点击"应用"按钮，切换到"番茄时钟"页面 加载setting的数据
    connect(ui->settings_apply_btn, &QToolButton::clicked, [=](){
        applyBtnSound->play();
       //索引0就是"番茄时钟"页面
       ui->stackedWidget->setCurrentIndex(0);
       //从"设置"页面获取基本数据
       loadValuesFromSettings();
       _current_stage = 0;
       _is_working_timer = true;
       reloadPromoClock();
       ui->tool_btn_clock->show();
       ui->settings_apply_btn->hide();
       ui->settings_cancel_btn->hide();

       //停止播放一切音乐
       _shutup();
    });

    //点击"取消"按钮，切换到"番茄时钟"页面 加载原先数据
    connect(ui->settings_cancel_btn, &QToolButton::clicked, [=](){
        cancelBtnSound->play();
       //索引0就是"番茄时钟"页面
       ui->stackedWidget->setCurrentIndex(0);
       //加载原先页面的基本数据
       _load_backup_settings();
       _update_settings_as_backup();
       ui->tool_btn_clock->show();
       ui->settings_apply_btn->hide();
       ui->settings_cancel_btn->hide();

       //停止播放一切音乐
       _shutup();
    });


    QTimer *timer = new QTimer(this);

    //点击"开始"按钮 计时器开始计时
    connect(ui->tool_btn_start, &QToolButton::clicked, [=](){
        startBtnSound->play();
        timer->start(DEFAULT_TIMER_INTERVAL);
        if(_is_working_timer)
            _playTickingSound();
    });

    //点击"暂停"按钮 计时器开始计时
    connect(ui->tool_btn_pause, &QToolButton::clicked, [=](){
        pauseBtnSound->play();
        timer->stop();
        _player->stop();
    });

    //点击"快进"按钮 进入下一个阶段
    connect(ui->tool_btn_next, &QToolButton::clicked, [=](){
        //播放按键声音
        nextBtnSound->play();
        //停止播放一切音乐
        _shutup();
        _moveToNextStage();
        //如果阶段在用户预设范围内就重新加载番茄钟 超出范围就整体停止番茄钟
        if(_current_stage  < __pomo_number * 2)
        {
            reloadPromoClock();
            //根据设置的两个拖动开关决定是不是在进入下个阶段的时候停止计时器或者自动开始计时
            if((_is_working_timer && !__auto_start_working) || (!_is_working_timer && !__auto_start_break))
              timer->stop();
            if((_is_working_timer && __auto_start_working) || (!_is_working_timer && __auto_start_break))
            {
              timer->start();
              if(_is_working_timer)
                  _playTickingSound();
            }
        }
        else
        {
            timer->stop();
            _promofocusFinished();

        }
    });

    //每次计时间隔都减小_current_stage_timer当前阶段计时器的值
    connect(timer, &QTimer::timeout, [=](){
        //如果当前阶段计时器走完current_timer_value==0 就要切换到下一个阶段
        if(_current_stage_timer == 0)
        {
            //停止播放一切音乐
            _shutup();
            _moveToNextStage();
            reloadPromoClock();
            if(!_is_working_timer)
                _playAlarmSound();

            //根据设置的两个拖动开关决定是不是在进入下个阶段的时候停止计时器或者自动开始计时
            if((_is_working_timer && !__auto_start_working) || (!_is_working_timer && !__auto_start_break))
              timer->stop();
            if((_is_working_timer && __auto_start_working) || (!_is_working_timer && __auto_start_break))
            {
              timer->start();
              if(_is_working_timer)
                  _playTickingSound();
            }
        }

        //如果当前番茄阶段超出用户预设阶段就停止番茄钟 打印提示信息并停止计时
        if(_current_pomo_stage > __pomo_number)
        {
            timer->stop();
            _promofocusFinished();

        }
        else
        {
            //倒计时 _current_stage_timer当前阶段计时器自减 他的数值是以秒为单位的
            _bar->setValue(_current_stage_timer);
            _bar->update();
            _current_stage_timer --;
        }
    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    //初始化圆形进度条
    _bar=new RoundProgressBar(this);
    //初始化设置页面的两个拨动开关
    _ts_auto_start_break = new SwitchControl(this);
    _ts_auto_start_working = new SwitchControl(this);

    //初始化播放器
    _player = new QMediaPlayer;

    loadDefaultSettings();
    //加载默认拨动开关
    _loadDefaultToggleSwitches();
    //初始化闹铃声音控件
    _initAlarmSoundControlPanel();

    //第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;

    reloadPromoClock();

    //加载圆形进度条到指定位置
    ui->gridLayout_round_progress_bar->addWidget(_bar,0, Qt::AlignHCenter|Qt::AlignVCenter);


    // 加载拨动开关到指定layout 居中对其
    ui->gridLayout_auto_start_break->addWidget(_ts_auto_start_break, 1, 1, Qt::AlignCenter);
    ui->gridLayout_auto_start_working->addWidget(_ts_auto_start_working, 1, 1, Qt::AlignCenter);
}

void Widget::loadDefaultSettings()
{
    //加载默认值到设置界面
    ui->box_pomo_working_time_setting->setMunites(DEFAULT_PROMO_WORKING_TIME_MINUTES);
    ui->box_pomo_working_time_setting->setSeconds(DEFAULT_PROMO_WORKING_TIME_SECONDS);
    ui->box_pomo_rest_time_setting->setMunites(DEFAULT_PROMO_REST_TIME_MINUTES);
    ui->box_pomo_rest_time_setting->setSeconds(DEFAULT_PROMO_REST_TIME_SECONDS);
    ui->box_pomo_long_rest_time_setting->setMunites(DEFAULT_PROMO_LONG_REST_TIME_MINUTES);
    ui->box_pomo_long_rest_time_setting->setSeconds(DEFAULT_PROMO_LONG_REST_TIME_SECONDS);
    ui->box_pomo_number_setting->setValue(DEFAULT_PROMO_NUMBER);

}

void Widget::_loadDefaultToggleSwitches()
{
    // 设置_ts_auto_start_break的状态、样式
    _ts_auto_start_break->setToggle(DEFAULT_AUTO_START_BREAK);
    _ts_auto_start_break->setCheckedColor(QColor(152,180,117));
    _ts_auto_start_break->setBackgroundColor(QColor(157,167,176));

    // 设置_ts_auto_start_working的状态、样式
    _ts_auto_start_working->setToggle(DEFAULT_AUTO_START_WORKING);
    _ts_auto_start_working->setCheckedColor(QColor(152,180,117));
    _ts_auto_start_working->setBackgroundColor(QColor(157,167,176));

}

void Widget::_initAlarmSoundControlPanel()
{
    //闹铃声音资源
    _alarm_sounds_map[1] = qMakePair(QString("无声音"), QString(""));
    _alarm_sounds_map[2] = qMakePair(QString("军号"), QString("qrc:/Sounds/alarm_army.wav"));
    _alarm_sounds_map[3] = qMakePair(QString("时钟"), QString("qrc:/Sounds/alarm_clock.mp3"));
    _alarm_sounds_map[4] = qMakePair(QString("急促电子"), QString("qrc:/Sounds/alarm_digital_fast.mp3"));
    _alarm_sounds_map[5] = qMakePair(QString("缓慢电子"), QString("qrc:/Sounds/alarm_digital_slow.wav"));
    _alarm_sounds_map[6] = qMakePair(QString("苹果雷达"), QString("qrc:/Sounds/alarm_iphone_radar.wav"));
    _alarm_sounds_map[7] = qMakePair(QString("提神音乐"), QString("qrc:/Sounds/alarm_music_fast.wav"));
    _alarm_sounds_map[8] = qMakePair(QString("舒缓音乐"), QString("qrc:/Sounds/alarm_music_slow.wav"));

    ui->alarm_sound_settings->loadSounds(_alarm_sounds_map);

    //初始化闹铃声音控件的默认值
    QString default_alarm_sound_name = DEFAULT_ALARM_SOUND_NAME;
    ui->alarm_sound_settings->setSoundName(default_alarm_sound_name);
    ui->alarm_sound_settings->setVolume(DEFAULT_ALARM_SOUND_VOLUME);
    ui->alarm_sound_settings->setRepeat(DEFAULT_ALARM_SOUND_REPEAT);


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

void Widget::loadValuesFromSettings()
{
    //从设置界面读取用户设置的值
    __pomo_working_time_minutes = ui->box_pomo_working_time_setting->getMunites();
    __pomo_working_time_seconds = ui->box_pomo_working_time_setting->getSeconds();
    __pomo_rest_time_minutes = ui->box_pomo_rest_time_setting->getMunites();
    __pomo_rest_time_seconds = ui->box_pomo_rest_time_setting->getSeconds();
    __pomo_long_rest_time_minutes = ui->box_pomo_long_rest_time_setting->getMunites();
    __pomo_long_rest_time_seconds = ui->box_pomo_long_rest_time_setting->getSeconds();
    __pomo_number = ui->box_pomo_number_setting->getValue();
    __alarm_sound_name = ui->alarm_sound_settings->getSoundName();
    __alarm_sound_volume = ui->alarm_sound_settings->getVolume();
    __alarm_sound_repeat = ui->alarm_sound_settings->getRepeat();
    __ticking_sound_name = ui->ticking_sound_settings->getSoundName();
    __ticking_sound_volume = ui->ticking_sound_settings->getVolume();
}

void Widget::reloadPromoClock()
{
    _resetCurrentPromoStage();
    _resetCurrentMaxTime();
    _resetCurrentTimerValue();
    _resetRoundProgressBar();
}

void Widget::_resetCurrentTimerValue()
{
        _current_stage_timer = _current_max_time;
}

void Widget::_resetCurrentMaxTime()
{
    _current_max_time = _is_working_timer ? (__pomo_working_time_minutes * 60 + __pomo_working_time_seconds) :
                        (_current_stage + 1 == __pomo_number * 2) ? (__pomo_long_rest_time_minutes * 60 + __pomo_long_rest_time_seconds) :
                        (__pomo_rest_time_minutes * 60 + __pomo_rest_time_seconds);
}

void Widget::_resetCurrentPromoStage()
{
    _current_pomo_stage = _current_stage / 2 + 1;
}

void Widget::_resetRoundProgressBar()
{
    //*********************** RoundProgressBar ************************
    _bar->setdefault(90, false);
    _bar->setOutterBarWidth(20);
    _bar->setInnerBarWidth(20);
    _bar->setControlFlags(RoundProgressBar::all);
    _bar->setInnerDefaultTextStyle(RoundProgressBar::timeFloat);
    _bar->setRange(0, _current_max_time);
    _bar->setValue(_current_max_time);
    if(_current_pomo_stage <= __pomo_number)
    {
        if(_is_working_timer)
        {
            //设置外圈颜色
            _bar->setOutterColor(QColor(233,248,248));
            //设置默认渐变色
            _bar->setInnerColor(QColor(95,113,127),QColor(157,167,176));//灰色
            _bar->setDefaultTextColor(QColor(95,113,127));
            //_bar->setInnerColor(QColor(49, 177, 190),QColor(133, 243, 244));//青色
            _bar->setToptitle(QString("%1/%2").arg(_current_pomo_stage).arg(__pomo_number));
            _bar->setSubtitle("专注");
        }
        else
        {
            _bar->setOutterColor(QColor(250,250,240));
            //_bar->setInnerColor(QColor(255,190,57),QColor(255,230,129)); //黄色
            _bar->setInnerColor(QColor(152,180,117),QColor(192,220,147)); //绿色
            _bar->setDefaultTextColor(QColor(152,180,117));
            if(_current_stage + 1 == __pomo_number * 2)
                _bar->setSubtitle("长休息");
            else
                _bar->setSubtitle("休息");
        }
    }
}

void Widget::_promofocusFinished()
{
    _shutup();
    QMessageBox::information(this, "完成番茄计划", "太棒了！你完成了整整" + QString::number(__pomo_number) + "个番茄的计划");
    loadValuesFromSettings();
    //第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;
    reloadPromoClock();
}

void Widget::_moveToNextStage()
{
    //阶段计数器自加
    ++_current_stage;
    //切换工作/休息状态
    _is_working_timer = !_is_working_timer;
}

void Widget::_store_current_settings()
{
    _backup_pomo_working_time_minutes = __pomo_working_time_minutes;
    _backup_pomo_working_time_seconds = __pomo_working_time_seconds;
    _backup_pomo_rest_time_minutes = __pomo_rest_time_minutes;
    _backup_pomo_rest_time_seconds = __pomo_rest_time_seconds;
    _backup_pomo_long_rest_time_minutes = __pomo_long_rest_time_minutes;
    _backup_pomo_long_rest_time_seconds = __pomo_long_rest_time_seconds;
    _backup_pomo_number = __pomo_number;
    _backup_auto_start_break = __auto_start_break;
    _backup_auto_start_working = __auto_start_working;
    _backup_alarm_sound_name = __alarm_sound_name;
    _backup_alarm_sound_volume = __alarm_sound_volume;
    _backup_alarm_sound_repeat = __alarm_sound_repeat;
    _backup_ticking_sound_name = __ticking_sound_name;
    _backup_ticking_sound_volume = __ticking_sound_volume;
}

void Widget::_load_backup_settings()
{
    __pomo_working_time_minutes = _backup_pomo_working_time_minutes;
    __pomo_working_time_seconds = _backup_pomo_working_time_seconds;
    __pomo_rest_time_minutes = _backup_pomo_rest_time_minutes;
    __pomo_rest_time_seconds = _backup_pomo_rest_time_seconds;
    __pomo_long_rest_time_minutes = _backup_pomo_long_rest_time_minutes;
    __pomo_long_rest_time_seconds = _backup_pomo_long_rest_time_seconds;
    __pomo_number = _backup_pomo_number;
    __auto_start_break = _backup_auto_start_break;
    __auto_start_working = _backup_auto_start_working;
    __alarm_sound_name = _backup_alarm_sound_name;
    __alarm_sound_volume = _backup_alarm_sound_volume;
    __alarm_sound_repeat = _backup_alarm_sound_repeat;
    __ticking_sound_name = _backup_ticking_sound_name;
    __ticking_sound_volume = _backup_ticking_sound_volume;
}

void Widget::_update_settings_as_backup()
{
    ui->box_pomo_working_time_setting->setMunites(_backup_pomo_working_time_minutes);
    ui->box_pomo_working_time_setting->setSeconds(_backup_pomo_working_time_seconds);
    ui->box_pomo_rest_time_setting->setMunites(_backup_pomo_rest_time_minutes);
    ui->box_pomo_rest_time_setting->setSeconds(_backup_pomo_rest_time_seconds);
    ui->box_pomo_long_rest_time_setting->setMunites(_backup_pomo_long_rest_time_minutes);
    ui->box_pomo_long_rest_time_setting->setSeconds(_backup_pomo_long_rest_time_seconds);
    ui->box_pomo_number_setting->setValue(_backup_pomo_number);
    _ts_auto_start_break->setToggle(_backup_auto_start_break);
    _ts_auto_start_working->setToggle(_backup_auto_start_working);
    ui->alarm_sound_settings->setSoundName(_backup_alarm_sound_name);
    ui->alarm_sound_settings->setVolume(_backup_alarm_sound_volume);
    ui->alarm_sound_settings->setRepeat(_backup_alarm_sound_repeat);
    ui->ticking_sound_settings->setSoundName(_backup_ticking_sound_name);
    ui->ticking_sound_settings->setVolume(_backup_ticking_sound_volume);
}

void Widget::autoBreakOnToggled(bool bChecked)
{
    __auto_start_break = bChecked;
}

void Widget::autoWorkingOnToggled(bool bChecked)
{
    __auto_start_working =  bChecked;
}

QString Widget::getAlarmSoundPath(QString &sound_name)
{
    return _getSoundPath(_alarm_sounds_map, sound_name);
}

QString Widget::getTickingSoundPath(QString &sound_name)
{
    return _getSoundPath(_ticking_sounds_map, sound_name);
}

QString Widget::_getSoundPath(SoundMap &sound_map, QString &sound_name)
{
    for(int i=0; i<sound_map.size(); ++i)
    {
        if(sound_map[i].first == sound_name)
            return sound_map[i].second;
    }
    return "";
}

void Widget::_playAlarmSound()
{
    _player->stop();
    QString alarm_sound_path = getAlarmSoundPath(__alarm_sound_name);
    if(alarm_sound_path != "")
    {

        QMediaPlaylist *play_list = new QMediaPlaylist();
        for(int i=1; i<=__alarm_sound_repeat; ++i)
        {
            play_list->addMedia(QUrl(alarm_sound_path));
        }
        play_list->setPlaybackMode(QMediaPlaylist::Sequential);
        _player->setPlaylist(play_list);
        _player->setVolume(__alarm_sound_volume);
        _player->play();
    }
}

void Widget::_playTickingSound()
{
    _player->stop();
    QString ticking_sound_path = getTickingSoundPath(__ticking_sound_name);
    if(ticking_sound_path != "")
    {
        QMediaPlaylist *play_list = new QMediaPlaylist();
        play_list->addMedia(QUrl(ticking_sound_path));
        play_list->setPlaybackMode(QMediaPlaylist::Loop);
        _player->setPlaylist(play_list);
        _player->setVolume(__ticking_sound_volume);
        _player->play();
    }
}


void Widget::_shutup()
{
    //停止播放
    _player->stop();
    //停止闹铃音乐控件的播放
    ui->alarm_sound_settings->player->stop();
    //停止时钟音乐控件的播放
    ui->ticking_sound_settings->player->stop();
}





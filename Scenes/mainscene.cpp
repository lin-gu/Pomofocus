#include "Scenes/mainscene.h"
#include "ui_mainscene.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QSound>
#include <QTimer>
#include <QDebug>
#include <QMediaPlaylist>

MainScene::MainScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    resize(475, 760);
    init();

    //初始化界面按钮音效
    QSound *nextBtnSound = new QSound(":/Sounds/button_next.wav", this);
    QSound *pauseBtnSound = new QSound(":/Sounds/button_pause.wav", this);
    QSound *startBtnSound = new QSound(":/Sounds/button_start.wav", this);

    _settingsBtnSound = new QSound(":/Sounds/button_settings.wav", this);

    //默认进入"番茄时钟"页面
    ui->stackedWidget->setCurrentIndex(0);

    //初始化计时器
    _timer = new QTimer(this);

    //点击"番茄时钟"按钮, 切换到"番茄时钟"页面
    connect(ui->tool_btn_clock, &QToolButton::clicked, [=](){
        _shutup();
    });

    //点击"应用"按钮，切换到"番茄时钟"页面 加载setting的数据
    connect(_settingsScene, &SettingsScene::applyBtnClicked, [=](){
       //索引0就是"番茄时钟"页面
       this->show();
       ui->stackedWidget->setCurrentIndex(0);
       //从"设置"页面获取基本数据
       loadValuesFromSettings();
       _current_stage = 0;
       _is_working_timer = true;
       reloadPromoClock();
       ui->tool_btn_clock->show();

       //停止闹铃音乐控件的播放
       ui->alarm_sound_settings->player->stop();
       //停止时钟音乐控件的播放
       ui->ticking_sound_settings->player->stop();
    });

    //点击"取消"按钮，切换到"番茄时钟"页面 加载原先数据
    connect(_settingsScene, &SettingsScene::cancelBtnClicked, [=](){
       //索引0就是"番茄时钟"页面
       this->show();
       ui->stackedWidget->setCurrentIndex(0);
       //加载原先页面的基本数据到当前
       _load_backup_settings();
       //设置界面由于被取消 要加载为原先备份的值
       _update_settings_as_backup();
       //显示番茄钟按钮 隐藏应用和取消按钮
       ui->tool_btn_clock->show();

       //停止闹铃音乐控件的播放
       ui->alarm_sound_settings->player->stop();
       //停止时钟音乐控件的播放
       ui->ticking_sound_settings->player->stop();

       //如果原来在运行，现在继续运行，原来的音频继续播放
       if(_backup_timer_is_active)
       {
           _startTimer();
           _player->play();
       }
    });


    //点击"开始"按钮 计时器开始计时 变成"暂停"按钮
    //点击"暂停"按钮 计时器停止计时 变成"开始"按钮
    connect(ui->tool_btn_start_pause, &QToolButton::clicked, [=](){
        if(!_timer->isActive())
        {
            startBtnSound->play();
            _startTimer();
            if(_is_working_timer && _player->state() != QMediaPlayer::State::PlayingState)
                _playTickingSound();
        }
        else
        {
            pauseBtnSound->play();
            _stopTimer();
            _player->pause();
        }

    });


    //点击"快进"按钮 进入下一个阶段
    connect(ui->tool_btn_next, &QToolButton::clicked, [=](){
        //播放按键声音
        nextBtnSound->play();
        //停止播放声音
        _player->stop();
        _moveToNextStage();
        //如果阶段在用户预设范围内就重新加载番茄钟 超出范围就整体停止番茄钟
        if(_current_stage  < __settings_data.pomo_number * 2)
        {
            //重新加载番茄钟
            reloadPromoClock();
            //如果是休息时间 播放工作警告声音
            //如果是工作时间 播放休息警告声音
            if(!_is_working_timer)
                _playWorkingAlarmSound();
            else
                _playRestAlarmSound();

            //根据设置的两个拖动开关决定是不是在进入下个阶段的时候停止计时器或者自动开始计时
            if((_is_working_timer && !__settings_data.auto_start_working) ||
              (!_is_working_timer && !__settings_data.auto_start_break))
                _stopTimer();
            if((_is_working_timer && __settings_data.auto_start_working) ||
              (!_is_working_timer && __settings_data.auto_start_break))
                _startTimer();
        }
        else
        {
            _stopTimer();
            _pomofocusFinished();

        }
    });

    //每次计时间隔都减小_current_stage_timer当前阶段计时器的值
    connect(_timer, &QTimer::timeout, [=](){
        //如果当前阶段计时器走完current_timer_value==0 就要切换到下一个阶段
        if(_current_stage_timer == 0)
        {
            //停止播放一切音乐
            _shutup();
            _moveToNextStage();
            reloadPromoClock();
            if(!_is_working_timer)
                _playWorkingAlarmSound();
            else
                _playRestAlarmSound();

            //根据设置的两个拖动开关决定是不是在进入下个阶段的时候停止计时器或者自动开始计时
            if((_is_working_timer && !__settings_data.auto_start_working) ||
              (!_is_working_timer && !__settings_data.auto_start_break))
              _stopTimer();
            if((_is_working_timer && __settings_data.auto_start_working) ||
              (!_is_working_timer && __settings_data.auto_start_break))
              _startTimer();
        }

        //如果当前番茄阶段超出用户预设阶段就停止番茄钟 打印提示信息并停止计时
        if(_current_pomo_stage > __settings_data.pomo_number)
        {
            _stopTimer();
            _pomofocusFinished();
        }
        else
        {
            //倒计时 _current_stage_timer当前阶段计时器自减 他的数值是以秒为单位的
            _bar->setValue(_current_stage_timer);
            _bar->update();
            _current_stage_timer --;
        }
    });

    //播放器状态如果发生改变进入stop的时候 要判断一下是否继续播放
    connect(_player, &QMediaPlayer::stateChanged, [=](QMediaPlayer::State state) {
        if(state == QMediaPlayer::State::StoppedState)
        {
            if(_timer->isActive())
            {
                //专注时间先播放休息警告声音 再连续播放ticking声音
                if(_is_working_timer)
                {
                    //判断一下是否有音乐被选择了 并且用户选择了连续播放次数大于1 当前重复播放次数没有用完
                    if(__settings_data.rest_alarm_sound_name != DEFAULT_REST_ALARM_SOUND_NAME &&
                       __settings_data.rest_alarm_sound_repeat > 1 &&
                       _current_rest_alarm_sound_repeat >= 0)
                    {
                        _player->play();
                        _current_rest_alarm_sound_repeat --;
                    }
                    else
                    {
                        if(_current_pomo_stage <= __settings_data.pomo_number)
                            _playTickingSound();
                    }
                }
                //休息时间要判断一下是否有音乐被选择了 并且用户选择了连续播放次数大于1 当前重复播放次数没有用完
                else
                {
                    if(__settings_data.working_alarm_sound_name != DEFAULT_WORKING_ALARM_SOUND_NAME &&
                       __settings_data.working_alarm_sound_repeat > 1 &&
                       _current_working_alarm_sound_repeat >= 0)
                    {
                        _player->play();
                        _current_working_alarm_sound_repeat --;
                    }
                }
            }
        }
    });

    connect(_player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status){
        _player_status = status;
    });

}

MainScene::~MainScene()
{
    delete ui;
}

void MainScene::init()
{
    //设置图标
    this->setWindowIcon(QIcon(":/Icons/pomo_button_1.png"));

    //初始化设置界面
    _settingsScene = new SettingsScene();
    //初始化内存设置数据
    __settings_data = _settingsScene->getSettingsData();

    //初始化圆形进度条
    _bar=new RoundProgressBar(this);
    //第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;
    //重新加载番茄钟
    reloadPromoClock();
    //加载圆形进度条到指定位置
    ui->gridLayout_round_progress_bar->addWidget(_bar,0, Qt::AlignHCenter|Qt::AlignVCenter);

    //初始化播放器
    _player = new QMediaPlayer();

    //初始化透明度滑动条
    ui->slider_window_opacity->setValue(_current_window_opacity);
    ui->slider_window_opacity->setStyleSheet("QSlider::handle:horizontal{width:14px;background-color:rgb(255,255,255);margin:-6px 0px -6px 0px;border-radius:7px;}"
                                             "QSlider::groove:horizontal{height:3px;background-color:rgb(219,219,219);}"
                                             "QSlider::add-page:horizontal{background-color:rgb(219,219,219);}"
                                             "QSlider::sub-page:horizontal{background-color:rgb(157,167,176);}");
}


void MainScene::loadValuesFromSettings()
{
    //从设置界面读取用户设置的值
    __settings_data = _settingsScene->getSettingsData();

     //从设置界面读取用户设置的闹铃声音重复计数器
    _current_working_alarm_sound_repeat = __settings_data.working_alarm_sound_repeat;
    _current_rest_alarm_sound_repeat = __settings_data.rest_alarm_sound_repeat;
}

void MainScene::reloadPromoClock()
{
    _resetCurrentPromoStage();
    _resetCurrentMaxTime();
    _resetCurrentTimerValue();
    _resetRoundProgressBar();
}

void MainScene::_resetCurrentTimerValue()
{
    _current_stage_timer = _current_max_time;
}

void MainScene::_resetCurrentMaxTime()
{
    _current_max_time = _is_working_timer ? (__settings_data.pomo_working_time_minutes * 60 + __settings_data.pomo_working_time_seconds) :
                        (_current_stage + 1 == __settings_data.pomo_number * 2) ? (__settings_data.pomo_long_rest_time_minutes * 60 + __settings_data.pomo_long_rest_time_seconds) :
                        (__settings_data.pomo_rest_time_minutes * 60 + __settings_data.pomo_rest_time_seconds);
}

void MainScene::_resetCurrentPromoStage()
{
    _current_pomo_stage = _current_stage / 2 + 1;
}

void MainScene::_resetRoundProgressBar()
{
    //*********************** RoundProgressBar ************************
    _bar->setdefault(90, false);
    _bar->setOutterBarWidth(20);
    _bar->setInnerBarWidth(20);
    _bar->setControlFlags(RoundProgressBar::all);
    _bar->setInnerDefaultTextStyle(RoundProgressBar::timeFloat);
    _bar->setRange(0, _current_max_time);
    _bar->setValue(_current_max_time);
    if(_current_pomo_stage <= __settings_data.pomo_number)
    {
        if(_is_working_timer)
        {
            //设置外圈颜色
            _bar->setOutterColor(QColor(233,248,248));
            //设置默认渐变色
            _bar->setInnerColor(QColor(95,113,127),QColor(157,167,176));//灰色
            _bar->setDefaultTextColor(QColor(95,113,127));
            //_bar->setInnerColor(QColor(49, 177, 190),QColor(133, 243, 244));//青色
            _bar->setToptitle(QString("%1/%2").arg(_current_pomo_stage).arg(__settings_data.pomo_number));
            _bar->setSubtitle("专注");
        }
        else
        {
            _bar->setOutterColor(QColor(250,250,240));
            //_bar->setInnerColor(QColor(255,190,57),QColor(255,230,129)); //黄色
            _bar->setInnerColor(QColor(152,180,117),QColor(192,220,147)); //绿色
            _bar->setDefaultTextColor(QColor(152,180,117));
            if(_current_stage + 1 == __settings_data.pomo_number * 2)
                _bar->setSubtitle("长休息");
            else
                _bar->setSubtitle("休息");
        }
    }
}

void MainScene::_pomofocusFinished()
{
    //停止计时器
    _timer->stop();
    //停止播放声音
    _player->stop();
    //提升完成对话框
    QMessageBox::information(this, "完成番茄计划", "太棒了！你完成了整整" + QString::number(__settings_data.pomo_number) + "个番茄的计划");
    //重新加载设置数据
    loadValuesFromSettings();
    //重置为第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;
    reloadPromoClock();
}

void MainScene::_moveToNextStage()
{
    //阶段计数器自加
    ++_current_stage;
    //切换工作/休息状态
    _is_working_timer = !_is_working_timer;
    //从新加载工作和休息闹铃计数器的值
    _current_working_alarm_sound_repeat = __settings_data.working_alarm_sound_repeat;
    _current_rest_alarm_sound_repeat = __settings_data.rest_alarm_sound_repeat;
}

void MainScene::_store_current_settings()
{
    _settings_data_snapshot = __settings_data;
    _backup_timer_is_active = _timer->isActive();
    _backup_current_working_alarm_sound_repeat = _current_working_alarm_sound_repeat;
    _backup_current_rest_alarm_sound_repeat = _current_rest_alarm_sound_repeat;

}

void MainScene::_load_backup_settings()
{
    __settings_data = _settings_data_snapshot;

    _current_working_alarm_sound_repeat = _backup_current_working_alarm_sound_repeat;
    _current_rest_alarm_sound_repeat = _backup_current_rest_alarm_sound_repeat;
}

void MainScene::_update_settings_as_backup()
{
    _settingsScene->setSettingsData(_settings_data_snapshot);
}

void MainScene::_playWorkingAlarmSound()
{
    //如果加载音频文件成功 就播放音频
    if(_loadWorkingAlarmSound())
    {
        _player->setVolume(__settings_data.working_alarm_sound_volume);
        if(_player->state() != QMediaPlayer::PlayingState)
        {
          _player->play();
          _current_working_alarm_sound_repeat --;
        }
    }
}

bool MainScene::_loadWorkingAlarmSound()
{
    _working_alarm_sound_path = _settingsScene->getWorkingAlarmSoundPath(__settings_data.working_alarm_sound_name);
    //如果player当前加载的音频和用户设置的不一样 就尝试加载
    if(_player->media().canonicalUrl().toString() != _working_alarm_sound_path)
    {
        //如果用户设置了音频就要播放
        if(_working_alarm_sound_path != "")
        {
            _player->setMedia(QUrl(_working_alarm_sound_path));
            return true;
        }
        //如果用户没有设置音频 就不要播放
        else
        {
            return false;
        }
    }
    //如果当前已经加载的音频和用户设置的一样 也要播放 但是没必要重新加载
    return true;
}

void MainScene::_playRestAlarmSound()
{
    //如果加载音频文件成功 就播放音频
    if(_loadRestAlarmSound())
    {
        _player->setVolume(__settings_data.rest_alarm_sound_volume);
        if(_player->state() != QMediaPlayer::PlayingState)
        {
          _player->play();
          _current_rest_alarm_sound_repeat --;
        }
    }
}

bool MainScene::_loadRestAlarmSound()
{
    _rest_alarm_sound_path = _settingsScene->getWorkingAlarmSoundPath(__settings_data.rest_alarm_sound_name);
    //如果player当前加载的音频和用户设置的不一样 就尝试加载
    if(_player->media().canonicalUrl().toString() != _rest_alarm_sound_path)
    {
        //如果用户设置了音频就要播放
        if(_rest_alarm_sound_path != "")
        {
            _player->setMedia(QUrl(_rest_alarm_sound_path));
            return true;
        }
        //如果用户没有设置音频 就不要播放
        else
        {
            return false;
        }
    }
    //如果当前已经加载的音频和用户设置的一样 也要播放 但是没必要重新加载
    return true;
}

void MainScene::_playTickingSound()
{
    if(_loadTickingSound())
    {
        _player->setVolume(__settings_data.ticking_sound_volume);
        if(_player->state() != QMediaPlayer::PlayingState)
          _player->play();
    }
}

bool MainScene::_loadTickingSound()
{
    _ticking_sound_path = _settingsScene->getTickingSoundPath(__settings_data.ticking_sound_name);
    if(_player->media().canonicalUrl().toString() != _ticking_sound_path)
    {
        if(_ticking_sound_path != "")
        {
            _player->setMedia(QUrl(_ticking_sound_path));
            return true;
        }
        else
            return false;
    }
    return true;
}

void MainScene::_shutup()
{
    //停止播放
    _player->stop();
    //停止闹铃音乐控件的播放
    ui->alarm_sound_settings->player->stop();
    //停止时钟音乐控件的播放
    ui->ticking_sound_settings->player->stop();
}


void MainScene::_startTimer()
{
    _timer->start(DEFAULT_TIMER_INTERVAL);
    ui->tool_btn_start_pause->setIcon(QIcon(":/Icons/pause_button.png"));

    ui->tool_btn_start_pause->setText("暂停");
     ui->tool_btn_start_pause->setShortcut(Qt::Key_Space);

}
void MainScene::_stopTimer()
{
    _timer->stop();
    ui->tool_btn_start_pause->setIcon(QIcon(":/Icons/play_button_1.png"));

    ui->tool_btn_start_pause->setText("开始");
     ui->tool_btn_start_pause->setShortcut(Qt::Key_Space);

}

//点击"设置"按钮, 切换到"设置"页面
void MainScene::on_tool_btn_settings_clicked()
{
    //在切换到_settingsScene场景之前 设置_settingsScene场景的位置为当前主场景的位置
    _settingsScene->setGeometry(this->geometry());

    //停止播放一切声音
    _shutup();
    //播放设置按键点击音
    _settingsBtnSound->play();

    //隐藏当前主场景
    this->hide();
    //显示settingsScene场景
    _settingsScene->show();

    _store_current_settings();
    //隐藏番茄钟按钮 显示应用和取消按钮
    ui->tool_btn_clock->hide();

    //暂停计时
    _stopTimer();
    //暂停声音
    _player->pause();
}

void MainScene::on_slider_window_opacity_valueChanged(int value)
{
    _current_window_opacity = value;
    setWindowOpacity(qreal(qreal(value)/100));
}

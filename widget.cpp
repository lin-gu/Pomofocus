#include "widget.h"
#include "ui_widget.h"
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化圆形进度条
    _bar=new RoundProgressBar(this);

    loadDefaultSettings();
    reloadPromoClock();

    //加载圆形进度条到制定位置
    ui->gridLayout_2->addWidget(_bar,0,0);

    //默认进入"番茄时钟"页面
    ui->stackedWidget->setCurrentIndex(0);

    //点击"番茄时钟"按钮, 切换到"番茄时钟"页面
    connect(ui->tool_btn_clock, &QToolButton::clicked, [=](){
       //索引0就是"番茄时钟"页面
       ui->stackedWidget->setCurrentIndex(0);
       //从"设置"页面获取基本数据
       _promo_working_time = ui->box_pomo_working_time_setting->getValue();
       _promo_rest_time = ui->box_pomo_rest_time_setting->getValue();
       _promo_number = ui->box_pomo_number_setting->getValue();
       _current_stage = 0;
       reloadPromoClock();
    });

    //点击"设置"按钮, 切换到"设置"页面
    connect(ui->tool_btn_settings, &QToolButton::clicked, [=](){
       ui->stackedWidget->setCurrentIndex(1);
    });


    QTimer *timer = new QTimer(this);

    //点击"开始"按钮 计时器开始计时
    connect(ui->tool_btn_start, &QToolButton::clicked, [=](){
        timer->start(DEFAULT_TIMER_INTERVAL);
    });

    //点击"暂停"按钮 计时器开始计时
    connect(ui->tool_btn_pause, &QToolButton::clicked, [=](){
        timer->stop();
    });

    //点击"快进"按钮 进入下一个阶段
    connect(ui->tool_btn_next, &QToolButton::clicked, [=](){
        _moveToNextStage();
        //如果阶段在用户预设范围内就重新加载番茄钟 超出范围就整体停止番茄钟
        if(_current_stage  < _promo_number * 2)
        {
            reloadPromoClock();
        }
        else
        {
            _promofocusFinished();
        }
        //进入下个阶段的时候就停止计时器
        timer->stop();
    });

    //每次计时间隔都减小_current_stage_timer当前阶段计时器的值
    connect(timer, &QTimer::timeout, [=](){
        //如果当前阶段计时器走完current_timer_value==0 就要切换到下一个阶段
        if(_current_stage_timer == 0)
        {
            _moveToNextStage();
            reloadPromoClock();
        }

        //如果当前番茄阶段超出用户预设阶段就停止番茄钟 打印提示信息并停止计时
        if(_current_promo_stage > _promo_number)
        {
            _promofocusFinished();
            timer->stop();
        }
        else
        {
            //倒计时 _current_stage_timer当前阶段计时器自减 他的数值是以秒为单位的 传递数据的时候要除以60
            _current_stage_timer -= 1;
            _bar->setValue(_current_stage_timer / 60);
            _bar->update();
        }
    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadDefaultSettings()
{
    //加载默认值到设置界面
    ui->box_pomo_working_time_setting->setValue(DEFAULT_PROMO_WORKING_TIME);
    ui->box_pomo_rest_time_setting->setValue(DEFAULT_PROMO_REST_TIME);
    ui->box_pomo_long_rest_time_setting->setValue(DEFAULT_PROMO_LONG_REST_TIME);
    ui->box_pomo_number_setting->setValue(DEFAULT_PROMO_NUMBER);

    //第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;
}

void Widget::loadValuesFromSettings()
{
    //从设置界面读取用户设置的值
    _promo_working_time = ui->box_pomo_working_time_setting->getValue();
    _promo_rest_time = ui->box_pomo_rest_time_setting->getValue();
    _promo_long_rest_time = ui->box_pomo_long_rest_time_setting->getValue();
    _promo_number = ui->box_pomo_number_setting->getValue();

    //第0个阶段
    _current_stage = 0;
    //工作阶段计时器
    _is_working_timer = true;
}

void Widget::reloadPromoClock()
{
    _resetCurrentPromoStage();
    _resetCurrentTimerValue();
    _resetCurrentMaxTime();
    _resetRoundProgressBar();
}

void Widget::_resetCurrentTimerValue()
{
    if(_is_working_timer)
        _current_stage_timer = _promo_working_time * 60;
    else
        _current_stage_timer = _promo_rest_time * 60;
}

void Widget::_resetCurrentMaxTime()
{
    _current_max_time = _is_working_timer ? _promo_working_time :
                        (_current_stage + 1 == _promo_number * 2) ? _promo_long_rest_time : _promo_rest_time;
}

void Widget::_resetCurrentPromoStage()
{
    _current_promo_stage = _current_stage / 2 + 1;
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
    if(_current_promo_stage <= _promo_number)
    {
        if(_is_working_timer)
        {
            //设置外圈颜色
            _bar->setOutterColor(QColor(233,248,248));
            //设置默认渐变色
            _bar->setInnerColor(QColor(95,113,127),QColor(157,167,176));//灰色
            _bar->setDefaultTextColor(QColor(95,113,127));
            //_bar->setInnerColor(QColor(49, 177, 190),QColor(133, 243, 244));//青色
            _bar->setToptitle(QString("%1/%2").arg(_current_promo_stage).arg(_promo_number));
            _bar->setSubtitle("专注");
        }
        else
        {
            _bar->setOutterColor(QColor(250,250,240));
            //_bar->setInnerColor(QColor(255,190,57),QColor(255,230,129)); //黄色
            _bar->setInnerColor(QColor(152,180,117),QColor(192,220,147)); //绿色
            _bar->setDefaultTextColor(QColor(152,180,117));
            if(_current_stage + 1 == _promo_number * 2)
                _bar->setSubtitle("长休息");
            else
                _bar->setSubtitle("休息");
        }
    }
}

void Widget::_promofocusFinished()
{
    QMessageBox::information(this, "完成番茄计划", "太棒了！你完成了整整" + QString::number(_promo_number) + "个番茄的计划");
    loadValuesFromSettings();
    reloadPromoClock();
}

void Widget::_moveToNextStage()
{
    //阶段计数器自加
    ++_current_stage;
    //切换工作/休息状态
    _is_working_timer = !_is_working_timer;
}


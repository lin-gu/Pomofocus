#include "soundscontrolpanel.h"
#include "ui_soundscontrolpanel.h"
#include <QSpinBox>
#include <QComboBox>
#include <QDebug>

SoundsControlPanel::SoundsControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundsControlPanel)
{
    ui->setupUi(this);

    //change value of volumeSpinBox, the value of volumeSlider will be changed accordingly
    void (QSpinBox:: * sbIntChanged)(int) = &QSpinBox::valueChanged;
    connect(ui->volumeSpinBox, sbIntChanged, ui->volumeSlider, &QSlider::setValue);

    //change value of volumeSlider, the value of volumeSpinBox will be changed accordingly
    connect(ui->volumeSlider, &QSlider::valueChanged, ui->volumeSpinBox, &QSpinBox::setValue);

    //每当用户选择一个音乐名称 就呼叫相应的槽函数
    void (QComboBox:: *cb_index_changed_with_text)(const QString &) = & QComboBox::currentIndexChanged;
    connect(ui->soundsSelectBox, cb_index_changed_with_text, this, &SoundsControlPanel::on_selectedSound);

    //设置默认音量
    setVolume(80);
    //设置默认重复次数
    setRepeat(1);

    //音量和重复不可用
    disableProgressVolumeRepeat();

    player = new QMediaPlayer;

    //播放时把播放时间的改变监听起来 用来决定声音的最长试听时间
    connect(player, &QMediaPlayer::positionChanged, this, &SoundsControlPanel::on_checkMaxPlayTime);
    //播放时把duration的改变监听 知道音频的总时间
    connect(player, &QMediaPlayer::durationChanged, this, &SoundsControlPanel::on_durationChanged);
}


SoundsControlPanel::~SoundsControlPanel()
{
    delete ui;
}

void SoundsControlPanel::loadSounds(QMap<int, QPair<QString, QString> > &sounds)
{
    QStringList name_list;
    for(int i=1; i<=sounds.size(); ++i)
    {
        name_list.append(sounds[i].first);
        _name_path_map[sounds[i].first] = sounds[i].second;
        if(i == 1)
        {
            _selected_sound_path = sounds[i].second;
        }
    }

    if(sounds.size() > 0)
    {
      ui->soundsSelectBox->addItems(name_list);
    }
}

QString SoundsControlPanel::getSoundName()
{
    return ui->soundsSelectBox->currentText();
}

void SoundsControlPanel::setSoundName(QString &sound_name)
{
    ui->soundsSelectBox->setCurrentText(sound_name);
}

QString SoundsControlPanel::getSoundPath()
{
    return _name_path_map[ui->soundsSelectBox->currentText()];
}

void SoundsControlPanel::setSoundPath(QString &sound_path)
{
    _name_path_map[ui->soundsSelectBox->currentText()] = sound_path;
}

int SoundsControlPanel::getVolume()
{
    return ui->volumeSpinBox->value();
}

void SoundsControlPanel::setVolume(int volume)
{
    ui->volumeSpinBox->setValue(volume);
}

int SoundsControlPanel::getRepeat()
{
    return ui->repeatSpinBox->value();
}

void SoundsControlPanel::setRepeat(int repeat)
{
    ui->repeatSpinBox->setValue(repeat);
}

void SoundsControlPanel::disableProgressVolumeRepeat()
{
    ui->volumeSpinBox->setDisabled(true);
    ui->volumeSlider->setDisabled(true);
    ui->repeatSpinBox->setDisabled(true);
}

void SoundsControlPanel::enableProgressVolumeRepeat()
{
    ui->volumeSpinBox->setDisabled(false);
    ui->volumeSlider->setDisabled(false);
    ui->repeatSpinBox->setDisabled(false);
}

void SoundsControlPanel::on_selectedSound(const QString &text)
{
    //先停止正在播放的音乐
    player->stop();

    //找到新选中音乐的文件路径 加载并播放
    _selected_sound_path = _name_path_map[text];
    if(_selected_sound_path != "")
    {
         player->setMedia(QUrl(_selected_sound_path));
         player->play();
    }

    if(!player->errorString().isEmpty())
        qDebug() << player->errorString();
    //只要文件不为空或者播放没有错误 就激活相应的组件
    if(_name_path_map[text] == "" || !player->errorString().isEmpty())
        disableProgressVolumeRepeat();
    else
        enableProgressVolumeRepeat();
}

void SoundsControlPanel::on_volumeSlider_sliderMoved(int volume)
{
    player->setVolume(volume);
}

void SoundsControlPanel::hideRepeatLayout()
{
    ui->repeatLabel->hide();
    ui->repeatSpinBox->hide();
}
void SoundsControlPanel::showRepeatLayout()
{
    ui->repeatLabel->show();
    ui->repeatSpinBox->show();
}

void SoundsControlPanel::on_checkMaxPlayTime(qint64 position)
{
    if(position / 1000 >  MAX_PLAY_TIME_IN_SECOND)
        player->stop();
}

void SoundsControlPanel::on_durationChanged(qint64 duration)
{
    duration = player->duration();
    _sound_duration = duration;
}

qint64 SoundsControlPanel::getSoundDuration()
{
    return _sound_duration;
}

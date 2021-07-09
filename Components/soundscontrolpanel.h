#ifndef SOUNDSCONTROLPANEL_H
#define SOUNDSCONTROLPANEL_H

#include <QWidget>
#include <QMap>
#include <QMediaPlayer>

namespace Ui {
class SoundsControlPanel;
}

#define MAX_PLAY_TIME_IN_SECOND 5

class SoundsControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SoundsControlPanel(QWidget *parent = nullptr);
    ~SoundsControlPanel();

    void loadSounds(QMap<int, QPair<QString, QString> > &sounds);
    QString getSoundName();
    void setSoundName(QString &sound_name);
    QString getSoundPath();
    void setSoundPath(QString &sound_path);
    int getVolume();
    void setVolume(int volume);
    int getRepeat();
    void setRepeat(int repeat);

    void hideRepeatLayout();
    void showRepeatLayout();

    void disableProgressVolumeRepeat();
    void enableProgressVolumeRepeat();
    void on_selectedSound(const QString &text);

    void on_checkMaxPlayTime(qint64 position);
    void on_durationChanged(qint64 duration);

    qint64 getSoundDuration();

    QMediaPlayer *player;

signals:
    void soundSelected();

private slots:
    void on_volumeSlider_sliderMoved(int volume);

private:
    Ui::SoundsControlPanel *ui;
    QMap<QString, QString> _name_path_map;
    QString _selected_sound_path;
    qint64 _sound_duration;

};

#endif // SOUNDSCONTROLPANEL_H

#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H

#include <QWidget>
#include <QSound>

namespace Ui {
class SettingsScene;
}

class SettingsScene : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScene(QWidget *parent = nullptr);
    ~SettingsScene();

signals:
    void cancelBtnClicked();

    void applyBtnClicked();

private slots:
    void on_cancel_btn_clicked();

    void on_apply_btn_clicked();

private:
    Ui::SettingsScene *ui;
    QSound *_applyBtnSound;
    QSound *_cancelBtnSound;
};

#endif // SETTINGSSCENE_H

#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H

#include <QWidget>

namespace Ui {
class SettingsScene;
}

class SettingsScene : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScene(QWidget *parent = nullptr);
    ~SettingsScene();

private:
    Ui::SettingsScene *ui;
};

#endif // SETTINGSSCENE_H

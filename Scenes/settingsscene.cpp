#include "settingsscene.h"
#include "ui_settingsscene.h"

SettingsScene::SettingsScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScene)
{
    ui->setupUi(this);
}

SettingsScene::~SettingsScene()
{
    delete ui;
}

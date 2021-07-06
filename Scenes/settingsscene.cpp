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


    //场景设置
    //设置场景标题
    this->setWindowTitle("设置");
}

SettingsScene::~SettingsScene()
{
    delete ui;
}

void SettingsScene::on_cancel_btn_clicked()
{
        _cancelBtnSound->play();
        this->hide();
        emit this->cancelBtnClicked();
}

void SettingsScene::on_apply_btn_clicked()
{
    _applyBtnSound->play();
    this->hide();
    emit this->applyBtnClicked();
}

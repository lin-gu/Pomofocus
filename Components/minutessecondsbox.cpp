#include "Components/minutessecondsbox.h"
#include "ui_minutessecondsbox.h"

MinutesSecondsBox::MinutesSecondsBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MinutesSecondsBox)
{
    ui->setupUi(this);
}

MinutesSecondsBox::~MinutesSecondsBox()
{
    delete ui;
}

void MinutesSecondsBox::setMunites(int min)
{
    ui->munitesBox->setValue(min);
}

int MinutesSecondsBox::getMunites()
{
    return ui->munitesBox->value();
}

void MinutesSecondsBox::setSeconds(int sec)
{
    ui->secondsBox->setValue(sec);
}

int MinutesSecondsBox::getSeconds()
{
    return ui->secondsBox->value();
}

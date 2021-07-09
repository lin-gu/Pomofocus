#include "Components/spinsliderbox.h"
#include "ui_spinsliderbox.h"
#include <QSpinBox>

SpinSliderBox::SpinSliderBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpinSliderBox)
{
    ui->setupUi(this);

    //change value of spinBox, the value of horizontalSlider will be changed accordingly
    void (QSpinBox:: * sbIntChanged)(int) = &QSpinBox::valueChanged;
    connect(ui->spinBox, sbIntChanged, ui->horizontalSlider, &QSlider::setValue);

    //change value of horizontalSlider, the value of spinBox will be changed accordingly
    connect(ui->horizontalSlider, &QSlider::valueChanged, ui->spinBox, &QSpinBox::setValue);

}

SpinSliderBox::~SpinSliderBox()
{
    delete ui;
}

void SpinSliderBox::setValue(int value)
{
    ui->spinBox->setValue(value);
}

int SpinSliderBox::getValue()
{
    return ui->spinBox->value();
}

void SpinSliderBox::setMax(int max)
{
    ui->spinBox->setMaximum(max);
    ui->horizontalSlider->setMaximum(max);
}

void SpinSliderBox::setMin(int min)
{
    ui->spinBox->setMinimum(min);
    ui->horizontalSlider->setMinimum(min);
}

#include "Scenes/mainscene.h"

#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //设置全局字体
    QFontDatabase::addApplicationFont(":/Fonts/YuppySC-Regular.ttf");
    QFont font("Yuppy SC", 12);
    a.setFont(font);

    MainScene w;
    w.show();
    return a.exec();
}

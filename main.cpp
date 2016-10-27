#include "wrt_window.h"
#include <QApplication>
#include "toolbox.h"
#include <cv.h>
 #include <QSoundEffect>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    toolbox::SetStyle("gray");

    //
    QSoundEffect effect;
    qDebug()<<"sds";
    effect.setSource(QUrl::fromLocalFile(":/wav/welcome.wav"));
    effect.setVolume(0.8f);
    effect.play();
    //
    wrt_window w;
    w.show();

    return a.exec();
}

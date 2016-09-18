#include "wrt_window.h"
#include <QApplication>
#include "toolbox.h"
#include <cv.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    toolbox::SetStyle("gray");


    wrt_window w;
    w.show();

    return a.exec();
}

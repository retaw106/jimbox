#include "jimbox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    jimbox w;
    w.show();

    return a.exec();
}

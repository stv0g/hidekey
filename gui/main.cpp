#include "hidekey.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hidekey w;
    w.show();
    return a.exec();
}

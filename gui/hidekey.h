#ifndef HIDEKEY_H
#define HIDEKEY_H

#include <QtGui/QMainWindow>
#include "ui_hidekey.h"

class hidekey : public QMainWindow
{
    Q_OBJECT

public:
    hidekey(QWidget *parent = 0);
    ~hidekey();

private:
    Ui::hidekeyClass ui;
};

#endif // HIDEKEY_H

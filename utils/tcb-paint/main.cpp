/**
 * Copyright (C) 2010-2010, by Andrey AndryBlack Kunitsyn
 * (support.andryblack@gmail.com)
 */

#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

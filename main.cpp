#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    int width = 710;
    int height = 710;

    w.move((QApplication::desktop()->width() - width) / 2 ,
           (QApplication::desktop()->height() - height) / 2);

    w.setMinimumWidth(width);
    //w.setMaximumWidth(width);
    w.setMinimumHeight(height);
    //w.setMaximumHeight(height);

    w.setWindowTitle("Rubik's cube 2x2 v. 1.0");
    w.show();

    return a.exec();
}

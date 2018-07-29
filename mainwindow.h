#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mb22.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMenuBar * bar; // main menu line
    QMenu * menu1;  //  menu Settings
    QAction * messAction;    // items of menu Settings

    QAction * rule1;
    QAction * rule2;
    QAction * wavCheck;
    QAction * wavChoose;
    QAction * colorChoose;

    QMenu* menu2;   // menu Rule 1
    QMenu* menu3;   // menu Rule 2
    QMenu* menu4;   // menu Help
    QAction * about;
    QAction * shortCut;

    MB22 * oglBox;

    float wheelDelta;

    void sendSignalPlus(){  emit signalPlus();  }
    void sendSignalMinus(){  emit signalMinus();  }
    void sendSignalZoom(float delta){  emit signalZoom(delta);  }

protected:
    void keyPressEvent(QKeyEvent * pe);
    void wheelEvent( QWheelEvent * e);

private:
    void createMenu();
    void createBox();

private slots:
    void showMessage();
    void slotOrder();
    void showRule();
    void slotPlayWAVClicked();
    void showShortCutInfo();
    void showAbout();
signals:
    void signalPlus();
    void signalMinus();
    void signalZoom(float data);
};

#endif // MAINWINDOW_H

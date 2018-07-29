#include "mainwindow.h"
#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    createBox();
    createMenu();
}

MainWindow::~MainWindow(){

}

void MainWindow::keyPressEvent(QKeyEvent *e){
    switch (e->key()) {
        case Qt::Key_Plus:
            qDebug() << "KEY +";
            sendSignalPlus();
            break;
        case Qt::Key_Minus:
            qDebug() << "KEY -";
            sendSignalMinus();
            break;
        default:
            break;
    }
}

void MainWindow::wheelEvent(QWheelEvent *e){
    //qDebug() << "wheelEvent";
    qreal scale;
    scale = (e->delta() / 120);
    //qDebug() << "wheelEvent->delta():" << scale;
    wheelDelta = e->delta();
    sendSignalZoom(wheelDelta);
}
void MainWindow::createMenu(){
    //qDebug() << "createMenu()";
    bar = new QMenuBar(this);

    menu1 = new QMenu("&Settings", bar);

    messAction = new QAction("&Mix 21 times", menu1);
    connect(messAction, SIGNAL(triggered()), oglBox, SLOT(slotButMessClicked()));
    menu1->addAction(messAction);

    QAction *orderAction = new QAction("&Order", menu1);
    connect(orderAction, SIGNAL(triggered()), oglBox, SLOT(slotButOrderClicked()));
    menu1->addAction(orderAction);

    QAction *isoAction = new QAction("Set &Isometry", menu1);
    connect(isoAction, SIGNAL(triggered()), oglBox, SLOT(slotButIsoClicked()));
    menu1->addAction(isoAction);

    menu1->addSeparator();

    rule1 = menu1->addAction("Show Rule #&1");
    rule1->setCheckable(true);
    rule1->setChecked(false);
    connect(rule1, SIGNAL(triggered()), this, SLOT(showRule()));

    rule2 = menu1->addAction("Show Rule #&2");
    rule2->setCheckable(true);
    rule2->setChecked(false);
    connect(rule2, SIGNAL(triggered()), this, SLOT(showRule()));

    menu1->addSeparator();

    wavCheck = menu1->addAction("Play WAV");
    wavCheck->setCheckable(true);
    wavCheck->setChecked(true);
    connect(wavCheck, SIGNAL(triggered()), this, SLOT(slotPlayWAVClicked()));

    wavChoose = menu1->addAction("Choose WAV ...");
    connect(wavChoose, SIGNAL(triggered()), oglBox, SLOT(slotChooseSound()));

    menu1->addSeparator();

    colorChoose = menu1->addAction("Choose background ...");
    connect(colorChoose, SIGNAL(triggered()), oglBox, SLOT(slotChooseColor()));

    menu2 = new QMenu("  * * * * * *", bar);
    menu3 = new QMenu("  * * * * * * * * * * * *", bar);
    menu4 = new QMenu("&Help", bar);
    about = new QAction("&About", menu4);
    menu4->addAction(about);
    connect(about, SIGNAL(triggered()), this, SLOT(showAbout()));
    menu4->addSeparator();
    shortCut = new QAction("&Shortcut commands", menu4);
    menu4->addAction(shortCut);
    connect(shortCut, SIGNAL(triggered()), this, SLOT(showShortCutInfo()));

    bar->addMenu(menu1);
    bar->addMenu(menu2);
    bar->addMenu(menu3);
    bar->addMenu(menu4);
    QMainWindow::setMenuBar(bar);

    connect(this, SIGNAL(signalPlus()), oglBox, SLOT(slotKeyPlus()));
    connect(this, SIGNAL(signalMinus()), oglBox, SLOT(slotKeyMinus()));
    connect(this, SIGNAL(signalZoom(float)), oglBox, SLOT(slotZoom(float)));
}

void MainWindow::createBox(){
    //qDebug() << "createBox()";
    oglBox = new MB22();
    oglBox->setCursor(Qt::CrossCursor);
    QMainWindow::setCentralWidget(oglBox);
}
// SLOTS
void MainWindow::showMessage(){
    qDebug() << "menu clicked";
}

void MainWindow::slotOrder(){
    qDebug() << "slotOrder";

}

void MainWindow::showRule(){ // done
    if (rule1->isChecked())
        menu2->setTitle(" R U -R -U");
    else
        menu2->setTitle("  * * * * * *");
    if (rule2->isChecked())
        menu3->setTitle("  U R -U -L U -R -U L U");
    else
        menu3->setTitle("  * * * * * * * * * * * *");
}

void MainWindow::slotPlayWAVClicked(){ //
    //qDebug() << "slotPlayWAVClicked()";
    if (wavCheck->isChecked()){
        wavChoose->setDisabled(false);
        oglBox->sound = true;
    }
    else {
        wavChoose->setDisabled(true);
        oglBox->sound = false;
    }

}

void MainWindow::showShortCutInfo(){
    QString txt = "Right button mouse click - rotation to counter clock wise direction";
    txt.append("\nLeft button mouse click - rotation to clock wise direction");
    txt.append("\nMouse scroll - scaling");
    txt.append("\n+/- key - faster/slowly rotation");
    QMessageBox::information (0, "Shortcut commands", txt);
}

void MainWindow::showAbout(){
    QString txt = "Look for instance here to solve Rubik's cube 2x2";
    txt.append("\nhttps://ruwix.com/twisty-puzzles/2x2x2-rubiks-cube-pocket/");
    txt.append("\n\nAnd for tip you can make visible needed two rules in Settings menu");
    txt.append("\n   ");
    txt.append("\ngithub.com/MarsLviv");
    QMessageBox::information (0, "About Rubik's cube 2x2", txt);
}

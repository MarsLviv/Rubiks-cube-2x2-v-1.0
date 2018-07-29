#include "mb22.h"

#include <QtGui>
#include <QMouseEvent>
#include <QScreen>
#include <GL/glu.h>
#include <math.h>
#include <locale.h>

#include <QColorDialog>
#include <QSound>
#include <QFileDialog>
#include <QMessageBox>

MB22::MB22(QWidget *parent) : QGLWidget(parent), scale(0.5), counter(0), delay(50)
            , nObj(0), backgroundColorINT(215.0, 214.0, 213.0, 1.0)
            , strSound(":/arrow3.wav"), sound(true) {
    boxes = new int[4];
    for (int i = 0; i < 8; i++){
        xRot[i] = 0; yRot[i] = 0; zRot[i] = 0;
    }
    zRot[1] = 90; yRot[2] = 180; yRot[3] = 90; zRot[4] = -90;// need rotate to be in
    zRot[5] = 180; xRot[6] = 180; zRot[7] = 180;    // decent position during init-g
    //poss.plot2T();
}

MB22::~MB22(){
    delete boxes;
}

void MB22::setScale(GLfloat newScale){
    if (newScale > 1.1){
        scaleU = 1.1; return;
    }
    if (newScale < 0.6){
        scaleU = 0.6; return;
    }
    scaleU = newScale; return;
}

GLfloat MB22::getScale(){
    return scale;
}

void MB22::mousePressEvent(QMouseEvent *e){

    if (counter > 0)// forbidden picking until processing previous pick (press)
        return;

    mousePressPosition = QVector2D(e->localPos());

    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow(winId());
    QRgb color = pixmap.toImage().pixel(e->x(), e->y());
    QColor color1 = pixmap.toImage().pixel(e->x(), e->y());
    qDebug() << "NEW PICK ";

    //  piked color in background
    if ( (color1.red() == backgroundColorINT.x() ) &&
        (color1.green() == backgroundColorINT.y() ) &&
        (color1.blue() == backgroundColorINT.z() ) ){
        qDebug() << "background";
        return;
    }

    if ( (color == 4278190080)// forbidden any color except black or any from surfaces
            || (color == 4294901760) || (color == 4278255360) || (color == 4294967040)
            || (color == 4294836224) || (color == 4294901248) || (color == 4278190335)
            || (color == 4294835456) || (color == 4278190334) || (color == 4294902015)
            || (color == 4278255104) || (color == 4294769664) || (color == 4294836478)
            || (color == 4294770688) || (color == 4278254848) || (color == 4294967295)
            || (color == 4294705152) || (color == 4278190333) || (color == 4294901502)
            || (color == 4294835709) || (color == 4294770941) || (color == 4278254592)
            || (color == 4278190332) || (color == 4294769916) || (color == 4294705404) ){
    ;} else
        return;

    if (color == 4278190080){   // piked black color in frame
        int tmp = 0;            // we going right in screen, while color of pixel == black
        while (color == 4278190080){qDebug() << "++tmp";
            color = pixmap.toImage().pixel(e->x() + ++tmp, e->y());
            color1 = pixmap.toImage().pixel(e->x() + ++tmp, e->y());
        }
    }
    // piked color in background.
    // need this check again.
    // because from black frame path can go to background
    if ( (color1.red() == backgroundColorINT.x() ) &&
        (color1.green() == backgroundColorINT.y() ) &&
        (color1.blue() == backgroundColorINT.z() ) ){
        qDebug() << "background from frame";
        return;
    }

    handlePickedColor(color);// calculate number [nObj] and it color [colorC] of picked box
    poss.picked(nObj, colorC);// calculate [face] and [boxes] to rotate
    face = poss.getFace();
    poss.getBoxes(boxes);
    qDebug() << "picked face" << face << "with boxes:" << boxes[0] << boxes[1] << boxes[2] << boxes[3];
    //poss.plot2T();

    for (int i = 0; i < 8; i++){
        xRot[i] = 0; yRot[i] = 0; zRot[i] = 0;
    }

    if(e->buttons() & Qt::RightButton){// rotate to counterClockWise
        if (sound)
            QSound::play(strSound);

        poss.rotateCounterClockWise();// rotate data in table
        //poss.plot2T();

        for (int i = 0; i < 4; i++){// rotate all picked boxes
                if (face == 'F') zRot[boxes[i] - 1] = 10;
                if (face == 'B') zRot[boxes[i] - 1] = -10;
                if (face == 'L') xRot[boxes[i] - 1] = -10;
                if (face == 'R') xRot[boxes[i] - 1] = 10;
                if (face == 'T') yRot[boxes[i] - 1] = 10;
                if (face == 'D') yRot[boxes[i] - 1] = -10;
        }
    }

    if(e->buttons() & Qt::LeftButton){// rotate to ClockWise
        if (sound)
            QSound::play(strSound);

        poss.rotateClockWise();// rotate data in table
        //poss.plot2T();

        for (int i = 0; i < 4; i++){// rotate all picked boxes
                if (face == 'F') zRot[boxes[i] - 1] = -10;
                if (face == 'B') zRot[boxes[i] - 1] = 10;
                if (face == 'L') xRot[boxes[i] - 1] = 10;
                if (face == 'R') xRot[boxes[i] - 1] = -10;
                if (face == 'T') yRot[boxes[i] - 1] = -10;
                if (face == 'D') yRot[boxes[i] - 1] = 10;
        }
    }

    counter = 9;
    timer.start(delay, this);

    for (int i = 0; i < 8; i++){
        xRotTMP[i] = xRot[i]; yRotTMP[i] = yRot[i]; zRotTMP[i] = zRot[i];
    }
}

void MB22::timerEvent(QTimerEvent *){
    if (counter > 0){
        for (int j = 0; j < 8; j++){
            xRot[j] = xRotTMP[j]; yRot[j] = yRotTMP[j]; zRot[j] = zRotTMP[j];
        }
        updateGL();
        counter--;
        if (counter == 0 && poss.checkForSolve())
            QMessageBox::information (0, "Congrats", "SOLVED!");
    }
    else {
        timer.stop();
    }
}

void MB22::setDelay(GLint newDelay){
    if (newDelay <= 1){
        delay = 1; return;
    }
    if (newDelay > 200){
        delay = 200; return;
    }
    delay = newDelay; return;
}

GLint MB22::getDelay(){
    return delay;
}

void MB22::setBackgroundColor(QVector4D newBackgroundColor){
    backgroundColorINT = newBackgroundColor;
}

QVector4D MB22::getBackgroundColor(){
    return backgroundColorINT;
}

void MB22::mouseMoveEvent(QMouseEvent *e)
{
    if (counter > 0)// forbidden updating <updateGL();> while process rotating
        return;

    // Current mouse position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse trajectory
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Rotation angle relative to the length of the mouse sweep
    qreal angle = diff.length() / 40;

    // New rotation axis
    rotationAxis = n.normalized();

    rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angle) * rotation;
    updateGL();
}

void MB22::initializeGL()
{
    initializeGLFunctions();
    glClearColor(backgroundColorINT.x() / 255, backgroundColorINT.y() / 255,
                 backgroundColorINT.z() / 255, backgroundColorINT.w());
    glShadeModel(GL_FLAT);// not works ???
    initShaders();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    //glEnable(GL_CULL_FACE);

    geometries1.init();
    geometries2.init();
    geometries3.init();
    geometries4.init();
    geometries5.init();
    geometries6.init();
    geometries7.init();
    geometries8.init();

    slotButIsoClicked();
}

void MB22::initShaders()
{
    // Override system locale until shaders are compiled
    setlocale(LC_NUMERIC, "C");

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Restore system locale
    setlocale(LC_ALL, "");
}

void MB22::resizeGL(int w, int h){
    glViewport(0, 0, w, h);

    // aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    const qreal zNear = 3.0, zFar = 12.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MB22::paintGL(){
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBox();
}

void MB22::drawBox(){ //
    QMatrix4x4 matrix1;
    matrix1.translate(0.0, 0.0, -5.0);
    matrix1.scale(scaleU);
    QQuaternion result1 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[0]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[0]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[0]);
    rotation1 = result1 * rotation1;
    matrix1.rotate(rotation * rotation1);// rotation1: rotating only this box; rotation: rotting all boxes
    matrix1.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);

    program.setUniformValue("mvp_matrix", projection * matrix1);// Send modelview-projection
                                                    // matrix to shader using "mvp_matrix"
    geometries1.drawGeometry(&program);// Draw cube_1 geometry

    QMatrix4x4 matrix2;
    matrix2.translate(0.0, 0.0, -5.0 );
    matrix2.scale(scaleU);
    QQuaternion result2 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[1]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[1]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[1]);
    rotation2 = result2 * rotation2;
    matrix2.rotate(rotation * rotation2);
    matrix2.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix2);
    geometries2.drawGeometry(&program);// Draw cube_2 geometry

    QMatrix4x4 matrix3;
    matrix3.translate(0.0, 0.0, -5.0);
    matrix3.scale(scaleU);
    QQuaternion result3 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[2]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[2]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[2]);
    rotation3 = result3 * rotation3;
    matrix3.rotate(rotation * rotation3);
    matrix3.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix3);
    geometries3.drawGeometry(&program);

    QMatrix4x4 matrix4;
    matrix4.translate(0.0, 0.0, -5.0);
    matrix4.scale(scaleU);
    QQuaternion result4 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[3]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[3]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[3]);
    rotation4 = result4 * rotation4;
    matrix4.rotate(rotation * rotation4);
    matrix4.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix4);
    geometries4.drawGeometry(&program);

    QMatrix4x4 matrix5;
    matrix5.translate(0.0, 0.0, -5.0);
    matrix5.scale(scaleU);
    QQuaternion result5 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[4]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[4]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[4]);
    rotation5 = result5 * rotation5;
    matrix5.rotate(rotation * rotation5);
    matrix5.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix5);
    geometries5.drawGeometry(&program);

    QMatrix4x4 matrix6;
    matrix6.translate(0.0, 0.0, -5.0);
    matrix6.scale(scaleU);
    QQuaternion result6 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[5]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[5]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[5]);
    rotation6 = result6 * rotation6;
    matrix6.rotate(rotation * rotation6);
    matrix6.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix6);
    geometries6.drawGeometry(&program);

    QMatrix4x4 matrix7;
    matrix7.translate(0.0, 0.0, -5.0);
    matrix7.scale(scaleU);
    QQuaternion result7 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[6]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[6]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[6]);
    rotation7 = result7 * rotation7;
    matrix7.rotate(rotation * rotation7);
    matrix7.translate(1.0 * scale, 1.0 * scale, 1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix7);
    geometries7.drawGeometry(&program);

    QMatrix4x4 matrix8;
    matrix8.translate(0.0, 0.0, -5.0);
    matrix8.scale(scaleU);
    QQuaternion result8 =   rotXYZangle(QVector3D(1.0, 0.0, 0.0), xRot[7]) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), yRot[7]) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), zRot[7]);
    rotation8 = result8 * rotation8;
    matrix8.rotate(rotation * rotation8);
    matrix8.translate(1.0 * scale, 1.0 * scale, -1.0 * scale);
    program.setUniformValue("mvp_matrix", projection * matrix8);
    geometries8.drawGeometry(&program);

    for (int i = 0; i < 8; i++){// after each 10 deg rotation clears data. But initial
        xRot[i] = 0; yRot[i] = 0; zRot[i] = 0; // data contain x(yz)RotTMP
    }
}

QQuaternion MB22::rotXYZangle(QVector3D XYZ, int angle){
    QQuaternion rotation = QQuaternion::fromAxisAndAngle(XYZ.normalized(), angle);
    return rotation;
}

void MB22::handlePickedColor(QRgb color){
    // 1 red 4294901760 gre 4278255360 yel 4294967040
    // 2 red 4294836224 yel 4294901248 blu 4278190335
    // 3 yel 4294835456 blu 4278190334 mag 4294902015
    // 4 gre 4278255104 yel 4294769664 mag 4294836478
    // 5 red 4294770688 gre 4278254848 whi 4294967295
    // 6 red 4294705152 blu 4278190333 whi 4294901502
    // 7 whi 4294835709 mag 4294770941 gre 4278254592
    // 8 blu 4278190332 whi 4294769916 mag 4294705404
    if (color == 4294901760 || color == 4278255360 || color == 4294967040){   // 1 box
        nObj = 1;                           // get number and it color of picked box
        if (color == 4294901760) colorC = 'r';
        else if (color == 4278255360) colorC = 'g';
        else colorC = 'y';
    }
    if (color == 4294836224 || color == 4294901248 || color == 4278190335){   // 2 box
        nObj = 2;
        if (color == 4294836224) colorC = 'r';
        else if (color == 4294901248) colorC = 'y';
        else colorC = 'b';
    }
    if (color == 4294835456 || color == 4278190334 || color == 4294902015){   // 3 box
        nObj = 3;
        if (color == 4294835456) colorC = 'y';
        else if (color == 4278190334) colorC = 'b';
        else colorC = 'm';
    }
    if (color == 4278255104 || color == 4294769664 || color == 4294836478){   // 4 box
        nObj = 4;
        if (color == 4278255104) colorC = 'g';
        else if (color == 4294769664) colorC = 'y';
        else colorC = 'm';
    }
    if (color == 4294770688 || color == 4278254848 || color == 4294967295){   // 5 box
        nObj = 5;
        if (color == 4294770688) colorC = 'r';
        else if (color == 4278254848) colorC = 'g';
        else colorC = 'w';
    }
    if (color == 4294705152 || color == 4278190333 || color == 4294901502){   // 6 box
        nObj = 6;
        if (color == 4294705152) colorC = 'r';
        else if (color == 4278190333) colorC = 'b';
        else colorC = 'w';
    }
    if (color == 4294835709 || color == 4294770941 || color == 4278254592){   // 7 box
        nObj = 7;
        if (color == 4294835709) colorC = 'w';
        else if (color == 4294770941) colorC = 'm';
        else colorC = 'g';
    }
    if (color == 4278190332 || color == 4294769916 || color == 4294705404){   // 8 box
        nObj = 8;
        if (color == 4278190332) colorC = 'b';
        else if (color == 4294769916) colorC = 'w';
        else colorC = 'm';
    }
    qDebug() << "picked" << nObj << "-st obj. with color" << color << " " << colorC;
}

void MB22::slotChooseColor(){
    //qDebug() << "slotChooseColor()";
    QColor color = QColorDialog::getColor(Qt::lightGray);
    if (color !=""){
        backgroundColorINT.setX(color.red());
        backgroundColorINT.setY(color.green());
        backgroundColorINT.setZ(color.blue());
        backgroundColorINT.setW(1.0);
    }
    glClearColor(backgroundColorINT.x() / 255, backgroundColorINT.y() / 255,
                 backgroundColorINT.z() / 255, backgroundColorINT.w());
    updateGL();
}

void MB22::slotChooseSound(){
    //qDebug() << "slotChooseSound()";
    QString str = QFileDialog::getOpenFileName(0, "Choose WAV-file","" , "*.wav");

    if (str !=""){
        QFileInfo fileinfo(str);
        qint64 size = fileinfo.size();
        if (size > 400000){
            QDialog * myDialog = new QDialog(this, Qt::Dialog);
            myDialog->setModal(true);
            myDialog->setWindowTitle("You chose very long sound!");
            myDialog->setFixedSize(250, 40);
            QPalette Pal(palette());
            Pal.setColor(QPalette::Background, Qt::red);
            myDialog->setAutoFillBackground(true);
            myDialog->setPalette(Pal);
            myDialog->exec();
        }
        else
            strSound = str;
    }
}

void MB22::slotButIsoClicked(){
    //qDebug() << "slotButIsoClicked";
    //works, but without delay
    rotation.setX(0.0); rotation.setY(0.0); rotation.setZ(0.0); rotation.setScalar(1.0);
    QQuaternion result =    rotXYZangle(QVector3D(1.0, 0.0, 0.0), 20) *
                            rotXYZangle(QVector3D(0.0, 1.0, 0.0), -20) *
                            rotXYZangle(QVector3D(0.0, 0.0, 1.0), 0);
    rotation = result * rotation;
    updateGL();
}

void MB22::slotButOrderClicked(){
    //qDebug() << "slotButOrderClicked";
    for (int i = 0; i < 8; i++){
        xRot[i] = 0; yRot[i] = 0; zRot[i] = 0;
    }
    zRot[1] = 90; yRot[2] = 180; yRot[3] = 90; zRot[4] = -90;
    zRot[5] = 180; xRot[6] = 180; zRot[7] = 180;

    rotation1.setX(0.0); rotation1.setY(0.0); rotation1.setZ(0.0); rotation1.setScalar(1);
    rotation2.setX(0.0); rotation2.setY(0.0); rotation2.setZ(0.0); rotation2.setScalar(1);
    rotation3.setX(0.0); rotation3.setY(0.0); rotation3.setZ(0.0); rotation3.setScalar(1);
    rotation4.setX(0.0); rotation4.setY(0.0); rotation4.setZ(0.0); rotation4.setScalar(1);
    rotation5.setX(0.0); rotation5.setY(0.0); rotation5.setZ(0.0); rotation5.setScalar(1);
    rotation6.setX(0.0); rotation6.setY(0.0); rotation6.setZ(0.0); rotation6.setScalar(1);
    rotation7.setX(0.0); rotation7.setY(0.0); rotation7.setZ(0.0); rotation7.setScalar(1);
    rotation8.setX(0.0); rotation8.setY(0.0); rotation8.setZ(0.0); rotation8.setScalar(1);
    poss.toOrder();
    updateGL();
}

void MB22::slotButMessClicked(){
    //qDebug() << "slotButMessClicked";

    for (int i = 0; i < 21; i++){
        int direction = Random(1, 2); // left - right
        int box = Random(1, 8); //
        int colorN = Random(1, 3); // each box have 3 color to choose

        switch (box){ // simulation handlePickedColor() function
            case 1: {
                nObj = 1;
                if (colorN == 1) colorC = 'r';
                else if (colorN == 2) colorC = 'g';
                else colorC = 'y';
                break;
            }
            case 2: {
                nObj = 2;
                if (colorN == 1) colorC = 'r';
                else if (colorN == 2) colorC = 'y';
                else colorC = 'b';
                break;
            }
            case 3: {
                nObj = 3;
                if (colorN == 1) colorC = 'y';
                else if (colorN == 2) colorC = 'b';
                else colorC = 'm';
                break;
            }
            case 4: {
                nObj = 4;
                if (colorN == 1) colorC = 'g';
                else if (colorN == 2) colorC = 'y';
                else colorC = 'm';
                break;
            }
            case 5: {
                nObj = 5;
                if (colorN == 1) colorC = 'r';
                else if (colorN == 2) colorC = 'g';
                else colorC = 'w';
                break;
            }
            case 6: {
                nObj = 6;
                if (colorN == 1) colorC = 'r';
                else if (colorN == 2) colorC = 'b';
                else colorC = 'w';
                break;
            }
            case 7: {
                nObj = 7;
                if (colorN == 1) colorC = 'w';
                else if (colorN == 2) colorC = 'm';
                else colorC = 'g';
                break;
            }
            case 8: {
                nObj = 8;
                if (colorN == 1) colorC = 'b';
                else if (colorN == 2) colorC = 'w';
                else colorC = 'm';
                break;
            }
            default: qDebug() << "YOU ARE IN TROUBLE. default section in slotButMessClicked() function";
        }
        //qDebug() << "dir:" << direction << "  box:" << box << "  colorC:" << colorC;
        poss.picked(nObj, colorC);// calculate [face] and [boxes] to rotate
        face = poss.getFace();
        poss.getBoxes(boxes);
        //qDebug() << "picked face" << face << "with boxes:" << boxes[0] << boxes[1] << boxes[2] << boxes[3];

        for (int i = 0; i < 8; i++){
            xRot[i] = 0; yRot[i] = 0; zRot[i] = 0;
        }
        if(direction == 1){// simulate rotatint to counterClockWise
            poss.rotateCounterClockWise();// rotate data in table
            for (int i = 0; i < 4; i++){// rotate all picked boxes
                    if (face == 'F') zRot[boxes[i] - 1] = 90;
                    if (face == 'B') zRot[boxes[i] - 1] = -90;
                    if (face == 'L') xRot[boxes[i] - 1] = -90;
                    if (face == 'R') xRot[boxes[i] - 1] = 90;
                    if (face == 'T') yRot[boxes[i] - 1] = 90;
                    if (face == 'D') yRot[boxes[i] - 1] = -90;
            }
        }

        if(direction == 2){// simulate rotatint to ClockWise
            poss.rotateClockWise();// rotate data in table
            for (int i = 0; i < 4; i++){// rotate all picked boxes
                    if (face == 'F') zRot[boxes[i] - 1] = -90;
                    if (face == 'B') zRot[boxes[i] - 1] = 90;
                    if (face == 'L') xRot[boxes[i] - 1] = 90;
                    if (face == 'R') xRot[boxes[i] - 1] = -90;
                    if (face == 'T') yRot[boxes[i] - 1] = -90;
                    if (face == 'D') yRot[boxes[i] - 1] = 90;
            }
        }
        updateGL();
    }
}

void MB22::slotKeyPlus(){
    int myDelay = delay - 10;
    setDelay(myDelay);
}

void MB22::slotKeyMinus(){
    int myDelay = delay + 10;
    setDelay(myDelay);
}

void MB22::slotZoom(float delta){
    if (delta > 0){
        float myScale = scaleU + 0.05;
        setScale(myScale);
    }
    else {
        float myScale = scaleU - 0.05;
        setScale(myScale);
    }
    updateGL();
}

int MB22::Random(int min, int max)
{	//srand( (int)clock() +  time( NULL ) % CLOCKS_PER_SEC);
    return min + rand() % (max - min + 1);
}

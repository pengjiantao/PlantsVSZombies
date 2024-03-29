#include "window.h"
#include "ui_window.h"
#include<QtOpenGL/QGLWidget>
#include<QtOpenGL/QGLFormat>
#include<QtOpenGL/QGL>
#include<QtOpenGL/QtOpenGL>
window::window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/image/Icon-144.png"));
    this->ui->main_screen_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers),this));
    this->ui->sun->setFixedSize(300,300);
    this->ui->tool_box->setFixedHeight(450);
    this->ui->groupBox->setFixedSize(150,300);
    this->ui->groupBox_2->setFixedSize(150,300);
    this->ui->groupBox_3->setFixedSize(150,300);
    this->ui->groupBox_4->setFixedSize(150,300);
    this->ui->groupBox_5->setFixedSize(150,300);
    this->ui->groupBox_6->setFixedSize(150,300);
    this->ui->groupBox_7->setFixedSize(150,300);
    this->ui->groupBox_8->setFixedSize(150,300);
    this->ui->groupBox_9->setFixedSize(150,300);
    this->ui->groupBox_10->setFixedSize(150,300);
    this->ui->plant0->setFixedSize(100,150);
    this->ui->plant1->setFixedSize(100,150);
    this->ui->plant2->setFixedSize(100,150);
    this->ui->plant3->setFixedSize(100,150);
    this->ui->plant4->setFixedSize(100,150);
    this->ui->plant5->setFixedSize(100,150);
    this->ui->plant6->setFixedSize(100,150);
    this->ui->plant7->setFixedSize(100,150);
    this->ui->plant8->setFixedSize(100,150);
    this->ui->plant9->setFixedSize(100,150);
    this->ui->shovel->setFixedSize(200,150);
    this->ui->pause->setFixedSize(200,150);
    this->ui->groupBox->setStyleSheet("background-color: grey");
    this->ui->groupBox_2->setStyleSheet("background-color: grey");
    this->ui->groupBox_3->setStyleSheet("background-color: grey");
    this->ui->groupBox_4->setStyleSheet("background-color: grey");
    this->ui->groupBox_5->setStyleSheet("background-color: grey");
    this->ui->groupBox_6->setStyleSheet("background-color: grey");
    this->ui->groupBox_7->setStyleSheet("background-color: grey");
    this->ui->groupBox_8->setStyleSheet("background-color: grey");
    this->ui->groupBox_9->setStyleSheet("background-color: grey");
    this->ui->groupBox_10->setStyleSheet("background-color: grey");
    this->ui->shovel->setStyleSheet("border-image: url(:/image/source/Shovel.png)");
    this->ui->pause->setStyleSheet("background-color: red");
    /*set sun widget style*/
    ui->sun->setDigitCount(5);
    ui->sun->setDecMode();
    ui->sun->setSegmentStyle(QLCDNumber::Flat);
    ui->sun->setStyleSheet("border: 1px solid green; color: green;background: silver");
}

window::~window()
{
    delete ui;
}

void window::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit(closed());
}

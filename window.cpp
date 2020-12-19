#include "window.h"
#include "ui_window.h"

window::window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{
    ui->setupUi(this);
    /*set sun widget style*/
    ui->sun->setDigitCount(5);
    ui->sun->setDecMode();
    ui->sun->setSegmentStyle(QLCDNumber::Flat);
    ui->sun->setStyleSheet("border: 1px solid green; color: green; background: silver;");
    ui->main_screen_view->setCacheMode(QGraphicsView::CacheBackground);
    ui->main_screen_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
}

window::~window()
{
    delete ui;
}

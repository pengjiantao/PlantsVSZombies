#include "sun.h"
#include<iostream>
#include"screen.h"
sun::sun(int s)
{
    value_=s;
    update_clock_=new QTimer();
    update_clock_->setInterval(50);
    update_clock_->start();
    connect(update_clock_,SIGNAL(timeout()),this,SLOT(update_clock_timeout()));
    connect(this,SIGNAL(beClicked()),this,SLOT(beClickedSlot()));
    this->setHeight(80);
    this->setWidth(80);
    this->setMovie(":/image/source/Sun.gif");
    this->show();
    this->selected_=false;
    botton=screen::PlantBase().height()+screen::YardSize().height()*screen::size_info.screen_high;
}

int sun::Botton()
{
    return botton;
}

void sun::setBotton(int n)
{
    botton=n;
}

int sun::Value()
{
    return this->value_;
}

void sun::setValue(int n)
{
    this->value_=n;
}

void sun::update_clock_timeout()
{
    QPointF p=this->pos();
    if(!selected_){
        if(p.y()<botton)
        {
           this->MoveDown(2);
        }
    }
    else if(p.y()<screen::PlantBase().height()&&p.x()<screen::PlantBase().width())
    {
        emit(beCollected(this));
    }
    else
    {
        this->MoveTop(20);
        this->MoveHead(p.x()/p.y()*20);
    }
}

void sun::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    std::cout<<"click event"<<std::endl;
    Q_UNUSED(event);
    emit(beClicked());
}

void sun::beClickedSlot()
{
    this->selected_=true;
}

void sun::pauseSlot()
{
    disconnect(update_clock_,SIGNAL(timeout()),this,SLOT(update_clock_timeout()));
    disconnect(this,SIGNAL(beClicked()),this,SLOT(beClickedSlot()));
}

void sun::continueSlot()
{
    connect(update_clock_,SIGNAL(timeout()),this,SLOT(update_clock_timeout()));
    connect(this,SIGNAL(beClicked()),this,SLOT(beClickedSlot()));
}

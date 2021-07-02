#include "pumpkin.h"
#include"screen.h"
PumpKin::PumpKin(QPointF s)
{
    this->setPos(s);
    level=1;
    health_=500;
    full_health_=health_;
    this->setHeight(screen::YardSize().height()/2);
    this->setWidth(screen::YardSize().width());
    this->setMovie(":/image/plant/9/PumpkinHead.gif");
    this->show();
}

float PumpKin::Health() const
{
    return health_;
}

float PumpKin::FullHealth() const
{
    return full_health_;
}

void PumpKin::deHealth(float n)
{
    health_-=n;
    if(health_<=0)
        emit(die(this));
}

void PumpKin::inHealth(float n)
{
    health_+=n;
}

int PumpKin::Level() const
{
    return level;
}

void PumpKin::updateMyself()
{
    if(health_<(full_health_*2/3)&&level<2)
    {
        this->setMovie(":/image/plant/9/PumpkinHead1.gif");
        level++;
    }
    else if(health_<(full_health_/3)&&level<3)
    {
        this->setMovie(":/image/plant/9/PumpkinHead2.gif");
        level++;
    }
    this->update();
}

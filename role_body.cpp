#include "role_body.h"
#include<iostream>
role_body::role_body(qreal wid,qreal hgh):m_width(wid),m_height(hgh)
{
    movie=nullptr;
}

QRectF role_body::boundingRect() const
{
    return QRectF(-m_width/2-1,-m_height/2-1,m_width+2,m_height+2);
}

void role_body::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if(movie&&movie->state()==QMovie::Running)
    {
        QRectF bound = boundingRect().adjusted(10, 10, -5, -5);
        painter->drawImage(bound, movie->currentImage());
    }
}

QPainterPath role_body::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(-m_width/2,-m_height/2,m_width,m_height));
    return path;
}

void role_body::setMovie(QString path)
{
    if(movie)
        delete movie;
    movie=new QMovie(path);
    movie->start();
}


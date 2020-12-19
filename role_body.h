#ifndef ROLE_BODY_H
#define ROLE_BODY_H

#include<QGraphicsItem>
#include<QPainter>
#include<QRectF>
#include<QPainterPath>
#include<QString>
#include<QMovie>
class role_body:public QGraphicsItem
{
public:
    role_body(qreal wid=100,qreal hgt=100);
    QRectF boundingRect() const;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option=NULL,QWidget* widget=NULL);
    QPainterPath shape() const;
    void setMovie(QString path);
private:
    qreal m_width;
    qreal m_height;
    QMovie* movie;
};

#endif // ROLE_BODY_H

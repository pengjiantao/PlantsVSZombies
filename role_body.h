#ifndef ROLE_BODY_H
#define ROLE_BODY_H

#include<QGraphicsItem>
#include<QPainter>
#include<QRectF>
#include<QPainterPath>
#include<QString>
#include<QMovie>
#include<QTimer>
class role_body:public QObject,public QGraphicsItem
{
    Q_OBJECT
public:
    ~role_body();
    role_body(qreal wid=100,qreal hgt=100);
    QRectF boundingRect() const;
    void paint(QPainter* painter,const QStyleOptionGraphicsItem* option=NULL,QWidget* widget=NULL);
    QPainterPath shape() const;
    void setMovie(QString path);
    void MoveHead(qint32 n);
    void MoveBack(qint32 n);
    void setPosByPosition(const QPointF& n);
    void setWidth(qreal width)
    {
        m_width=width;
        boundingRect();
    }
    void setHeight(qreal height)
    {
        m_height=height;
        boundingRect();
    }
    QMovie* Movie() const
    {
        return movie;
    }
    void setTimer(QTimer* s);
    void resetTimer();
    QTimer* Timer() const
    {
        return timer_;
    }
private:
    qreal m_width;
    qreal m_height;
    QMovie* movie;
    QTimer* timer_=nullptr;
    QTimer* update_clock_;
private slots:
    void movieEnd();
    void updateMyself();
signals:
    void end(role_body* s);
};

#endif // ROLE_BODY_H

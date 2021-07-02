#ifndef SUN_H
#define SUN_H
#include<QObject>
#include<QGraphicsItem>
#include"role_body.h"
#include<QTimer>
class sun:public role_body
{
    Q_OBJECT
public:
    sun(int s=50);
    ~sun(){
        update_clock_->disconnect();
        delete update_clock_;
        this->disconnect();
    }
    int Botton();
    void setBotton(int n);
    int Value();
    void setValue(int n);
private:
    int value_;
    QTimer* update_clock_;
    bool selected_=false;
    int botton;
private slots:
    void update_clock_timeout();
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void beClickedSlot();
    void pauseSlot();
    void continueSlot();
signals:
    void beCollected(sun*);
    void beClicked();
};

#endif // SUN_H

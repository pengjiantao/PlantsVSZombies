#ifndef PUMPKIN_H
#define PUMPKIN_H
#include"role_body.h"

class PumpKin:public role_body
{
    Q_OBJECT
public:
    PumpKin(QPointF s);
    ~PumpKin(){
        disconnect();
    }
    float Health() const;
    float FullHealth() const;
    void deHealth(float n);
    void inHealth(float n);
    int Level() const;
protected:
    float health_=500;
    float full_health_=500;
    int level=1;
protected slots:
    void updateMyself();
signals:
    void die(PumpKin*);
};

#endif // PUMPKIN_H

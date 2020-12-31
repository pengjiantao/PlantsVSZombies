#pragma once
#include"swfunix.h"
#include<iostream>
#include<string>
#include<QObject>
#include<QGraphicsItem>
#include<QString>
#include<QMovie>
#include<QTime>
#include<QTimer>
#include"role_body.h"
#include"PlantVSZombie.h"
#include"pumpkin.h"
using namespace std;

class yard_node;

enum plant_kind {
	shoot=0,sunflower,doubleshoot,iceshoot,nut,highnut,wogua,cherrybomb,garlic,pumpkin
};

enum  zombie_kind {
	conehead=0,reading,pole,clown,throwstone,normal
};


template<typename T1, typename T2> struct locate {
	T1 high;
	T2 width;
};


enum class zombie_status{
    attack,walk,wait
};


class plant_info {
public:
    plant_info():name("NULL"),health(0),ice_time(0),attack_power(0),price(0),wait(0),store_card_(nullptr) {};
    plant_info(const char* _name, int _health, int _attack_power,int _price,QWidget* card) :
        name(_name),health(_health),attack_power(_attack_power),price(_price),ice_time(_price/50){
        wait=ice_time;
        store_card_=card;
    }
	~plant_info();
public:
    bool prepared=false;
    qreal wait;
	const int price;
	const double ice_time;
	const char* name;
    const int health;
	const int attack_power;
    QWidget* store_card_;
};


class zombie_info {
public:
	static bool ALL_ZOMBIE;
    static int ZOMBIE_NUM;
    zombie_info():health(0),name("NULL"),speed(0),attack_power(0),number(0){}
    zombie_info(const int _health,const int _attack_power,const float _speed,const char* _name,const int num);
	~zombie_info();
public:
	int number = 0;
    int health;
    int attack_power;
    float speed;
    const char* name;
};


class role:public QObject
{
    Q_OBJECT
public:
    virtual bool attack(double time,yard_node*** yard) = 0;
    role(const char* _name,float _health,int _attack_power);
    virtual ~role();
	float getHealth() const{
		return health;
	}
	bool ifDead();
	char* getName() const{
		return (char*)name;
	}
	bool skill = true;
    virtual bool deHealth(float n);
	bool inHealth(float n);
	int get_attack_power() const {
		return attack_power;
	}
	void changeName(char* new_name) { name = new_name; }
    void setScale(float s){
        this->body->setScale(s);
    }
protected:
	char* name;

	float health;

    float fullHealth;

	int attack_power = 0;

    int fullAttackPower;

    static int flash_time_;

    int level=1;
public:
    float FullHealth() const;
    int FullAttackPower() const;
    QTimer* attack_time;
    role_body *body;
    static int FlashTime();
    static void setFlashTime(int n);
    void setBodyFlashTime(int n);
    int bodyLevel() const;
private slots:
    virtual void timeout_attack();
};
class zombie;

class plant :public role {
    Q_OBJECT
public:
    plant(const char* _name, float _health,int _attack_power,int _price,locate<int,int> _position);
	virtual ~plant();
    virtual bool attack(double time,yard_node*** yard) = 0;
	locate<int, int> getPosition() const { return position; }
	int getprice() const
	{
		return price;
	}
    PumpKin* ProtectHead() const;
    void setProtectHead(PumpKin* s);
    bool deHealth(float s);
protected:
	locate<int, int> position;
	int price = 0;
    PumpKin* protect_head_;
signals:
    void createBullet(plant* s);
    void die(plant* s);
    void zombieDie(zombie* s);
private slots:
    void timeout_attack();
    virtual void pauseSlot();
    virtual void continueSlot();
    void protectHeadDestroy(PumpKin* s);
};

class zombie :public role {
    Q_OBJECT
public:
    zombie(const char* _name, float _health,int _attack_power,float _speed);
	virtual ~zombie();
    virtual bool attack(double time,yard_node*** yard) = 0;
	locate<int,float> get_position() const{
		return { position.high,position.width };
	}
    bool move(float d,yard_node*** yard);
    bool move_aside(yard_node*** yard);
	bool deSpeed(float n) {
		speed -= n;
		if (speed < 0) speed = 0;
		return true;
	}
	float getSpeed() const{ return speed; }
    void beIce();
    float FullSpeed() const;
protected:
	locate<int,float> position;
	float speed;
    float fullSpeed;
    zombie_status status=zombie_status::walk;
    bool beIced=false;
    QTimer *ice_clock_;
protected slots:
    void timeout_attack();
    virtual void walkToAttackSlot();
    virtual void attackToWalkSlot();
    virtual void runToPauseSlot();
    virtual void pauseToRunSlot();
    void ice_clock_timeout();

signals:
    void success();
    void plantDie(plant* n);
    void die(zombie* n);
    void walkToAttack();
    void attackToWalk();
};

/*sunflower*/
class Sunflower :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time,yard_node*** yard);
	Sunflower(const plant_info& src,locate<int,int> p);
    virtual ~Sunflower(){
        disconnect();
        sun_timer_->disconnect();
        delete sun_timer_;
    }
private:
    float ice_timer_;
    QTimer* sun_timer_;
private slots:
    void sun_clock_timeout();
    void pauseSlot();
    void continueSlot();
};

/*shoot*/
class Shoot :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Shoot(const plant_info& src, locate<int, int> p);
    virtual ~Shoot(){
        disconnect();
        attack_clock_->disconnect();
        delete attack_clock_;
    }
private:
    float ice_time_;
    QTimer* attack_clock_;
private slots:
    void attack_clock_timeout();
    void pauseSlot();
    void continueSlot();
};

/*doubleshoot*/
class Doubleshoot :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Doubleshoot(const plant_info& src, locate<int, int> p);
    virtual ~Doubleshoot() {
        attack_clock_->disconnect();
        delete attack_clock_;
        second_shoot_->disconnect();
        delete second_shoot_;
        this->disconnect();
    }
private:
    float ice_timer_;
    QTimer* attack_clock_;
    QTimer* second_shoot_;
private slots:
    void attack_clock_timeout();
    void second_shoot_timeout();
    void pauseSlot();
    void continueSlot();
};

/*iceshoot*/
class Iceshoot :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Iceshoot(const plant_info& src, locate<int, int> p);
    virtual ~Iceshoot() {
        attack_clock_->disconnect();
        delete attack_clock_;
        this->disconnect();
    }
private:
    float ice_time_;
    QTimer* attack_clock_;
private slots:
    void attack_clock_timeout();
    void pauseSlot();
    void continueSlot();
};

/*nut*/
class Nut :public plant {
public:
    virtual bool attack(double time, yard_node*** yard);
	Nut(const plant_info& src, locate<int, int> p);
    virtual ~Nut(){
        disconnect();
    }
private:
    void timeout_attack();
};

/*highnut*/
class Highnut :public plant {
public:
    virtual bool attack(double time, yard_node*** yard);
	Highnut(const plant_info& src, locate<int, int> p);
    virtual ~Highnut(){
        disconnect();
    }
private:
    void timeout_attack();
};

/*wogua*/
class Wogua :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Wogua(const plant_info& src, locate<int, int> p);
    virtual ~Wogua(){
        disconnect();
    }
private:
    bool attacking=false;
private slots:
    void animationEndSlot();
    void timeout_attack();
    void pauseSlot();
    void continueSlot();
};

/*cherrybomb*/
class Cherrybomb :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Cherrybomb(const plant_info& src, locate<int, int> p);
    virtual ~Cherrybomb(){
        disconnect();
    }
private:
    bool attacking=false;
private slots:
    void animationEndSlot(role_body*);
    void timeout_attack();
    void pauseSlot();
    void continueSlot();
};

/*garlic*/
class Garlic :public plant {
    Q_OBJECT
public:
    virtual bool attack(double time, yard_node*** yard);
	Garlic(const plant_info& src, locate<int, int> p);
    virtual ~Garlic() {
        disconnect();
    };
private slots:
    void timeout_attack();
};


/*Conehead*/
class Conehead :public zombie {
    Q_OBJECT
public:
	Conehead(zombie_info& k);
    bool virtual attack(double time, yard_node*** yard);
    ~Conehead(){
        disconnect();
    }
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
    void timeout_attack();
};

/*reading*/
class Reading :public zombie {
    Q_OBJECT
public:
	Reading(zombie_info& k);
    bool virtual attack(double time, yard_node*** yard);
    ~Reading(){
        disconnect();
    }
protected:
    bool loseNewspaper=false;
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
    void timeout_attack();
};

/*pole*/
class Pole :public zombie {
    Q_OBJECT
public:
	Pole(zombie_info& k);
    bool virtual attack(double time, yard_node*** yard);
    ~Pole(){
        disconnect();
    }
private:
    bool jumping=false;
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
    void dealJump0Finished();
    void dealJump1Finished();
    void timeout_attack();
signals:
    void jump0Finished();
    void jump1Finished();
};

/*clown*/
class Clown :public zombie {
    Q_OBJECT
public:
	Clown(zombie_info& k);
    bool virtual attack(double time, yard_node*** yard);
    ~Clown(){
        open_box_->disconnect();
        bomb_clock_->disconnect();
        delete open_box_;
        delete bomb_clock_;
        disconnect();
    }
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
    void bomb();
    void openBox();
    void bombEnd();
private:
    QTimer *bomb_clock_;
    QTimer *open_box_;
};

/*throwstone*/
class Throwstone :public zombie {
    Q_OBJECT
public:
	Throwstone(zombie_info& k);
    bool virtual attack(double time, yard_node*** yard);
    ~Throwstone(){
        disconnect();
    }
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
};

/*normal*/
class Normal :public zombie {
    Q_OBJECT
public:
	Normal(zombie_info& k);
    bool virtual attack(double time,yard_node*** yard);
    virtual ~Normal(){disconnect();}
private slots:
    void walkToAttackSlot();
    void attackToWalkSlot();
    void runToPauseSlot();
    void pauseToRunSlot();
};

class Bullet :public role {
    Q_OBJECT
public:
    Bullet(const char* name,int _attack_power, float _speed,locate<int,float> p);
    bool virtual attack(double time, yard_node*** yard);
    virtual ~Bullet(){disconnect();};
protected:
    zombie_status status;
    locate<int,float> position;
    float speed_;
private slots:
    void pauseToRunSlot();
    void runToPauseSlot();
    void timeout_attack();
signals:
    void die(Bullet* s);
    void zombieDie(zombie* s);
};


class yard_node:public QObject {
    Q_OBJECT
public:
	plant* p;
    zombie* z[50];
	int first = -1;
public:
    yard_node() :p(nullptr){
        for (int i = 0; i < 50; i++)
            z[i] = nullptr;
		first = -1;
	}
    ~yard_node(){
        disconnect();
    }
	void find_first(double time);
	void kii_all_zombie();

	void kill_plant();
	bool push_zombie(zombie* zom);
	bool pop_zombie(zombie* zom);
signals:
    void zombieDie(zombie* s);
    void plantDie(plant* s);
};

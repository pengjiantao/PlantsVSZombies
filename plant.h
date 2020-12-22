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
using namespace std;

class yard_node;


/*角色颜色枚举*/
enum  obj_color {
	black=0, lightblue, lightgreen, lakeblue, lightred, purple, yellow, white,
	grey, blue, green, lightlightgreen, red, lightpurple, lightyellow, brightwhite
};

/*植物枚举*/
enum plant_kind {
	shoot=0,sunflower,doubleshoot,iceshoot,nut,highnut,wogua,cherrybomb,garlic,pumpkin
};

/*僵尸枚举*/
enum  zombie_kind {
	conehead=0,reading,pole,clown,throwstone,normal
};


template<typename T1, typename T2> struct locate {
	T1 high;
	T2 width;
};





/*this is a class named plant_info which stroe the
information of one kind of plant,these information will
be used when a plant be created*/
class plant_info {
public:
	plant_info():name("NULL"),health(0),ice_time(0),attack_power(0),color(black),price(0),last_create(0) {};
	plant_info(const char* _name, int _health, int _attack_power, obj_color _color,int _price);
	~plant_info();
public:
	/*last time to create this plant*/
    uint64_t last_create;
	const int price;
	/*ice_time of plant,this is a const vary*/
	const double ice_time;
	const char* name;
	const int health;
	/*攻击力*/
	const int attack_power;
	const obj_color color;
};

/*this is a class which stroe the
basic information of a kind of zombie,
this information will be used when a zombie be created*/
class zombie_info {
public:
	/*是否生成全部僵尸*/
	static bool ALL_ZOMBIE;

	zombie_info():health(0),name("NULL"),speed(0),color(black),attack_power(0),number(0){}
	zombie_info(const int _health,const int _attack_power,const float _speed,const char* _name,const obj_color _color,const int num);
	~zombie_info();
public:
	int number = 0;
	const int health;
	const int attack_power;
	const float speed;
	const char* name;
	const obj_color color;
};

/*this a base class of all zombies and plants!*/
class role:public QObject
{
    Q_OBJECT
public:
	/*this a attack logic which is variable 
	on different kind of plant or zombie.*/

	virtual bool attack(double time,yard_node** yard) = 0;
	role(const char* _name,float _health,obj_color _objcolor,int _attack_power);
    virtual ~role();

	/*get health value*/

	float getHealth() const{
		return health;
	}

	/*if health value<0 return true,else return false.*/

	bool ifDead();

	char* getName() const{
		return (char*)name;
	}

	obj_color getColor() const{
		return color;
	}

	bool skill = true;
	/*decrease health value ,if something error happen,
	it return false and throw an error,
	else it will return true. */
	bool deHealth(float n);

	/*this function returns true if nothing beyond exception happen.*/
	bool inHealth(float n);

	int get_attack_power() const {
		return attack_power;
	}

	void changeName(char* new_name) { name = new_name; }
protected:
	char* name;

	/*health of role*/
	float health;

	/*color of role at this time*/
	obj_color color;

	int attack_power = 0;

    static int flash_time_;
public:
    QTimer* attack_time;
    role_body *body;
    int FlashTime();
    void setFlashTime(int n);
private slots:
    virtual void timeout_attack();
};

/*this is the father class of all kinds of plants*/
class plant :public role {
public:
	plant(const char* _name, float _health, obj_color _objcolor,int _attack_power,int _price,locate<int,int> _position);
	virtual ~plant();
	virtual bool attack(double time,yard_node** yard) = 0;
	locate<int, int> getPosition() const { return position; }
	int getprice() const
	{
		return price;
	}
protected:
	locate<int, int> position;
	int price = 0;
};

/*this is the father class of all kinds of zombies*/
class zombie :public role {

public:
	zombie(const char* _name, float _health, obj_color _objcolor,int _attack_power,float _speed);
	virtual ~zombie();
	virtual bool attack(double time,yard_node** yard) = 0;
	locate<int,float> get_position() const{
		return { position.high,position.width };
	}
	bool move(float d,yard_node** yard);
	bool move_aside(yard_node** yard);
	bool deSpeed(float n) {
		color = lightblue;
		speed -= n;
		if (speed < 0) speed = 0;
		return true;
	}
	float getSpeed() const{ return speed; }
protected:
	locate<int,float> position;
	float speed;
private :
    void timeout_attack();
};

/*sunflower*/
class Sunflower :public plant {
public:
	virtual bool attack(double time,yard_node** yard);
	Sunflower(const plant_info& src,locate<int,int> p);
	virtual ~Sunflower() =default;

private:
	/*生成阳关的冷却时间*/
	float iceTime;
    uint64_t last_time_create_money;
};

/*shoot*/
class Shoot :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Shoot(const plant_info& src, locate<int, int> p);
	virtual ~Shoot(){}
private:
	float iceTime;
    uint64_t last_time_of_shoot;
};

/*doubleshoot*/
class Doubleshoot :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Doubleshoot(const plant_info& src, locate<int, int> p);
	virtual ~Doubleshoot() {}
private:
	float iceTime;
    uint64_t last_time_of_shoot;
};

/*iceshoot*/
class Iceshoot :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Iceshoot(const plant_info& src, locate<int, int> p);
	virtual ~Iceshoot() {}
private:
	float iceTime;
    uint64_t last_time_of_shoot;
};

/*nut*/
class Nut :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Nut(const plant_info& src, locate<int, int> p);
	virtual ~Nut(){}
};

/*highnut*/
class Highnut :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Highnut(const plant_info& src, locate<int, int> p);
	virtual ~Highnut(){}
};

/*wogua*/
class Wogua :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Wogua(const plant_info& src, locate<int, int> p);
	virtual ~Wogua(){}
};

/*cherrybomb*/
class Cherrybomb :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Cherrybomb(const plant_info& src, locate<int, int> p);
	virtual ~Cherrybomb(){}
};

/*garlic*/
class Garlic :public plant {
public:
	virtual bool attack(double time, yard_node** yard);
	Garlic(const plant_info& src, locate<int, int> p);
	virtual ~Garlic() {};
};

/*pumpkin*/
class Pumpkin :public plant {

};

/*Conehead*/
class Conehead :public zombie {
public:
	Conehead(zombie_info& k);
	bool virtual attack(double time, yard_node** yard);
	~Conehead(){}
};

/*reading*/
class Reading :public zombie {
public:
	Reading(zombie_info& k);
	bool virtual attack(double time, yard_node** yard);
	~Reading(){}
};

/*pole*/
class Pole :public zombie {
public:
	Pole(zombie_info& k);
	bool virtual attack(double time, yard_node** yard);
	~Pole(){}
};

/*clown*/
class Clown :public zombie {
public:
	Clown(zombie_info& k);
	bool virtual attack(double time, yard_node** yard);
	~Clown(){}
};

/*throwstone*/
class Throwstone :public zombie {
public:
	Throwstone(zombie_info& k);
	bool virtual attack(double time, yard_node** yard);
	~Throwstone(){}
private:
	float store_power = 70;
	float shoot_freq = 1.5;
	int shootNum = 5;
    uint64_t last_shoot;
};

/*normal*/
class Normal :public zombie {
public:
	Normal(zombie_info& k);
	bool virtual attack(double time,yard_node** yard);
	~Normal(){}
};

class Bullet :public zombie {
public:
	Bullet(const char* name, obj_color _color, int _attack_power, float _speed,const char* _aim,locate<int,float> p);
	bool virtual attack(double time, yard_node** yard);
	~Bullet(){}
	const char* aim = "zombie";
};


class yard_node {
public:
	using menufunc = void (*)();
	obj_color color;
	/*如果有植物，p!=NULL,else p=NULL*/
	plant* p;
	/*僵尸队列，最多十个*/
	zombie* z[10];
	int first = -1;
public:
    yard_node() :p(nullptr), color(black), iz{ 0 }{
		for (int i = 0; i < 10; i++)
            z[i] = nullptr;
		first = -1;
		effect_time = 1;
		time_of_killzombies = 0;
		
	}
	~yard_node() = default;
	void find_first(double time);
	void kii_all_zombie();
	void icefirstzombie();

	void kill_plant();
	/*插入一个僵尸，如果失败返回flase*/
	bool push_zombie(zombie* zom);
	/*减少一个僵尸，失败返回false*/
	bool pop_zombie(zombie* zom);
private:
    uint64_t time_of_killzombies = 0;
	float effect_time;
	float iz[10];
};

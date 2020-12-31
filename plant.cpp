#include"plant.h"
#include"swfunix.h"
#include"toolfunc.h"
#include"screen.h"
#include"role_body.h"
#include"PlantVSZombie.h"
#include<QRandomGenerator>
using namespace std;

template struct locate<int, int>;
template struct locate<int,float>;


plant_info::~plant_info(){}

bool zombie_info::ALL_ZOMBIE = false;
int zombie_info::ZOMBIE_NUM=6;
zombie_info::zombie_info(const int _health,const int _attack_power,const float _speed,const char* _name,const int num):
health(_health),attack_power(_attack_power),speed(_speed),name(_name){
	number = num;
}
zombie_info::~zombie_info() {
}


int role::flash_time_=100;
role::role(const char* _name,float _health,int _attack_power):name((char*)_name) {
	health = _health;
	attack_power = _attack_power;
	skill = true;
    body=new role_body();
    attack_time=new QTimer(this);
    attack_time->setInterval(flash_time_);
    attack_time->start();
    connect(attack_time,SIGNAL(timeout()),this,SLOT(timeout_attack()));
    fullAttackPower=_attack_power;
    fullHealth=_health;
}
void role::timeout_attack()
{

}
role::~role(){
    body->disconnect();
    delete body;
    attack_time->disconnect();
    delete attack_time;
}

bool role::ifDead() {
	return (health <= 0) ? true : false;
}

void role::setFlashTime(int n)
{
    flash_time_=n;
}

void role::setBodyFlashTime(int n)
{
    this->body->setFlashTime(n);
}

int role::bodyLevel() const
{
    return level;
}

int role::FlashTime()
{
    return flash_time_;
}

bool role::deHealth(float n) {
	if (health <= n) {
        health-=n;
		return false;
	}
	else {
		health -= n;
		return true;
	}
}
bool role::inHealth(float n) {
	if (health <= 0) {
		errlog("plant:try to increase a dead plant's health value!");
		return false;
	}
	else
	{
		health += n;
		return true;
    }
}

float role::FullHealth() const
{
    return fullHealth;
}

int role::FullAttackPower() const
{
    return fullAttackPower;
}

plant::plant(const char* _name, float _health, int _attack_power,int _price,locate<int,int> p):
    role(_name,_health,_attack_power),position(p){
	price = _price;
    protect_head_=nullptr;
    this->body->setWidth(screen::YardSize().width());
    this->body->setHeight(screen::YardSize().height()*4/5);
    this->body->setPos(screen::PlantBase().width()*1.15+screen::YardSize().width()*this->position.width,screen::PlantBase().height()*1.5+screen::YardSize().height()*this->position.high);
}

PumpKin *plant::ProtectHead() const
{
    return protect_head_;
}

void plant::setProtectHead(PumpKin *s)
{
    if(protect_head_)
    {
        protect_head_->disconnect();
        protect_head_=nullptr;
        delete protect_head_;
    }
    protect_head_=s;
    connect(protect_head_,SIGNAL(die(PumpKin*)),this,SLOT(protectHeadDestroy(PumpKin*)));
}

bool plant::deHealth(float s)
{
    if(protect_head_)
    {
        protect_head_->deHealth(s);
        return true;
    }
    else
    {
        return role::deHealth(s);
    }
}

void plant::timeout_attack()
{

}

void plant::pauseSlot()
{

}

void plant::continueSlot()
{

}

void plant::protectHeadDestroy(PumpKin *s)
{
    Q_UNUSED(s);
    protect_head_->disconnect();
    delete protect_head_;
    protect_head_=nullptr;
}
plant::~plant(){
    if(protect_head_)
    {
        protect_head_->disconnect();
        delete protect_head_;
    }
    disconnect();
}

zombie::zombie(const char* _name, float _health,int _attack_power,float _speed):
role(_name,_health,_attack_power){
    position = { (int)(clock()%screen::size_info.screen_high),(float)(screen::size_info.screen_width-0.1) };
	speed = _speed;
    status=zombie_status::walk;
    this->body->setPos(screen::ZombieBase().width()+screen::YardSize().width()*this->position.width,screen::ZombieBase().height()+screen::YardSize().height()*this->position.high);
    beIced=false;
    ice_clock_=new QTimer();
    ice_clock_->setInterval(1000);
    fullSpeed=_speed;
}
zombie::~zombie(){
    disconnect();
    ice_clock_->disconnect();
    delete ice_clock_;
}
bool zombie::move(const float d,yard_node *** yard) {
    if(position.width>=0&&position.width<=screen::Size().width())
    {
        const int pre = (position.width);
        position.width -= d;
        this->body->setPosByPosition({this->position.width,(qreal)this->position.high});
        if (position.width >= 0 && ( position.width < (float)screen::size_info.screen_width))
        {
            if (pre !=(int)(position.width) )
            {
                yard[position.high][pre]->pop_zombie(this);
                yard[position.high][(int)position.width]->push_zombie(this);
            }
        }
        return true;
    }
    if(position.width<0)
        emit(success());
    return false;
}

bool zombie::move_aside(yard_node*** yard)
{
	if (this->position.high > 0)
	{
        yard[position.high][(int)position.width]->pop_zombie(this);
        yard[position.high - 1][(int)position.width]->push_zombie(this);
        position.high-=1;
        body->MoveTop(screen::YardSize().height());
		return true;
	}
	else
	{
        yard[position.high][(int)position.width]->pop_zombie(this);
        yard[position.high + 1][(int)position.width]->push_zombie(this);
        position.high+=1;
        this->body->MoveDown(screen::YardSize().height());
		return true;
    }
}

void zombie::timeout_attack()
{
    if(this->status!=zombie_status::wait){
        if(game::game_yard[position.high][(int)position.width]->p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            if(status==zombie_status::attack)
                emit(attackToWalk());
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}

void zombie::walkToAttackSlot()
{
    status=zombie_status::attack;
}

void zombie::attackToWalkSlot()
{
    status=zombie_status::walk;
}

void zombie::runToPauseSlot()
{
    status=zombie_status::wait;
}

void zombie::pauseToRunSlot()
{
    status=zombie_status::walk;
}

void zombie::ice_clock_timeout()
{
    speed=speed*3;
    this->body->Movie()->setSpeed(100);
    ice_clock_->stop();
    beIced=false;
    disconnect(ice_clock_,SIGNAL(timeout()),this,SLOT(ice_clock_timeout()));
}

void zombie::beIce()
{
    if(beIced)
    {
        ice_clock_->stop();
        ice_clock_->start();
    }
    else
    {
        this->speed=speed/3;
        this->body->Movie()->setSpeed(33);
        ice_clock_->start();
        beIced=true;
        connect(ice_clock_,SIGNAL(timeout()),this,SLOT(ice_clock_timeout()));
    }
}

float zombie::FullSpeed() const
{
    return fullSpeed;
}

/*植物类*/
Sunflower::Sunflower(const plant_info& src,locate<int,int> p):plant(src.name,src.health,src.attack_power,src.price,p)
{
    ice_timer_=4;
    sun_timer_=new QTimer();
    sun_timer_->setInterval(ice_timer_*1000);
    sun_timer_->start();
    this->body->setMovie(":/image/plant/1/SunFlower1.gif");
    this->body->show();
    connect(sun_timer_,SIGNAL(timeout()),this,SLOT(sun_clock_timeout()));
}

void Sunflower::sun_clock_timeout()
{
    attack(100,game::game_yard);
}

void Sunflower::pauseSlot()
{
    sun_timer_->stop();

}

void Sunflower::continueSlot()
{
    sun_timer_->start();
}
bool Sunflower::attack(double time,yard_node *** yard)
{
    Q_UNUSED(time);
    Q_UNUSED(yard);
    emit(createBullet(this));
    return true;
}

Shoot::Shoot(const plant_info& src,locate<int,int> p):plant(src.name, src.health, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/0/Peashooter.gif");
    this->body->show();
    ice_time_=1.5;
    attack_clock_=new QTimer();
    attack_clock_->setInterval(ice_time_*1000);
    attack_clock_->start();
    connect(attack_clock_,SIGNAL(timeout()),this,SLOT(attack_clock_timeout()));
}

void Shoot::attack_clock_timeout()
{
    attack(100,game::game_yard);
}

void Shoot::pauseSlot()
{
    attack_clock_->stop();
}

void Shoot::continueSlot()
{
    attack_clock_->start();
}
bool Shoot::attack(double time, yard_node*** yard) {
    Q_UNUSED(yard);
    Q_UNUSED(time);
    emit(createBullet(this));
    return true;
}

Doubleshoot::Doubleshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.attack_power, src.price, p)
{
    ice_timer_=1.5;
    this->body->setMovie(":/image/plant/2/Repeater.gif");
    this->body->show();
    attack_clock_=new QTimer();
    attack_clock_->setInterval(ice_timer_*1500);
    attack_clock_->start();
    second_shoot_=new QTimer();
    second_shoot_->setInterval(200);
    second_shoot_->start();
    connect(attack_clock_,SIGNAL(timeout()),this,SLOT(attack_clock_timeout()));
}

void Doubleshoot::attack_clock_timeout()
{
    attack(100,game::game_yard);
    connect(second_shoot_,SIGNAL(timeout()),this,SLOT(second_shoot_timeout()));
}

void Doubleshoot::second_shoot_timeout()
{
    attack(100,game::game_yard);
    disconnect(second_shoot_,SIGNAL(timeout()),this,SLOT(second_shoot_timeout()));
}

void Doubleshoot::pauseSlot()
{
    attack_clock_->stop();
    second_shoot_->stop();
}

void Doubleshoot::continueSlot()
{
    attack_clock_->start();
    second_shoot_->start();
}
bool Doubleshoot::attack(double time, yard_node*** yard) {
    Q_UNUSED(yard);
    Q_UNUSED(time);
    emit(createBullet(this));
    return true;
}

Iceshoot::Iceshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.attack_power, src.price, p)
{
    ice_time_ = 1.5;
    attack_clock_=new QTimer();
    attack_clock_->setInterval(ice_time_*1000);
    this->body->setMovie(":/image/plant/3/SnowPea.gif");
    this->body->show();
    attack_clock_->start();
    connect(attack_clock_,SIGNAL(timeout()),this,SLOT(attack_clock_timeout()));
}

void Iceshoot::attack_clock_timeout()
{
    attack(100,game::game_yard);
}

void Iceshoot::pauseSlot()
{
    attack_clock_->stop();
}

void Iceshoot::continueSlot()
{
    attack_clock_->start();
}
bool Iceshoot::attack(double time, yard_node*** yard) {
    Q_UNUSED(time);
    Q_UNUSED(yard);
    emit(createBullet(this));
    return true;
}



Normal::Normal(zombie_info& k):zombie(k.name,k.health,k.attack_power,k.speed) {
    this->body->setMovie(":/image/zombie/5/Zombie2.gif");
    this->body->show();

}
bool Normal::attack(double time,yard_node*** yard)
{
    if (yard[position.high][(int)position.width]->p != nullptr){
        yard[position.high][(int)position.width]->p->deHealth((float)(time / 1000) * attack_power);
        if(yard[position.high][(int)position.width]->p->ifDead())
        {
            emit(plantDie(yard[position.high][(int)position.width]->p));
            emit(attackToWalk());
        }
    }
    return true;
}

void Normal::walkToAttackSlot()
{
    status=zombie_status::attack;
    this->body->setMovie(":/image/zombie/5/ZombieAttack.gif");
}

void Normal::attackToWalkSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/5/Zombie.gif");
}

void Normal::runToPauseSlot()
{
    status=zombie_status::wait;
    this->body->setMovie(":/image/zombie/5/3.gif");
}

void Normal::pauseToRunSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/5/Zombie.gif");
}

bool Nut::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    Q_UNUSED(yard);
	return true;
}
Nut::Nut(const plant_info& src, locate<int, int> p):plant(src.name, src.health, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/4/WallNut.gif");
    this->body->show();
}

void Nut::timeout_attack()
{
    if(health<(fullHealth*2/3)&&level<2)
    {
        this->body->setMovie(":/image/plant/4/Wallnut_cracked1.gif");
        level=2;
    }
    else if(health<(fullHealth/3)&&level<3)
    {
        this->body->setMovie(":/image/plant/4/Wallnut_cracked2.gif");
        level=3;
    }
}

bool Highnut::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    Q_UNUSED(yard);
	return true;
}

Highnut::Highnut(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/5/TallNut.gif");
    this->body->show();
}

void Highnut::timeout_attack()
{
    if(health<(fullHealth*2/3)&&level<2)
    {
        this->body->setMovie(":/image/plant/5/TallnutCracked1.gif");
        level=2;
    }
    else if(health<(fullHealth/3)&&level<3)
    {
        this->body->setMovie(":/image/plant/5/TallnutCracked2.gif");
        level=3;
    }
}


void yard_node::kii_all_zombie()
{
    find_first(0);
	if (first == -1)
	{
		return;
	}
	else
	{
		for(int i=0;i<10;i++)
            if (z[i] != nullptr)
			{
                z[i]->deHealth(1000);
                emit(zombieDie(z[i]));
            }
		first = -1;
	}
}

void yard_node::kill_plant()
{
    if (p != nullptr)
	{
        p->deHealth(1000);
        emit(plantDie(p));
	}
}
void yard_node::find_first(double time)
{
	float min = 100;
	int _first = -1;
	for (int j = 0; j < 10; j++)
        if (z[j] != nullptr && z[j]->get_position().width < min)
		{
			min = z[j]->get_position().width;
			_first = j;
		}
	first = _first;
    Q_UNUSED(time);
}


Wogua::Wogua(const plant_info& src,locate<int,int> p):plant(src.name, src.health, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/6/Squash.gif");
    this->body->show();
    this->body->setScale(1.5);
    this->body->MoveTop(screen::YardSize().height()/5);
    this->body->MoveHead(screen::YardSize().width()/8);
    attacking=false;
}

void Wogua::animationEndSlot()
{
    for(auto i:game::game_yard[position.high][position.width]->z)
    {
        if(i)
        {
            i->deHealth(10000);
            emit(zombieDie(i));
        }
    }
    emit(die(this));
}

void Wogua::timeout_attack()
{
    attack(100,game::game_yard);
}

void Wogua::pauseSlot()
{

}

void Wogua::continueSlot()
{

}
bool Wogua::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    if(!attacking)
    {
        yard[position.high][position.width]->find_first(0);
        if (yard[position.high][position.width]->first != -1)
        {
            this->body->setMovie(":/image/plant/6/SquashAttack.gif");
            connect(this->body->Movie(),SIGNAL(finished()),this,SLOT(animationEndSlot()));
            attacking=true;
        }
    }
	return true;
}

Cherrybomb::Cherrybomb(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/7/CherryBomb.gif");
    this->body->show();
}

void Cherrybomb::animationEndSlot(role_body* s)
{
    Q_UNUSED(s)
    emit(die(this));
}

void Cherrybomb::timeout_attack()
{
    if(!attacking)
        attack(100,game::game_yard);
}

void Cherrybomb::pauseSlot()
{

}

void Cherrybomb::continueSlot()
{

}
bool Cherrybomb::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
	for(int i=position.high-1;i<=position.high+1;i++)
		for (int j = position.width - 1; j <= position.width + 1; j++)
		{
			if (i >= 0 && i < screen::size_info.screen_high && j >= 0 && j < screen::size_info.screen_width)
                yard[i][j]->kii_all_zombie();
		}
    this->body->setMovie(":/image/plant/7/Boom.gif");
    this->body->setTimer(new QTimer());
    this->body->Timer()->setInterval(1000);
    this->body->Timer()->start();
    connect(this->body,SIGNAL(end(role_body*)),this,SLOT(animationEndSlot(role_body*)));
    attacking=true;
    return true;
}

Garlic::Garlic(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.attack_power, src.price, p)
{
	skill = true;
    this->body->setMovie(":/image/plant/8/Garlic.gif");
    this->body->show();
}

void Garlic::timeout_attack()
{
    attack(100,game::game_yard);
}
bool Garlic::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    if(health<(fullHealth/3)&&skill==true)
    {
        for(auto i:yard[position.high][position.width]->z)
        {
            if(i)
            {
                i->move_aside(yard);
                i->attackToWalk();
            }
            skill=false;
        }
    }
	return true;
}

Conehead::Conehead(zombie_info& k):zombie(k.name, k.health, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/0/ConeheadZombie.gif");
    this->body->show();
}
bool Conehead::attack(double time, yard_node*** yard)
{

    if (yard[position.high][(int)position.width]->p != nullptr)
        if (!yard[position.high][(int)position.width]->p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width]->p));
            emit(attackToWalk());
		}
    return true;
}

void Conehead::walkToAttackSlot()
{
    status=zombie_status::attack;
    if(name!=(string)"normal")
        this->body->setMovie(":/image/zombie/0/ConeheadZombieAttack.gif");
    else
        this->body->setMovie(":/image/zombie/5/ZombieAttack.gif");
}

void Conehead::attackToWalkSlot()
{
    status=zombie_status::walk;
    if(name!=(string)"normal")
        this->body->setMovie(":/image/zombie/0/ConeheadZombie.gif");
    else
        this->body->setMovie(":/image/zombie/5/Zombie.gif");
}

void Conehead::runToPauseSlot()
{
    status=zombie_status::wait;
    if(name!=(string)"normal")
        this->body->setMovie(":/image/zombie/0/1.gif");
    else
        this->body->setMovie(":/image/zombie/5/3.gif");
}

void Conehead::pauseToRunSlot()
{
    status=zombie_status::walk;
    if(name!=(string)"normal")
        this->body->setMovie(":/image/zombie/0/ConeheadZombie.gif");
    else
        this->body->setMovie(":/image/zombie/5/Zombie.gif");
}

void Conehead::timeout_attack()
{
    if ((health <= fullHealth/3)&&name!=(string)"normal")
    {
        name = (char*)"normal";
        attackToWalk();
    }
    if(this->status!=zombie_status::wait){
        if(game::game_yard[position.high][(int)position.width]->p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            if(status==zombie_status::attack)
                emit(attackToWalk());
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}
Reading::Reading(zombie_info& k) :zombie(k.name, k.health, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
    this->body->show();
    loseNewspaper=false;
}
bool Reading::attack(double time, yard_node*** yard)
{

    if (yard[position.high][(int)position.width]->p != nullptr)
        if (!yard[position.high][(int)position.width]->p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width]->p));
            emit(attackToWalk());
		}
    return true;
}

void Reading::walkToAttackSlot()
{
    status=zombie_status::attack;
    if(speed>fullSpeed*1.2)
        this->body->setMovie(":/image/zombie/1/HeadAttack0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadAttack1.gif");
}

void Reading::attackToWalkSlot()
{
    status=zombie_status::walk;
    if(speed>fullSpeed*1.2)
        this->body->setMovie(":/image/zombie/1/HeadWalk0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
}

void Reading::runToPauseSlot()
{
    status=zombie_status::wait;
    if(speed>fullSpeed*1.2)
        this->body->setMovie(":/image/zombie/1/1.gif");
    else
        //there is no source of zombie wait with no newspaper...
        this->body->setMovie(":/image/zombie/1/1.gif");
}

void Reading::pauseToRunSlot()
{
    status=zombie_status::walk;
    if(speed>fullSpeed*1.2)
        this->body->setMovie(":/image/zombie/1/HeadWalk0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
}

void Reading::timeout_attack()
{
    if ((health <= fullHealth*2/3)&&loseNewspaper==false)
    {
        speed = fullSpeed*3/2;
        loseNewspaper=true;
        attackToWalk();
    }
    if(this->status!=zombie_status::wait){
        if(game::game_yard[position.high][(int)position.width]->p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            if(status==zombie_status::attack)
                emit(attackToWalk());
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}
Pole::Pole(zombie_info& k) :zombie(k.name, k.health, k.attack_power, k.speed)
{
	skill = true;
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombie.gif");
    this->body->show();
}
bool Pole::attack(double time, yard_node*** yard)
{
    if(jumping==true)
    {
        return true;
    }
    if (skill == true && jumping==false)
	{
        if (yard[position.high][(int)position.width]->p != nullptr)
		{
            this->body->setMovie(":/image/zombie/2/PoleVaultingZombieJump.gif");
            connect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump0Finished()));
            jumping=true;
            return true;
		}
	}
    if (yard[position.high][(int)position.width]->p != nullptr)
        if (!yard[position.high][(int)position.width]->p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width]->p));
            emit(attackToWalk());
        }
    return true;
}
void Pole::walkToAttackSlot()
{
    status=zombie_status::attack;
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieAttack.gif");
}

void Pole::attackToWalkSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
}

void Pole::runToPauseSlot()
{
    status=zombie_status::wait;
    if(skill==true)
    {
        this->body->setMovie(":/image/zombie/2/1.gif");
    }
    else
    {
        this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
    }
}

void Pole::pauseToRunSlot()
{
    status=zombie_status::walk;
    if(skill==true)
    {
        this->body->setMovie(":/image/zombie/2/PoleVaultingZombie.gif");
    }
    else
    {
        this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
    }
}

void Pole::dealJump0Finished()
{
    if(game::game_yard[position.high][(int)position.width]->p!=nullptr &&
            game::game_yard[position.high][(int)position.width]->p->getName()==(string)"highnut")
    {
        skill=false;
        jumping=false;
        disconnect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump0Finished()));
        this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
        speed=fullSpeed/2;
        return ;
    }
    disconnect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump0Finished()));
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieJump2.gif");
    connect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump1Finished()));
    this->body->MoveHead(screen::YardSize().width()*2/3);
}

void Pole::dealJump1Finished()
{
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
    disconnect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump1Finished()));
    speed=fullSpeed/2;
    jumping=false;
    skill=false;
    this->move(1,game::game_yard);
}

void Pole::timeout_attack()
{
    if(this->status!=zombie_status::wait&&jumping==false){
        if(game::game_yard[position.high][(int)position.width]->p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            if(status==zombie_status::attack)
                emit(attackToWalk());
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}

Clown::Clown(zombie_info& k) :zombie(k.name, k.health, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/3/Walk.gif");
    this->body->show();
    bomb_clock_=new QTimer();
    open_box_=new QTimer();
    int i=QRandomGenerator::global()->bounded(10)+15;
    open_box_->setInterval(i*1000);
    open_box_->start();
    bomb_clock_->setInterval(1000);
    connect(this->open_box_,SIGNAL(timeout()),this,SLOT(openBox()));
}
bool Clown::attack(double time, yard_node*** yard) {
    if (yard[position.high][(int)position.width]->p != nullptr)
        if (!yard[position.high][(int)position.width]->p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width]->p));
            emit(attackToWalk());
		}
    return true;

}

void Clown::walkToAttackSlot()
{
    status=zombie_status::attack;
    this->body->setMovie(":/image/zombie/3/Attack.gif");
}

void Clown::attackToWalkSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/3/Walk.gif");
}

void Clown::runToPauseSlot()
{
    status=zombie_status::wait;
    this->bomb_clock_->stop();
    this->open_box_->stop();
    this->body->setMovie(":/image/zombie/3/1.gif");
}

void Clown::pauseToRunSlot()
{
    status=zombie_status::walk;
    this->bomb_clock_->start();
    this->open_box_->start();
    this->body->setMovie(":/image/zombie/3/Walk.gif");
}

void Clown::bomb()
{
    this->body->setMovie(":/image/zombie/3/Boom.gif");
    disconnect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bomb()));
    connect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bombEnd()));
    this->health-=1000;
    if(this->position.width-1>=0)
    {
        game::game_yard[position.high][(int)position.width-1]->kill_plant();
    }
    if(this->position.width+1<=screen::size_info.screen_width)
    {
        game::game_yard[position.high][(int)position.width+1]->kill_plant();
    }
    if(this->position.high+1<screen::size_info.screen_high)
    {
        game::game_yard[position.high+1][(int)position.width]->kill_plant();
    }
    if(this->position.high-1>=0)
    {
        game::game_yard[position.high-1][(int)position.width]->kill_plant();
    }
    game::game_yard[position.high][(int)position.width]->kill_plant();
}

void Clown::openBox()
{
    status=zombie_status::wait;
    this->body->setMovie(":/image/zombie/3/OpenBox.gif");
    disconnect(this->open_box_,SIGNAL(timeout()),this,SLOT(openBox()));
    connect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bomb()));
    bomb_clock_->start();
}

void Clown::bombEnd()
{
    emit(die(this));
}

Throwstone::Throwstone(zombie_info& k) :zombie(k.name, k.health, k.attack_power, k.speed)
{
    this->body->setWidth(200);
    this->body->setHeight(200);
    this->body->setMovie(":/image/zombie/4/2.gif");
    this->body->show();
}
bool Throwstone::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    if(yard[position.high][(int)this->position.width]->p!=nullptr)
    {
        if(yard[position.high][(int)position.width]->p->getName()==(string)"cherrybomb")
        {
            return false;
        }
        if(yard[position.high][(int)position.width]->p->getName()==(string)"wogua")
        {
            return false;
        }
        emit(plantDie(yard[position.high][(int)this->position.width]->p));
    }
    return true;
}

void Throwstone::walkToAttackSlot()
{
    status=zombie_status::attack;
}

void Throwstone::attackToWalkSlot()
{
    status=zombie_status::walk;
}

void Throwstone::runToPauseSlot()
{
    status=zombie_status::wait;
    this->body->setMovie(":/image/zombie/4/1.gif");
}

void Throwstone::pauseToRunSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/4/2.gif");
}

Bullet::Bullet(const char* _name, int _attack_power, float _speed,locate<int,float> p):
    role(_name,1000,_attack_power),position(p),speed_(_speed)
{
	position.high = p.high;
	position.width = p.width;
    status=zombie_status::walk;
    this->body->setHeight(40);
    this->body->setWidth(40);
    this->body->setPos(screen::ZombieBase().width()+screen::YardSize().width()*this->position.width,screen::ZombieBase().height()+screen::YardSize().height()*this->position.high);
    if(this->name==(string)"GreenBullet")
        this->body->setMovie(":/image/bullet/PB00.gif");
    else if(this->name==(string)"BlueBullet")
        this->body->setMovie(":/image/bullet/PB-10.gif");
    this->body->show();
}
bool Bullet::attack(double time, yard_node*** yard)
{
    Q_UNUSED(time);
    yard[position.high][(int)position.width]->find_first(0);
    int index=yard[this->position.high][(int)this->position.width]->first;
    if(index!=-1)
    {
        yard[this->position.high][(int)position.width]->z[index]->deHealth(this->attack_power);
        if(this->name==(string)"BlueBullet")
            yard[this->position.high][(int)position.width]->z[index]->beIce();
        if(yard[this->position.high][(int)position.width]->z[index]->ifDead())
            emit(zombieDie(yard[this->position.high][(int)position.width]->z[index]));
        emit(die(this));
    }
    else{
        QPointF p=this->body->pos();
        p.setX(p.x()+this->speed_*this->FlashTime()/1000*screen::YardSize().width());
        this->body->setPos(p);
        this->position.width+=this->speed_*this->FlashTime()/1000;
        if(this->position.width<0||this->position.width>=screen::size_info.screen_width)
            emit(die(this));
    }
    return true;
}

void Bullet::pauseToRunSlot()
{
    this->status=zombie_status::walk;
}

void Bullet::runToPauseSlot()
{
    this->status=zombie_status::wait;
}

void Bullet::timeout_attack()
{
    if(status==zombie_status::walk)
    {
        attack(100,game::game_yard);
    }
    else
    {
        return;
    }
}

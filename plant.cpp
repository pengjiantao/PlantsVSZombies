#include"plant.h"
#include"swfunix.h"
#include"toolfunc.h"
#include"screen.h"
#include"role_body.h"
#include"PlantVSZombie.h"
using namespace std;

template struct locate<int, int>;
template struct locate<int,float>;

plant_info::plant_info(const char* _name, int _health, int _attack_power, obj_color _color, int _price) :
	name(_name),health(_health),attack_power(_attack_power),color(_color),price(_price),ice_time(_price/50){
    wait=ice_time;
}
plant_info::~plant_info(){}

bool zombie_info::ALL_ZOMBIE = false;
zombie_info::zombie_info(const int _health,const int _attack_power,const float _speed,const char* _name,const obj_color _color,const int num):
health(_health),attack_power(_attack_power),speed(_speed),name(_name),color(_color){
	number = num;
}
zombie_info::~zombie_info() {
}


int role::flash_time_=100;
role::role(const char* _name,float _health,obj_color _color,int _attack_power):name((char*)_name) {
	health = _health;
	color = _color;
	attack_power = _attack_power;
	skill = true;
    body=new role_body();
    attack_time=new QTimer(this);
    attack_time->setInterval(flash_time_);
    attack_time->start();
    connect(attack_time,SIGNAL(timeout()),this,SLOT(timeout_attack()));
}
void role::timeout_attack()
{

}
role::~role(){
    delete body;
    delete attack_time;
}

bool role::ifDead() {
	return (health <= 0) ? true : false;
}

void role::setFlashTime(int n)
{
    flash_time_=n;
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

plant::plant(const char* _name, float _health, obj_color _objcolor,int _attack_power,int _price,locate<int,int> p):
	role(_name,_health,_objcolor,_attack_power),position(p){
	price = _price;
    this->body->setWidth(80);
    this->body->setHeight(80);
    this->body->setPos(screen::PlantBase().width()*1.15+screen::YardSize().width()*this->position.width,screen::PlantBase().height()*1.5+screen::YardSize().height()*this->position.high);
}

void plant::timeout_attack()
{

}
plant::~plant() = default;

zombie::zombie(const char* _name, float _health, obj_color _objcolor,int _attack_power,float _speed):
role(_name,_health,_objcolor,_attack_power){
    position = { (int)(clock()%screen::size_info.screen_high),(float)(screen::size_info.screen_width-0.1) };
	speed = _speed;
    status=zombie_status::walk;
    this->body->setPos(screen::ZombieBase().width()+screen::YardSize().width()*this->position.width,screen::ZombieBase().height()+screen::YardSize().height()*this->position.high);
}
zombie::~zombie() = default;
bool zombie::move(const float d,yard_node ** yard) {
    if(position.width>=0&&position.width<=screen::Size().width())
    {
        const int pre = (int)position.width;
        position.width -= d;
        this->body->setPosByPosition({this->position.width,(qreal)this->position.high});
        if (position.width >= 0 && ( position.width < (float)screen::size_info.screen_width))
        {
            if (pre !=(int)(position.width) )
            {
                yard[position.high][pre].pop_zombie(this);
                yard[position.high][(int)position.width].push_zombie(this);
            }
        }
        return true;
    }
    if(position.width<0)
        emit(success());
    return false;
}

bool zombie::move_aside(yard_node** yard)
{
	if (this->position.high > 0)
	{
		yard[position.high][(int)position.width].pop_zombie(this);
		yard[position.high - 1][(int)position.width].push_zombie(this);
		return true;
	}
	else
	{
		yard[position.high][(int)position.width].pop_zombie(this);
		yard[position.high + 1][(int)position.width].push_zombie(this);
		return true;
    }
}

void zombie::timeout_attack()
{
    if(this->status!=zombie_status::wait){
        if(game::game_yard[position.high][(int)position.width].p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}

void zombie::walkToAttackSlot()
{
    status=zombie_status::attack;
    cout<<"walk to attack"<<endl;
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

/*植物类*/
Sunflower::Sunflower(const plant_info& src,locate<int,int> p):plant(src.name,src.health,src.color,src.attack_power,src.price,p)
{
	iceTime = 3;
	last_time_create_money = GetTickCount64();
    this->body->setMovie(":/image/plant/1/SunFlower1.gif");
    this->body->show();
}
bool Sunflower::attack(double time,yard_node ** yard)
{
	if ((GetTickCount64() - last_time_create_money) / 1000 >= iceTime)
	{
		last_time_create_money = GetTickCount64();
		return false;
	}
	return true;
}

Shoot::Shoot(const plant_info& src,locate<int,int> p):plant(src.name, src.health, src.color, src.attack_power, src.price, p)
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
bool Shoot::attack(double time, yard_node** yard) {
    Q_UNUSED(yard);
    Q_UNUSED(time);
    emit(createBullet(this));
    return true;
}

Doubleshoot::Doubleshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
	iceTime = 1.0;
	last_time_of_shoot = GetTickCount64();
    this->body->setMovie(":/image/plant/2/Repeater.gif");
    this->body->show();
}
bool Doubleshoot::attack(double time, yard_node** yard) {
	color = green;
	if ((float)(GetTickCount64() - last_time_of_shoot) / 1000 > iceTime) {
        auto* b = new Bullet("@", yellow, attack_power, -4,  { position.high,(float)position.width });
        b = new Bullet("@", yellow, attack_power, -4,  { position.high,(float)position.width });
		last_time_of_shoot = GetTickCount64();
		return true;
	}
	return true;
}

Iceshoot::Iceshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
	iceTime = 1.0;
	last_time_of_shoot = GetTickCount64();
    this->body->setMovie(":/image/plant/3/SnowPea.gif");
    this->body->show();
}
bool Iceshoot::attack(double time, yard_node** yard) {
	color = green;
	if ((float)(GetTickCount64() - last_time_of_shoot) / 1000 > iceTime) {
        Bullet* b = new Bullet("@", lightblue, attack_power, -4,  { position.high,(float)position.width });
		last_time_of_shoot = GetTickCount64();
		return true;
	}
	return true;
}



Normal::Normal(zombie_info& k):zombie(k.name,k.health,k.color,k.attack_power,k.speed) {
    this->body->setMovie(":/image/zombie/5/Zombie2.gif");
    this->body->show();

}
bool Normal::attack(double time,yard_node** yard)
{
    if (yard[position.high][(int)position.width].p != nullptr){
        yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power);
        if(yard[position.high][(int)position.width].p->ifDead())
        {
            emit(plantDie(yard[position.high][(int)position.width].p));
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

bool Nut::attack(double time, yard_node** yard)
{
    Q_UNUSED(time);
    Q_UNUSED(yard);
	color = green;
	return true;
}
Nut::Nut(const plant_info& src, locate<int, int> p):plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/4/WallNut.gif");
    this->body->show();
}

bool Highnut::attack(double time, yard_node** yard)
{
	color = green;
	return true;
}

Highnut::Highnut(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/5/TallNut.gif");
    this->body->show();
}


void yard_node::kii_all_zombie()
{
	if (first == -1)
	{
		return;
	}
	else
	{
		for(int i=0;i<10;i++)
			if (z[i] != NULL)
			{
				delete z[i];
				z[i] = NULL;
				color = red;
				time_of_killzombies = GetTickCount64();
			}
			else {
				color = red;
				time_of_killzombies = GetTickCount64();
			}
		first = -1;
	}
	find_first(0);
}

void yard_node::kill_plant()
{
	if (p != NULL)
	{
		delete p;
		p = NULL;
		color = red;
		time_of_killzombies = GetTickCount64();
	}
}
void yard_node::find_first(double time)
{
	float min = 100;
	int _first = -1;
	for (int j = 0; j < 10; j++)
		if (z[j] != NULL && z[j]->get_position().width < min && z[j]->getName()!="@")
		{
			min = z[j]->get_position().width;
			_first = j;
		}
	first = _first;
	if (color==red&&(GetTickCount64() - time_of_killzombies)/1000 > effect_time)
		color = black;
	if (p != NULL && p->skill==false)
	{
		delete p;
		p = NULL;
	}
	for (int i = 0; i < 10; i++)
	{
		if (iz[i] > 0)
		{
			iz[i] -= time;
			if (iz[i] <= 0)
			{
				z[i]->deSpeed(-z[i]->getSpeed());
			}
		}

	}
}

void yard_node::icefirstzombie()
{
	if (first != -1)
	{
		z[first]->deSpeed(z[first]->getSpeed() / 2);
		iz[first] = effect_time*1000;
	}
}

Wogua::Wogua(const plant_info& src,locate<int,int> p):plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/6/Squash.gif");
    this->body->show();
}
bool Wogua::attack(double time, yard_node** yard)
{
	if (yard[position.high][position.width].first != -1)
	{
		yard[position.high][position.width].kii_all_zombie();
		skill = false;
		yard[position.high][position.width].find_first(0);
	}
	else if((position.width+1)<screen::size_info.screen_width && yard[position.high][position.width+1].first!=-1)
	{
		yard[position.high][position.width + 1].kii_all_zombie();
		skill = false;
		yard[position.high][position.width+1].find_first(0);
	}
	return true;
}

Cherrybomb::Cherrybomb(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
    this->body->setMovie(":/image/plant/7/CherryBomb.gif");
    this->body->show();
}
bool Cherrybomb::attack(double time, yard_node** yard)
{
	for(int i=position.high-1;i<=position.high+1;i++)
		for (int j = position.width - 1; j <= position.width + 1; j++)
		{
			if (i >= 0 && i < screen::size_info.screen_high && j >= 0 && j < screen::size_info.screen_width)
				yard[i][j].kii_all_zombie();
		}
	skill = false;
	return true;
}

Garlic::Garlic(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
	skill = true;
    this->body->setMovie(":/image/plant/8/Garlic.gif");
    this->body->show();
}
bool Garlic::attack(double time, yard_node** yard)
{
	if (skill == true && yard[position.high][position.width].first != -1)
	{
		yard[position.high][position.width].z[yard[position.high][position.width].first]->move_aside(yard);
		skill = false;
	}
	return true;
}

Conehead::Conehead(zombie_info& k):zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/0/ConeheadZombie.gif");
    this->body->show();
}
bool Conehead::attack(double time, yard_node** yard)
{
    if (health <= 100&&name!=(string)"normal")
	{
        cout<<"yes"<<endl;
        name = (char*)"normal";
        this->body->setMovie(":/image/zombie/5/Zombie.gif");
	}
    if (yard[position.high][(int)position.width].p != nullptr)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width].p));
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
Reading::Reading(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
    this->body->show();
}
bool Reading::attack(double time, yard_node** yard)
{
	if (health <= 100)
		speed = 0.45;
    if (yard[position.high][(int)position.width].p != nullptr)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width].p));
            emit(attackToWalk());
		}
    return true;
}

void Reading::walkToAttackSlot()
{
    status=zombie_status::attack;
    if(speed==0.45)
        this->body->setMovie(":/image/zombie/1/HeadAttack0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadAttack1.gif");
}

void Reading::attackToWalkSlot()
{
    status=zombie_status::walk;
    if(speed==0.45)
        this->body->setMovie(":/image/zombie/1/HeadWalk0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
}

void Reading::runToPauseSlot()
{
    status=zombie_status::wait;
    if(speed==0.45)
        this->body->setMovie(":/image/zombie/1/1.gif");
    else
        //there is no source of zombie wait with no newspaper...
        this->body->setMovie(":/image/zombie/1/1.gif");
}

void Reading::pauseToRunSlot()
{
    status=zombie_status::walk;
    if(speed==0.45)
        this->body->setMovie(":/image/zombie/1/HeadWalk0.gif");
    else
        this->body->setMovie(":/image/zombie/1/HeadWalk1.gif");
}
Pole::Pole(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
	skill = true;
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombie.gif");
    this->body->show();
}
bool Pole::attack(double time, yard_node** yard)
{
    if(jumping==true)
    {
        return true;
    }
    if (skill == true && jumping==false)
	{
        if (yard[position.high][(int)position.width].p != nullptr)
		{
			yard[position.high][(int)position.width].pop_zombie(this);
            this->body->setMovie(":/image/zombie/2/PoleVaultingZombieJump.gif");
            connect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump0Finished()));
            jumping=true;
            return true;
		}
	}
    if (yard[position.high][(int)position.width].p != nullptr)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width].p));
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
    disconnect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump0Finished()));
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieJump2.gif");
    connect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump1Finished()));
    this->body->MoveHead(screen::YardSize().width()*2/3);
}

void Pole::dealJump1Finished()
{
    this->position.width-=1;
    game::game_yard[position.high][(int)position.width].push_zombie(this);
    this->body->setMovie(":/image/zombie/2/PoleVaultingZombieWalk.gif");
    disconnect(this->body->Movie(),SIGNAL(finished()),this,SLOT(dealJump1Finished()));
    speed=speed/2;
    jumping=false;
    skill=false;
}

void Pole::timeout_attack()
{
    if(this->status!=zombie_status::wait&&jumping==false){
        if(game::game_yard[position.high][(int)position.width].p!=nullptr)
        {
            if(status!=zombie_status::attack)
                emit(walkToAttack());
            this->attack((double)role::FlashTime(),game::game_yard);
        }
        else{
            this->move((float)role::FlashTime()*speed/1000,game::game_yard);
        }
    }
}

Clown::Clown(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
    this->body->setMovie(":/image/zombie/3/Walk.gif");
    this->body->show();
    bomb_clock_=new QTimer(this);
    open_box_=new QTimer(this);
    int i=qrand()%10+15;
    open_box_->setInterval(i*1000);
    open_box_->start();
    bomb_clock_->setInterval(1500);


    connect(this->open_box_,SIGNAL(timeout()),this,SLOT(openBox()));
}
bool Clown::attack(double time, yard_node** yard) {
    if (yard[position.high][(int)position.width].p != nullptr)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
            emit(plantDie(yard[position.high][(int)position.width].p));
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
    this->body->setMovie(":/image/zombie/3/1.gif");
}

void Clown::pauseToRunSlot()
{
    status=zombie_status::walk;
    this->body->setMovie(":/image/zombie/3/Walk.gif");
}

void Clown::bomb()
{
    this->body->setMovie(":/image/zombie/3/Boom.gif");
    connect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bombEnd()));
    disconnect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bomb()));
    this->health=-1000;
    if(this->position.width-1>=0)
    {
        game::game_yard[position.high][(int)position.width-1].kill_plant();
    }
    if(this->position.width+1<=screen::size_info.screen_width)
    {
        game::game_yard[position.high][(int)position.width+1].kill_plant();
    }
    if(this->position.high+1<screen::size_info.screen_high)
    {
        game::game_yard[position.high+1][(int)position.width].kill_plant();
    }
    if(this->position.high-1>=0)
    {
        game::game_yard[position.high-1][(int)position.width].kill_plant();
    }
    game::game_yard[position.high][(int)position.width].kill_plant();
}

void Clown::openBox()
{
    status=zombie_status::wait;
    this->body->setMovie(":/image/zombie/3/OpenBox.gif");
    connect(this->bomb_clock_,SIGNAL(timeout()),this,SLOT(bomb()));
    disconnect(this->open_box_,SIGNAL(timeout()),this,SLOT(openBox()));
    bomb_clock_->start();
}

void Clown::bombEnd()
{
    emit(die(this));
}

Throwstone::Throwstone(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
    this->body->setWidth(200);
    this->body->setHeight(200);
    this->body->setMovie(":/image/zombie/4/2.gif");
    this->body->show();
}
bool Throwstone::attack(double time, yard_node** yard)
{
    Q_UNUSED(time);
    if(yard[position.high][(int)this->position.width].p!=nullptr)
    {
        emit(plantDie(yard[position.high][(int)this->position.width].p));
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

Bullet::Bullet(const char* _name, obj_color _color, int _attack_power, float _speed,locate<int,float> p):
    role(_name,1000,_color,_attack_power),position(p),speed_(_speed)
{
	position.high = p.high;
	position.width = p.width;
    status=zombie_status::walk;
    this->body->setHeight(40);
    this->body->setWidth(40);
    this->body->setPos(screen::ZombieBase().width()+screen::YardSize().width()*this->position.width,screen::ZombieBase().height()+screen::YardSize().height()*this->position.high);
    if(this->name==(string)"GreenBullet")
        this->body->setMovie(":/image/bullet/PB10.gif");
    else if(this->name==(string)"BlueBullet")
        this->body->setMovie(":/image/bullet/PB10.gif");
    this->body->show();
}
bool Bullet::attack(double time, yard_node** yard)
{
    Q_UNUSED(time);
    yard[position.high][(int)position.width].find_first(0);
    int index=yard[this->position.high][(int)this->position.width].first;
    if(index!=-1)
    {
        yard[this->position.high][(int)position.width].z[index]->deHealth(this->attack_power);
        if(yard[this->position.high][(int)position.width].z[index]->ifDead())
            emit(zombieDie(yard[this->position.high][(int)position.width].z[index]));
        emit(die(this));
    }
    else{
        QPointF p=this->body->pos();
        p.setX(p.x()+this->speed_*this->body->FlashTime()/1000*screen::YardSize().width());
        this->body->setPos(p);
        this->position.width+=this->speed_*this->body->FlashTime()/1000;
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

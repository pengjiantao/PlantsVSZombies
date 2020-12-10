#include"plant.h"
#include"swfunix.h"
#include"toolfunc.h"
#include"screen.h"
using namespace std;
/*显示实例化：plant locate*/
template struct locate<int, int>;

/*显式实例化：zombie locate*/
template struct locate<int,float>;

plant_info::plant_info(const char* _name, int _health, int _attack_power, obj_color _color, int _price) :
	name(_name),health(_health),attack_power(_attack_power),color(_color),price(_price),ice_time(_price/50){
	last_create = GetTickCount64();
}
plant_info::~plant_info(){}

bool zombie_info::ALL_ZOMBIE = false;
zombie_info::zombie_info(const int _health,const int _attack_power,const float _speed,const char* _name,const obj_color _color,const int num):
health(_health),attack_power(_attack_power),speed(_speed),name(_name),color(_color){
	number = num;
}
zombie_info::~zombie_info() {
}

role::role(const char* _name,float _health,obj_color _color,int _attack_power):name((char*)_name) {
	health = _health;
	color = _color;
	attack_power = _attack_power;
	skill = true;
}
bool role::ifDead() {
	return (health <= 0) ? true : false;
}
bool role::deHealth(float n) {
	if (health <= n) {
		//errlog("plant:try to decrease a dead plant's health value!");
		return false;
	}
	else {
		health -= n;
		color = red;
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
}
plant::~plant() = default;

zombie::zombie(const char* _name, float _health, obj_color _objcolor,int _attack_power,float _speed):
role(_name,_health,_objcolor,_attack_power){
    position = { (int)(clock()%screen::size_info.screen_high),(float)(screen::size_info.screen_width-0.1) };
	speed = _speed;
}
zombie::~zombie() = default;
bool zombie::move(const float d,yard_node ** yard) {
	const int pre = (int)position.width;
	position.width -= d;
	
	if (position.width >= 0 && ( position.width < (float)screen::size_info.screen_width))
	{
		if (pre !=(int)(position.width) )
		{
			yard[position.high][pre].pop_zombie(this);
			yard[position.high][(int)position.width].push_zombie(this);
		}
		return false;
	}
	else {
		return true;
	}
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

/*植物类*/
Sunflower::Sunflower(const plant_info& src,locate<int,int> p):plant(src.name,src.health,src.color,src.attack_power,src.price,p)
{
	iceTime = 3;
	last_time_create_money = GetTickCount64();
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
	iceTime = 1.0;
	last_time_of_shoot = GetTickCount64();
}
bool Shoot::attack(double time, yard_node** yard) {
	color = green;
	if ((float)(GetTickCount64() - last_time_of_shoot)/1000 > iceTime) {
		auto* b = new Bullet("@", yellow, attack_power, -4, "zombie", {position.high,(float)position.width});
		yard[position.high][position.width].push_zombie(b);
		last_time_of_shoot = GetTickCount64();
		return true;
	}
	return true;
}

Doubleshoot::Doubleshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
	iceTime = 1.0;
	last_time_of_shoot = GetTickCount64();
}
bool Doubleshoot::attack(double time, yard_node** yard) {
	color = green;
	if ((float)(GetTickCount64() - last_time_of_shoot) / 1000 > iceTime) {
		auto* b = new Bullet("@", yellow, attack_power, -4, "zombie", { position.high,(float)position.width });
		yard[position.high][position.width].push_zombie(b);
		b = new Bullet("@", yellow, attack_power, -4, "zombie", { position.high,(float)position.width });
		yard[position.high][position.width].push_zombie(b);
		last_time_of_shoot = GetTickCount64();
		return true;
	}
	return true;
}

Iceshoot::Iceshoot(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{
	iceTime = 1.0;
	last_time_of_shoot = GetTickCount64();
}
bool Iceshoot::attack(double time, yard_node** yard) {
	color = green;
	if ((float)(GetTickCount64() - last_time_of_shoot) / 1000 > iceTime) {
		Bullet* b = new Bullet("@", lightblue, attack_power, -4, "zombie", { position.high,(float)position.width });
		yard[position.high][position.width].push_zombie(b);
		last_time_of_shoot = GetTickCount64();
		return true;
	}
	return true;
}



Normal::Normal(zombie_info& k):zombie(k.name,k.health,k.color,k.attack_power,k.speed) {

}
bool Normal::attack(double time,yard_node** yard)
{
	color = grey;
	if (yard[position.high][(int)position.width].p != NULL)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
			return false;
		}
		else{
			return false;
		}
	else
	{
		if (move(time / 1000 * speed,yard))
		{
			return true;
		}
		else
			return false;
	}
}

bool Nut::attack(double time, yard_node** yard)
{
	color = green;
	return true;
}
Nut::Nut(const plant_info& src, locate<int, int> p):plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{

}

bool Highnut::attack(double time, yard_node** yard)
{
	color = green;
	return true;
}

Highnut::Highnut(const plant_info& src, locate<int, int> p) :plant(src.name, src.health, src.color, src.attack_power, src.price, p)
{

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

}
bool Conehead::attack(double time, yard_node** yard)
{
	if (health <= 100)
	{
		name = (char*)"普通僵尸";
	}
	color = grey;
	if (yard[position.high][(int)position.width].p != NULL)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
			return false;
		}
		else {
			return false;
		}
	else
	{
		if (move(time / 1000 * speed, yard))
		{
			return true;
		}
		else
			return false;
	}
}
Reading::Reading(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{

}
bool Reading::attack(double time, yard_node** yard)
{
	if (health <= 100)
	{
		color = purple;
		speed = 0.45;
	}
	else
		color = grey;
	if (yard[position.high][(int)position.width].p != NULL)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
			return false;
		}
		else {
			return false;
		}
	else
	{
		if (move(time / 1000 * speed, yard))
		{
			return true;
		}
		else
			return false;
	}
}
Pole::Pole(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
	skill = true;
}
bool Pole::attack(double time, yard_node** yard)
{
	if (skill == true)
	{
		color = purple;
		if (yard[position.high][(int)position.width].p != NULL)
		{
			yard[position.high][(int)position.width].pop_zombie(this);
			position.width -= 1;
			yard[position.high][(int)position.width].push_zombie(this);
			if (position.width <= 0)
				return true;
			skill = false;
		}
	}
	else
		color = grey;
	if (yard[position.high][(int)position.width].p != NULL)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
			return false;
		}
		else {
			return false;
		}
	else
	{
		if (move(time / 1000 * speed, yard))
		{
			return true;
		}
		else
			return false;
	}
	return false;
}

Clown::Clown(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{

}
bool Clown::attack(double time, yard_node** yard) {
	if (yard[position.high][(int)position.width].p != NULL && skill==true)
	{
		skill = false;
		if (GetTickCount64() % 2 == 0)
		{
			for (int i = position.high - 1;i<=position.high+1;i++)
				for (int j = position.width - 1; j < position.width + 1; j++)
				{
					if (i >= 0 && i < screen::size_info.screen_high && j >= 0 && j < screen::size_info.screen_width)
						yard[i][j].kill_plant();
				}
			return false;
		}
	}
	color = grey;
	if (yard[position.high][(int)position.width].p != NULL)
		if (!yard[position.high][(int)position.width].p->deHealth((float)(time / 1000) * attack_power))
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
			return false;
		}
		else {
			return false;
		}
	else
	{
		if (move(time / 1000 * speed, yard))
		{
			return true;
		}
		else
			return false;
	}
	return false;
	
}

Throwstone::Throwstone(zombie_info& k) :zombie(k.name, k.health, k.color, k.attack_power, k.speed)
{
	shootNum = 1;
	shoot_freq = 1.5;
	store_power = 70;
	last_shoot = GetTickCount64();
}
bool Throwstone::attack(double time, yard_node** yard)
{
	if (skill == true && shootNum == 0)
	{
		skill = false;
		speed = speed / 2;
		return false;
	}
	else if (skill == true && shootNum > 0)
	{
		color = purple;
		if (GetTickCount64() - last_shoot > shoot_freq)
		{
			for (int i = position.width; i >= 0; i--)
			{
				if (yard[position.high][i].p != NULL)
				{
					Bullet* b = new Bullet{ "@",purple,(int)store_power,4,"plant",position };
					yard[position.high][(int)position.width].push_zombie(b);
					shootNum--;
					last_shoot = GetTickCount64();
					break;
				}
			}
		}
		return false;
	}
	else if(skill==false)
	{
		color = grey;
		if (yard[position.high][(int)position.width].p != NULL)
		{
			delete yard[position.high][(int)position.width].p;
			yard[position.high][(int)position.width].p = NULL;
		}
		if (move(time / 1000 * speed, yard))
			return true;
		else
			return false;
	}
	return false;
}

Bullet::Bullet(const char* _name, obj_color _color, int _attack_power, float _speed,const char* _aim,locate<int,float> p):
	zombie(_name,1000,_color,_attack_power,_speed),aim(_aim)
{
	position.high = p.high;
	position.width = p.width;
}
bool Bullet::attack(double time, yard_node** yard)
{
	if (0)
	{
	}
	else
	{
		if (aim == "plant")
		{
			if (position.width>=0 && position.width<8 && yard[position.high][(int)position.width].p != NULL)
			{
				
				if (!yard[position.high][(int)position.width].p->deHealth(attack_power))
				{
					delete yard[position.high][(int)position.width].p;
					yard[position.high][(int)position.width].p = NULL;
				}
				return true;
			}
		}
		else if(aim=="zombie")
		{
			yard[position.high][(int)position.width].find_first(0);
			if (yard[position.high][(int)position.width].first != -1&&yard[position.high][(int)position.width].z[yard[position.high][(int)position.width].first]!=NULL)
			{
				if (!yard[position.high][(int)position.width].z[yard[position.high][(int)position.width].first]->deHealth(attack_power))
				{
					delete yard[position.high][(int)position.width].z[yard[position.high][(int)position.width].first];
					yard[position.high][(int)position.width].z[yard[position.high][(int)position.width].first] = NULL;
					yard[position.high][(int)position.width].find_first(0);
					return true;
				}
				/**
				 *these programs have problems,so don't use them temporily;
				 *if (this->color == lightblue)
				 *	yard[position.high][(int)position.width].icefirstzombie();
				 */
				return true;
			}
		}
	}
	if (move(time / 1000 * speed, yard))
	{
		return true;
	}
}

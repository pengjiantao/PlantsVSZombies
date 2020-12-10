#include"player.h"
#include"screen.h"
#include"toolfunc.h"
player::player(string _name,int _money):name(_name),money(_money){
	log("a player created");
}
player::player() : name("player"), money(100) {
}
player::~player() = default;
bool player::deMoney(int n)
{
	log("money decreased");
	if (money >= n)
	{
		money -= n;
		screen::flash_money(money);
		return true;
	}
	else
	{
		errlog("deMoney:lack of money");
		screen::putMessage("阳光不够");
		return false;
	}
}
bool player::inMoney(int n)
{
	log("money increased");
	money += n;
	screen::flash_money(money);
	return true;
}
int player::getMoney() const
{
	return money;
}
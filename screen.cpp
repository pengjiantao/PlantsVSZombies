#include"screen.h"
#include<iostream>
#include<stdio.h>
#include"swfunix.h"
#include<string>
#include"plant.h"
#include"game.h"
#include"toolfunc.h"
#include"PlantVSZombie.h"
using namespace std;

QSize screen::zombie_base_={2110,126};
QSize screen::plant_base_={2550,83};
QSize screen::yard_size_={830,100};

screen::size screen::size_info = { 90,5};

void screen::flash_money(int money)
{
    Q_UNUSED(money);
}
screen::screen(int env_w,int env_h)
{
	log("init a new screen config");
	size_info.screen_high = env_h;
	size_info.screen_width = env_w;
}
screen::~screen()
{

}

void screen::setSize(const QSize& n)
{
    size_info.screen_high=n.height();
    size_info.screen_width=n.width();
}

void screen::setZombieBase(const QSize& n)
{
    zombie_base_=n;
}

void screen::setPlantBase(const QSize& n)
{
    plant_base_=n;
}

void screen::setYardSize(const QSize &n)
{
    yard_size_=n;
}

QSize screen::Size()
{
    return {size_info.screen_width,size_info.screen_high};
}

QSize screen::ZombieBase()
{
    return zombie_base_;
}

QSize screen::PlantBase()
{
    return plant_base_;
}

QSize screen::YardSize()
{
    return yard_size_;
}

void screen::init_game_screen()
{
	log("init game screen");
    printf("VERSION :%s\n", version);
}


void screen::putMessage(string s)
{
    cout<<"Message: "<<s<<endl;
}

void screen::putResult(int n)
{
	if (n > 0)
	{
		printf("******************************\n");
		printf("*      YOU WIN!              *\n");
		printf("******************************\n");
	}
	else
	{
		printf("******************************\n");
		printf("*      YOU LOSE              *\n");
		printf("******************************\n");
	}
	putchar('\n');
}



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

QSize screen::zombie_base_={211,126};
QSize screen::plant_base_={171,126};
QSize screen::yard_size_={85,100};

screen::size screen::size_info = { 8,5,10,5 };/*screen_width,screen_high,space_w,space_h*/

obj_color screen::env_color = blue;

void screen::flash_money(int money)
{
    cout<<"flash sunney:new sunney is "<<money<<endl;
}
screen::screen(int env_w,int env_h,int node_w,int node_h)
{
	log("init a new screen config");
	size_info.screen_high = env_h;
	size_info.screen_width = env_w;
	size_info.node_high = node_h;
	size_info.node_width = node_w;
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
    return {size_info.node_width,size_info.screen_high};
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
    cout<<"Message"<<s<<endl;
}

void screen::putResult(int n,int grade)
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
	printf("Your grade is: %d\n", grade);
}

void screen::flash_grade(int grade)
{
    cout<<"Your grade: "<<grade<<endl;
}


coord screen::get_position(const locate<int, int>& p)
{
    coord res = { 0,0 };
	res.Y = 2 + (p.high) * (size_info.node_high + 1);
	res.X = 1 + (p.width) * (size_info.node_width + 1);
	return res;
}

coord screen::get_position(const locate<int, float>& p) {
	locate<int, int> tmp = { p.high,(int)p.width };
	return get_position(tmp);
}

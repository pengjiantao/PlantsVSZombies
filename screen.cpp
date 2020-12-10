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

void screen::init_game_screen()
{
	log("init game screen");
    printf("VERSION :%s\n", version);
}

void screen::flash_menu(menu_entry* menu_list,int n,int c)
{
	init_menu(menu_list, n, c);
}

void screen::init_menu(menu_entry* menu_list,int n=0,int c=0)
{
	printf("menu:");
	short base_x = 5;
    short base_y = 1;
    COORD curPos = { base_x,base_y };
    WORD color = 0;
    if (menu_list[n].name == "NULL")
        return;
    for (int i = n; i != -1; i = menu_list[i].next)
    {
        if (i == c)
        {
            color = red * 16 + blue;
        }
        else
            color = blue;
        printf("%-20s", menu_list[i].name.c_str());
        if(i==c)
            cout<<"<<--"<<endl;
        else
            cout<<endl;
        curPos.Y++;
    }
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
	Sleep(2000);
}

void screen::flash_grade(int grade)
{
    cout<<"Your grade: "<<grade<<endl;
}


COORD screen::get_position(const locate<int, int>& p)
{
	COORD res = { 0,0 };
	res.Y = 2 + (p.high) * (size_info.node_high + 1);
	res.X = 1 + (p.width) * (size_info.node_width + 1);
	return res;
}

COORD screen::get_position(const locate<int, float>& p) {
	locate<int, int> tmp = { p.high,(int)p.width };
	return get_position(tmp);
}

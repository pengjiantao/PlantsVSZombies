#pragma once
#include<iostream>
#include<stdio.h>
#include"swfunix.h"
#include"plant.h"
#include"game.h"
using namespace std;




class screen {
public:

    static coord get_position(const locate<int, float>& p);
    static coord get_position(const locate<int, int>& p);


	/*init of screen of game,print the edge of screen*/
    static void init_game_screen();

	/*init the menu*/
	static void init_menu(menu_entry* menu_list,int n,int c);

	/*falsh the menu*/
	static void flash_menu(menu_entry* menu_list,int n,int c);

	/*print the result of one game.*/
	static void putResult(int n,int grade);

	/*print the message of the game*/
	static void putMessage(string s);

	/*flash the change of money on the screen.*/
	static void flash_money(int money);

	static void flash_grade(int grade);

	screen(int a,int b,int c,int d);
	~screen();
public:
	static struct size{
		int screen_width;
		int screen_high;
		int node_width;
		int node_high;
	}size_info;
	static obj_color env_color;
};

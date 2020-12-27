#pragma once
#include<iostream>
#include<stdio.h>
#include"swfunix.h"
#include"plant.h"
#include"game.h"
using namespace std;




class screen {
public:
    static void init_game_screen();

    static void putResult(int n);

	static void putMessage(string s);

	static void flash_money(int money);

    screen(int a,int b);
	~screen();
public:
	static struct size{
		int screen_width;
		int screen_high;
	}size_info;
private:
    static QSize zombie_base_;
    static QSize plant_base_;
    static QSize yard_size_;
public:
    static void setSize(const QSize& n);
    static void setZombieBase(const QSize& n);
    static void setPlantBase(const QSize& n);
    static void setYardSize(const QSize& n);
    static QSize Size();
    static QSize ZombieBase();
    static QSize PlantBase();
    static QSize YardSize();
};

#pragma once
#include"PlantVSZombie.h"
#include"player.h"
#include"plant.h"
#include<ctime>
#include"unistd.h"
#include"swfunix.h"
#include<QObject>
#include"window.h"
#include<QTimer>
#include<QTime>
#include<QGraphicsScene>
/*单项菜单结构：名字，子菜单（父菜单），下一个兄弟节点，包含具体操作的函数指针*/
struct menu_entry {
	string name="";
	int child=-1;
	int next=-1;
};

/*yard node class ,which contain all information in one node of the yard.*/


enum control {
	toup,todown,toleft,toright,toback,toenter,toremove,undef
};

enum pointer_location {
	store,onyard,menu
};

enum mode {
	running_mode,menu_mode
};

enum state {
    on_pause,running,non
};

class game:public QObject
{
    Q_OBJECT
public:
	/*game start*/
	bool game_start();

	/*read config file ,to create plant_list,zombie_list ,menu_list and so on*/
	void game_init();

	/*program break log*/
	void break_log();


	void configToDisk();

    /*game mian scene*/
    window* main_screen;

	game();
	~game();
private:
	/*create one player*/
	player user;

	/*计时器*/
	struct {
		ULONGLONG time_of_last_frame;
		ULONGLONG time_of_last_zombie;
		ULONGLONG time_of_last_money;
	}count_clock;

	struct {
		ULONGLONG s_1;
		ULONGLONG s_2;
		ULONGLONG s_3;
	}clock_pause;

	/*if ALL_zombie have been created,flags vary*/
	struct {
		bool ALL_zombie;//所有僵尸是否生成完毕
		int lasted_zombie;//剩余僵尸
		double zombie_time;//生成间隔时间
	}zombie_count;


	void game_pause();

	/*exit game when encount an deadly error,
	it will create an error log before exit game,
	don't use this func to exit game as normal*/
	void game_exit();

	/*return true if everything is fun,else return false,
	this func will create the plant when purchase a plant.*/
	bool purchase_plant();


	bool game_continue();

	/*when all conditions satisfied,create a plant and flash screen*/
	bool create_plant();


	/*handle the control from keyboard*/
	bool control_keyboard();

	control getcommand();

	bool doMenufunc();

	/*以下皆为控制指针位置和mode切换函数*/

	void storetoyard();

	void yardtostore();

	void storetomenu();

	void menutostore();

	void runtomenu();

	void menutorun();

	void pointerMove(control c);

	bool remove_plant();
private:
	/*游戏在运行中，或在菜单中*/
	mode game_mode;
	/*游戏：暂停，运行，未开始*/
	state game_state;
	/*屏幕操纵指针位置：在庭院上，在商店里，在菜单上*/
	pointer_location click_location;

	/*是否退出游戏程序的标志位*/
	bool exit_flag;

	/*一把游戏是否结束的标志位*/
	bool game_finished;

	/*当前目录页的根节点*/
	int menu_root = 0;

	/*指向商店的指针位置*/
	int store_pointer;
	/*指向菜单的指针位置*/
	int menu_pointer;
	/*指向庭院的指针位置*/
	locate<int, int> yard_pointer;

	/*this is the array of yard*/
	yard_node** yard;

	/*this array store all information of plant*/
	plant_info* plant_list;

	/*this array stores all info of zombies*/
	zombie_info* zombie_list;

	/*menu entry_list*/
	menu_entry* menu_list;

	/*生成阳光的周期*/
	float sunny_cycle = 2;

	/*阳光粒度*/
	int sunny_granularity = 50;

	/*生成僵尸的周期*/
	float zombie_cycle = 3;

	/*是否在选择移除植物*/
	bool chooseToRemove = false;

	int grade = 0;

    QTimer *sun_timer;
    QTimer *zombie_timer;
    QGraphicsScene* scene;
private:
	void menufunc_new();
	void menufunc_continue();
	void menufunc_pause();
	void menufunc_exit();
	void menufunc_changeColor();
	void menufunc_zombieNum();
	void menufunc_changeSize();
private slots:
    /*create sun*/
    void generate_money();
    /*try to create_plant*/
    bool create_zombie();
};

#include"PlantVSZombie.h"
#include"game.h"
#include"screen.h"
#include<iostream>
#include"toolfunc.h"
#include"swfunix.h"
using namespace std;

const char* version = "2.01";

//用于描述单个空格的宽度：space_w=(5);
int space_w = 10;
//用于描述单个空格的高度：space_h=(5);
int space_h = 5;
//用于描述游戏庭院宽度：env_w=(8);
int env_w = 8;
//用于描述游戏庭院行数：env_h=(8);
int env_h = 5;


//游戏配置文件主目录
#if _WIN32
string config_path = "C://.plantvszombie/";
#elif __linux__
string config_path = "/home/fiyqkrc/plantvszombie/";
#else
#error this program can not be complied in a machine which neithor windows nor linux
#endif

//最高得分记录
int MAX_GRADE = 0;

log_entry* log_list = NULL;

int main()
{
	if (!init())
	{
		cout << "Error: game init failed!" << endl;
		cout << "你可以检查config files是否在指定文件夹下：(defaults path: ~/.plantvszombie/)" << endl;
		cout << "program will exit soon!" << endl;
		Sleep(1000);
		return 0;
	}
	/*start a screen acording to config file*/
	screen display(env_w,env_h,space_w,space_h);
	game* g=new game;
	g->game_init();
	while (g->game_start())
	{
		log("main:start a new game!");
		delete g;
		g = new game;
		g->game_init();
	}
	delete g;
	log("main:exit game!");
	end();
	return 0;
}

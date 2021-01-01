#include"PlantVSZombie.h"
#include"game.h"
#include"screen.h"
#include<iostream>
#include"toolfunc.h"
#include"swfunix.h"
using namespace std;

const char* version = "3.20";


//游戏配置文件主目录
#if _WIN32
string config_path = "./image/files/";
#elif __linux__
string config_path = "./image/files/";
#else
#error this program can not be complied in a machine which is neithor windows nor linux
#endif




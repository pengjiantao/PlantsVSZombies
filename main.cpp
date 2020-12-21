#include <QApplication>

#include"PlantVSZombie.h"
#include"game.h"
#include"screen.h"
#include<iostream>
#include"toolfunc.h"
#include"swfunix.h"
#include"window.h"
#include"settings.h"

int main(int argc,char *argv[])
{
    QApplication a(argc, argv);
    if (!init())
    {
        cout << "Error: game init failed!" << endl;
        cout << "你可以检查config files是否在指定文件夹下：(defaults path: ~/.plantvszombie/)" << endl;
        cout << "program will exit soon!" << endl;
        return 0;
    }
    /*start a screen acording to config file*/
    screen display(env_w,env_h,space_w,space_h);
    game* g=new game;
    g->game_init();
    g->game_start();
    return a.exec();
}

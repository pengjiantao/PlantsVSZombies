#include "mainwindow.h"

#include <QApplication>

#include"PlantVSZombie.h"
#include"game.h"
#include"screen.h"
#include<iostream>
#include"toolfunc.h"
#include"swfunix.h"


int main(int argc,char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.exec();

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

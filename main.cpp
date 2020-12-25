#include <QApplication>

#include"PlantVSZombie.h"
#include"game.h"
#include"screen.h"
#include<iostream>
#include"toolfunc.h"
#include"swfunix.h"
#include"window.h"
#include"settings.h"
#include"settings.h"

int main(int argc,char *argv[])
{
    QApplication a(argc, argv);
    settings* menu=new settings();
    menu->show();
    return a.exec();
}

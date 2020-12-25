#include "settings.h"
#include "ui_settings.h"
#include"screen.h"
#include"game.h"
#include"toolfunc.h"
#include"PlantVSZombie.h"
settings::settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settings)
{
    current_mode_=0;
    ui->setupUi(this);
    this->setWindowTitle("PlantsVSZombies");
    this->setFixedSize(800,600);
    this->setStyleSheet("background-color: lightgrey");
    this->ui->mode1->setFixedSize(750,250);
    this->ui->mode2->setFixedSize(750,250);
    this->ui->mode1->setStyleSheet("background-color: grey");
    this->ui->mode2->setStyleSheet("background-color: grey");
    connect(this->ui->mode1,SIGNAL(clicked()),this,SLOT(mode1BeSelected()));
    connect(this->ui->mode1,SIGNAL(destroyed()),this,SLOT(mode1BeSelected()));
    connect(this->ui->mode2,SIGNAL(clicked()),this,SLOT(mode2BeSelected()));
    connect(this->ui->mode2,SIGNAL(destroyed()),this,SLOT(mode2BeSelected()));
}

settings::~settings()
{
    delete ui;
}

void settings::gameEnd(game *s)
{
    if(s->Result()==false)
    {
        s->disconnect();
        delete s;
        mode1BeSelected();
    }
}

void settings::mode1BeSelected()
{
    current_mode_=1;
    class::screen display(9,5,space_w,space_h);
    init();
    game* g=new game();
    connect(g,SIGNAL(die(game*)),this,SLOT(gameEnd(game*)));
    g->game_init();
    g->game_start();
}

void settings::mode2BeSelected()
{

}

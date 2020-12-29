#include "settings.h"
#include "ui_settings.h"
#include"screen.h"
#include"game.h"
#include"toolfunc.h"
#include"PlantVSZombie.h"
#include"Info.h"
#include"InfoRead.h"
#include<vector>
#include<QtMultimedia/QSound>
settings::settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::settings)
{
    current_mode_=0;
    pass_num_=0;
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/image/Icon-144.png"));
    this->setWindowTitle("PlantsVSZombies");
    this->setFixedSize(800,600);
    this->ui->mode1->setFixedSize(750,250);
    this->ui->mode2->setFixedSize(750,250);
    connect(this->ui->mode1,SIGNAL(clicked()),this,SLOT(mode1BeSelected()));
    connect(this->ui->mode2,SIGNAL(clicked()),this,SLOT(mode2BeSelected()));
}

settings::~settings(){
    if(back_music_!=nullptr)
    {
        delete back_music_;
    }
    delete ui;
}

void settings::initScreen(int n)
{
    string path;
    if(n==0)
    {
        path=config_path+"forever.info";
    }
    else
    {
        QString sn=QString::number(n);
        path=config_path+"pass"+sn.toStdString()+".info";
    }
    InfoRead in(path,"r");
    vector<Info> infos=in.getInfos();
    in.closeFile();
    for(auto i:infos)
    {
        if(i.Name()=="yard_size_")
        {
            screen::setYardSize({stoi(i.ValueList()[0]),stoi(i.ValueList()[1])});
        }
        else if(i.Name()=="size_info_")
        {
            screen::setSize({stoi(i.ValueList()[0]),stoi(i.ValueList()[1])});
        }
        else if(i.Name()=="plant_base_")
        {
            screen::setPlantBase({stoi(i.ValueList()[0]),stoi(i.ValueList()[1])});
        }
        else if(i.Name()=="zombie_base_")
        {
            screen::setZombieBase({stoi(i.ValueList()[0]),stoi(i.ValueList()[1])});
        }
    }
}

void settings::initProgram()
{
    string path;
    path=config_path+"universal.info";
    InfoRead in(path,"r");
    vector<Info> infos=in.getInfos();
    in.closeFile();
    for(auto i:infos)
    {
        if(i.Name()=="pass_num_")
        {
            pass_num_=stoi( i.Value());
        }
        else if(i.Name()=="background_music_")
        {
            background_music_=stoi(i.Value());
        }
        else if(i.Name()=="start_movie_")
        {
            start_movie_=stoi(i.Value());
        }
    }
    if(background_music_)
        playBackMusic();
}

string settings::getConfigPath()
{
    string path;
    if(current_mode_==1)
    {
        path=config_path+"forever.info";
    }
    else
    {
        QString sn=QString::number(current_num_);
        path=config_path+"pass"+sn.toStdString()+".info";
    }
    return path;
}

void settings::playStartMovie()
{

}

void settings::playBackMusic()
{
    if(back_music_!=nullptr)
        delete back_music_;
    back_music_=new QSound(":/image/audio/Grazy Dave.wav");
    back_music_->play();
}


void settings::gameEnd()
{
    bool result=current_game_->Result();
    current_game_->disconnect();
    delete current_game_;
    current_game_=nullptr;
    if(result==true&&current_mode_==2)
    {
        current_num_++;
        if(current_mode_<=pass_num_)
            mode2BeSelected();
    }
    this->show();
}

void settings::mode1BeSelected()
{
    if(current_game_)
        gameEnd();
    current_mode_=1;
    initScreen(0);
    game* g=new game(getConfigPath());
    current_game_=g;
    connect(g,SIGNAL(die(game*)),this,SLOT(gameEnd()));
    g->game_init();
    g->game_start();
    this->hide();
}

void settings::mode2BeSelected()
{
    if(current_game_)
        gameEnd();
    current_mode_=2;
    initScreen(current_num_);
    game* g=new game(getConfigPath());
    current_game_=g;
    connect(g,SIGNAL(die(game*)),this,SLOT(gameEnd()));
    g->game_init();
    g->game_start();
    this->hide();
}

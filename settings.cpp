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
    back_music_=new QSound(":/image/audio/Grazy Dave.wav");
    back_music_listener_=new QTimer(this);
    back_music_listener_->setInterval(5000);
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
    path=config_path+"history.info";
    in.setFile(path,"r");
    if(!in.empty())
    {
        Info info=in.getInfo();
        for(int i=0;i<=pass_num_;i++)
        {
            max_grade_.emplace_back(stoi(info[i]));
        }
    }
    else
    {
        for(int i=0;i<=pass_num_;i++)
        {
            max_grade_.emplace_back(0);
        }
        in.closeFile();
    }
    if(background_music_)
    {
        playBackMusic();
        back_music_listener_->start();
        connect(back_music_listener_,SIGNAL(timeout()),SLOT(back_music_listener_slot_()));
    }
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
    back_music_->play();
}

void settings::writeConfig()
{
    InfoRead out(config_path+"history.info","w");
    Info outinfo;
    outinfo.setName("max_grade_");
    for(auto i:max_grade_)
    {
        outinfo.addValue(QString::asprintf("%d",i).toStdString());
    }
    out.writeInfo(outinfo);
    out.closeFile();
}

void settings::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    writeConfig();
}


void settings::gameEnd(game* g,int grade)
{
    Q_UNUSED(g);
    if(max_grade_[current_num_]<grade)
        max_grade_[current_num_]=grade;
    bool result=current_game_->Result();
    current_game_->disconnect();
    delete current_game_;
    current_game_=nullptr;
    writeConfig();
    if(result==true&&current_mode_==2)
    {
        current_num_++;
        if(current_mode_<=pass_num_)
            mode2BeSelected();
    }
    else
        this->show();
}

void settings::mode1BeSelected()
{
    if(current_game_)
        gameEnd(current_game_,0);
    current_mode_=1;
    initScreen(0);
    current_num_=0;
    game* g=new game(getConfigPath());
    g->setMaxGrade(max_grade_[current_num_]);
    current_game_=g;
    connect(g,SIGNAL(die(game*,int)),this,SLOT(gameEnd(game*,int)));
    g->game_init();
    g->game_start();
    this->hide();
}

void settings::mode2BeSelected()
{
    if(current_num_==0)
        current_num_=1;
    if(current_game_)
        gameEnd(current_game_,0);
    current_mode_=2;
    initScreen(current_num_);
    game* g=new game(getConfigPath());
    g->setMaxGrade(max_grade_[current_num_]);
    current_game_=g;
    connect(g,SIGNAL(die(game*,int)),this,SLOT(gameEnd(game*,int)));
    g->game_init();
    g->game_start();
    this->hide();
}

void settings::back_music_listener_slot_()
{
    if(back_music_->isFinished())
    {
        playBackMusic();
    }
}

#include"game.h"
#include"screen.h"
#include"swfunix.h"
#include"plant.h"
#include"toolfunc.h"
#include<fstream>
#include"window.h"
#include"ui_window.h"
#include<QTime>
#include<QTimer>
#include<QRandomGenerator>
#include"sun.h"
using namespace std;

template struct locate<int, int>;
template struct locate<int,float>;

yard_node*** game::game_yard=nullptr;
game::game():QObject(),user()
{
	log("a game object be created");


	sunny_cycle = 2;
	zombie_cycle = 3;
	sunny_granularity = 50;

	store_pointer = 0;
	yard_pointer.width = 0;
	yard_pointer.high = 0;

    yard = new yard_node**[screen::size_info.screen_high];
	for (int i = 0; i < screen::size_info.screen_high; i++)
        yard[i] = new yard_node*[screen::size_info.screen_width];

	for (int i = 0; i < screen::size_info.screen_high; i++)
		for (int j = 0; j < screen::size_info.screen_width; j++)
        {
            yard[i][j] =new yard_node();
            connect(yard[i][j],SIGNAL(plantDie(plant*)),this,SLOT(dealPlantDead(plant*)));
            connect(yard[i][j],SIGNAL(zombieDie(zombie*)),this,SLOT(dealZombieDead(zombie*)));
        }

    game_yard=yard;

	plant_list = new plant_info[10]{
        {"shooter",100,20,100},
        {"sunflower",50,0,50},
        {"repeater",100,20,200},
        {"iceshoot",100,30,300},
        {"nut",500,0,100},
        {"highnut",1000,0,200},
        {"wogua",1000,500,100},
        {"cherrybomb",1000,500,250},
        {"garlic",100,0,100},
        {"nanguatou",400,0,100}
    };
	zombie_list = new zombie_info[10]{
        {300,50,0.3,"conehead",1},
        {150,50,0.3,"reading",1},
        {100,40,0.5,"pole",1},
        {100,30,0.3,"xiaochou",1},
        {400,70,0.2,"throwstone",1},
        {100,30,0.3,"normal",1}
	};


	chooseToRemove = false;
	zombie_info::ALL_ZOMBIE = false;
    numZombieOnYard=0;

    main_screen=new window;
    sun_timer=new QTimer(this);
    sun_timer->setInterval(sunny_cycle*1000);
    sun_timer->stop();
    zombie_timer=new QTimer(this);
    zombie_timer->setInterval(zombie_cycle*1000);
    zombie_timer->stop();
    plant_ice_action_=new QTimer(this);
    plant_ice_action_->setInterval(200);
    plant_ice_action_->stop();
    zombie_check_=new QTimer(this);
    zombie_check_->setInterval(2000);
    zombie_check_->stop();
    exit_clock_=new QTimer(this);
    scene=new GameScene();
    bk_yard_size={screen::Size().width(),screen::Size().height()};

    connect(this->zombie_timer,SIGNAL(timeout()),this,SLOT(create_zombie()));
    connect(this->sun_timer,SIGNAL(timeout()),this,SLOT(generate_money()));
    connect(this->plant_ice_action_,SIGNAL(timeout()),this,SLOT(plant_ice()));
    connect(this,SIGNAL(onePlantPrepared(int)),this,SLOT(plantPrepared(int)));
    connect(this,SIGNAL(plantSelectedChanged(int)),this,SLOT(changePlantSelected(int)));
    connect(this->main_screen->ui->shovel,SIGNAL(clicked()),this,SLOT(shovelClicked()));
    connect(this->scene,SIGNAL(beClicked(QPoint)),this,SLOT(dealClickedRequest(QPoint)));
    connect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_pause()));
    connect(this->zombie_check_,SIGNAL(timeout()),this,SLOT(zombie_check_timeout()));
    connect(this,SIGNAL(allZombieBeKilled()),this,SLOT(plantSuccess()));
}

game::~game()
{
	configToDisk();
	log("a game object be killed");
	for (int i =0;i< screen::size_info.screen_high; i++)
		delete[] yard[i];
    sun_timer->disconnect();
    zombie_timer->disconnect();
    plant_ice_action_->disconnect();
    exit_clock_->disconnect();
    zombie_check_->disconnect();
    delete exit_clock_;
    delete plant_ice_action_;
    delete sun_timer;
    delete zombie_timer;
    delete zombie_check_;

    delete main_screen;
	delete[] yard;
	delete[] plant_list;
	delete[] zombie_list;

    if(!bgItem)
        delete bgItem;
    scene->disconnect();
    delete scene;
    this->disconnect();
}

void game::game_init()
{

	log("game_init start");


    screen::setSize(bk_yard_size);
    main_screen->show();
    main_screen->ui->main_screen_view->setScene(scene);
    scene->setSceneRect(0,0,1000,800);
    main_screen->ui->main_screen_view->setCacheMode(QGraphicsView::CacheBackground);
    main_screen->ui->main_screen_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    main_screen->ui->main_screen_view->scale(1,1.1);
    bgItem = new QGraphicsPixmapItem(QPixmap(":/image/environment/nonstop/Background.jpg"));
    scene->addItem(bgItem);
    main_screen->ui->main_screen_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //main_screen->ui->main_screen_view->scale((qreal)main_screen->ui->main_screen_view->size().height()/(qreal)scene->sceneRect().height(),
                                             //(qreal)main_screen->ui->main_screen_view->size().width()/(qreal)scene->sceneRect().width());
    log("game_init finished");
}


bool game::game_start()
{
	log("a game start now!");
    screen::init_game_screen();
	screen::putMessage("游戏开始！");
	click_location = store;
	store_pointer = 0;
	yard_pointer.width = 0;
	yard_pointer.high = 0;
	chooseToRemove = false;
    sun_timer->start();
    zombie_timer->start();
    plant_ice_action_->start();
    zombie_check_->start();


    return true;
}

void game::game_pause()
{
    zombie_timer->stop();
    sun_timer->stop();
    plant_ice_action_->stop();
    emit(pause());
    disconnect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_pause()));
    this->main_screen->ui->pause->setStyleSheet("background-color: green");
    connect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_continue()));
}

bool game::game_continue()
{
    zombie_timer->start();
    sun_timer->start();
    plant_ice_action_->start();
    emit(gameContinue());
    disconnect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_continue()));
    this->main_screen->ui->pause->setStyleSheet("background-color: red");
    connect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_pause()));
	return true;
}

bool game::purchase_plant() {
    if ((int)store_pointer >= 10 || plant_list[store_pointer].name ==(string) "NULL")
	{
		errlog("purchase_plant:you try to purchase a plant unexisted!");
		errlog("purchase_plant:your purchase has been blocked!");
		return false;
	}
	else
	{
        if (yard[yard_pointer.high][yard_pointer.width]->p != NULL&&store_pointer!=9)
		{
			errlog("try to create an plant on a unblank grace!");
			screen::putMessage("不能在非空的草地上种植物");
			return false;
		}
        else if (store_pointer == 9 && yard[yard_pointer.high][yard_pointer.width]->p == NULL)
		{
			errlog("try to create an pumpkin at a blank grace");
			screen::putMessage("请在植物上种植南瓜头");
			return false;
		}
        else if (plant_list[store_pointer].wait>0)
		{
			errlog("ice_time!");
			screen::putMessage("请注意冷却时间");
			return false;
		}
		else if (user.deMoney(plant_list[store_pointer].price))
		{
            plant_list[store_pointer].wait = plant_list[store_pointer].ice_time;
			create_plant();
            changePlantSelected(store_pointer);
            switch(store_pointer)
            {
                   case 0:this->main_screen->ui->groupBox->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant0,SIGNAL(clicked()),this,SLOT(plant1BeSelected()));break;
                   case 1:this->main_screen->ui->groupBox_2->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant1,SIGNAL(clicked()),this,SLOT(plant2BeSelected()));break;
                   case 2:this->main_screen->ui->groupBox_3->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant2,SIGNAL(clicked()),this,SLOT(plant3BeSelected()));break;
                   case 3:this->main_screen->ui->groupBox_4->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant3,SIGNAL(clicked()),this,SLOT(plant4BeSelected()));break;
                   case 4:this->main_screen->ui->groupBox_5->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant4,SIGNAL(clicked()),this,SLOT(plant5BeSelected()));break;
                   case 5:this->main_screen->ui->groupBox_6->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant5,SIGNAL(clicked()),this,SLOT(plant6BeSelected()));break;
                   case 6:this->main_screen->ui->groupBox_7->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant6,SIGNAL(clicked()),this,SLOT(plant7BeSelected()));break;
                   case 7:this->main_screen->ui->groupBox_8->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant7,SIGNAL(clicked()),this,SLOT(plant8BeSelected()));break;
                   case 8:this->main_screen->ui->groupBox_9->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant8,SIGNAL(clicked()),this,SLOT(plant9BeSelected()));break;
                   case 9:this->main_screen->ui->groupBox_10->setStyleSheet("background-color: red");
                       disconnect(this->main_screen->ui->plant9,SIGNAL(clicked()),this,SLOT(plant10BeSelected()));break;
                   default:break;
            }
			log("you purchase a plant successfully");
			return true;
		}
		else
		{
			errlog("purchase_plant:purchase failed,you may don't have enough money!");
			return false;
		}
	}
}

bool game::create_plant()
{
	plant* np = nullptr;
	switch (store_pointer)
	{
	case shoot:
		np = new Shoot( plant_list[shoot], yard_pointer );
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case sunflower:
		np = new Sunflower(plant_list[sunflower], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case doubleshoot:
		np = new Doubleshoot(plant_list[doubleshoot], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case iceshoot:
		np = new Iceshoot(plant_list[iceshoot], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case nut:
		np = new Nut(plant_list[nut], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case highnut:
		np = new Highnut(plant_list[highnut], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case cherrybomb:
		np = new Cherrybomb(plant_list[cherrybomb], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case wogua:
		np = new Wogua(plant_list[wogua], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case garlic:
		np = new Garlic(plant_list[garlic], yard_pointer);
        yard[yard_pointer.high][yard_pointer.width]->p = np;
		break;
	case pumpkin:
	{
        PumpKin* ph=new PumpKin(yard[yard_pointer.high][yard_pointer.width]->p->body->pos());
        scene->addItem(ph);
        yard[yard_pointer.high][yard_pointer.width]->p->setProtectHead(ph);
		break;
	}
	default:
		errlog("create_plant:try to create an plant unexisted");
		return false;
	}
    if(np!=nullptr){
        scene->addItem(np->body);
        connect(np,SIGNAL(createBullet(plant*)),this,SLOT(createBullet(plant*)));
        connect(this,SIGNAL(pause()),np,SLOT(pauseSlot()));
        connect(this,SIGNAL(gameContinue()),np,SLOT(continueSlot()));
        connect(np,SIGNAL(die(plant*)),this,SLOT(dealPlantDead(plant*)));
        connect(np,SIGNAL(zombieDie(zombie*)),this,SLOT(dealZombieDead(zombie*)));
    }
	return true;
}

bool game::create_zombie()
{
	zombie* nz = nullptr;
    int index = QRandomGenerator::global()->bounded(10);
	for (int i = 0; i < 10; i++)
	{
        if (zombie_list[(i + index)%10].name !=(string) "NULL" && zombie_list[(i + index)%10].number > 0)
		{
			switch ((i + index)%10)
			{
			case normal:
				nz = new Normal(zombie_list[(i + index)%10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			case conehead:
				nz = new Conehead(zombie_list[(i + index) % 10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			case reading:
				nz = new Reading(zombie_list[(i+index) % 10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			case pole:
				nz = new Pole(zombie_list[(i + index) % 10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			case clown:
				nz = new Clown(zombie_list[(i + index) % 10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			case throwstone:
				nz = new Throwstone(zombie_list[(i + index) % 10]);
                yard[nz->get_position().high][(int)nz->get_position().width]->push_zombie(nz);
				break;
			default:
				errlog("try to create an unexisted zombie");
				return false;
			}
            scene->addItem(nz->body);
			zombie_list[(i + index)%10].number--;
            connect(nz,SIGNAL(success()),this,SLOT(zombieSuccess()));
            connect(nz,SIGNAL(plantDie(plant*)),this,SLOT(dealPlantDead(plant*)));
            connect(nz,SIGNAL(walkToAttack()),nz,SLOT(walkToAttackSlot()));
            connect(this,SIGNAL(pause()),nz,SLOT(runToPauseSlot()));
            connect(this,SIGNAL(gameContinue()),nz,SLOT(pauseToRunSlot()));
            connect(nz,SIGNAL(attackToWalk()),nz,SLOT(attackToWalkSlot()));
            connect(nz,SIGNAL(die(zombie*)),this,SLOT(dealZombieDead(zombie*)));
            this->numZombieOnYard++;
			return true;
		}
	}
	zombie_info::ALL_ZOMBIE = true;
    return true;
}

void game::createBullet(plant *s)
{
    Bullet* b;
    if(s->getName()==(string)"shooter"||s->getName()==(string)"repeater")
    {
        b=new Bullet("GreenBullet",30,16,{s->getPosition().high,(float)s->getPosition().width});

    }
    else if(s->getName()==(string)"iceshoot")
    {
        b=new Bullet("BlueBullet",30,16,{s->getPosition().high,(float)s->getPosition().width});
    }
    else if(s->getName()==(string)"sunflower")
    {
        generate_sun_plant(s);
        return;
    }
    else
        return;
    connect(this,SIGNAL(pause()),b,SLOT(runToPauseSlot()));
    connect(this,SIGNAL(gameContinue()),b,SLOT(pauseToRunSlot()));
    connect(b,SIGNAL(die(Bullet*)),this,SLOT(dealBulletDead(Bullet*)));
    connect(b,SIGNAL(zombieDie(zombie*)),this,SLOT(dealZombieDead(zombie*)));
    scene->addItem(b->body);
}

void game::plant_ice()
{
    for(int i=0;i<10;i++)
    {
        if(plant_list[i].wait>0&&plant_list[i].wait<0.2)
            emit(onePlantPrepared(i));
        plant_list[i].wait=max(plant_list[i].wait-0.2,0.0);
    }
}

void game::changePlantSelected(int n)
{
    if(n==store_pointer&&click_location==pointer_location::onyard&&chooseToRemove==false)
    {
        switch(store_pointer)
        {
        case 0:this->main_screen->ui->groupBox->setStyleSheet("background-color: lightgreen");break;
        case 1:this->main_screen->ui->groupBox_2->setStyleSheet("background-color: lightgreen");break;
        case 2:this->main_screen->ui->groupBox_3->setStyleSheet("background-color: lightgreen");break;
        case 3:this->main_screen->ui->groupBox_4->setStyleSheet("background-color: lightgreen");break;
        case 4:this->main_screen->ui->groupBox_5->setStyleSheet("background-color: lightgreen");break;
        case 5:this->main_screen->ui->groupBox_6->setStyleSheet("background-color: lightgreen");break;
        case 6:this->main_screen->ui->groupBox_7->setStyleSheet("background-color: lightgreen");break;
        case 7:this->main_screen->ui->groupBox_8->setStyleSheet("background-color: lightgreen");break;
        case 8:this->main_screen->ui->groupBox_9->setStyleSheet("background-color: lightgreen");break;
        case 9:this->main_screen->ui->groupBox_10->setStyleSheet("background-color: lightgreen");break;
        default:break;
        }
        QCursor s;
        this->main_screen->setCursor(s);
        click_location=pointer_location::store;
        return ;
    }
    if(chooseToRemove==true&&click_location==pointer_location::onyard)
    {
        this->main_screen->ui->shovel->setStyleSheet("border-image: url(:/image/source/Shovel.png)");
        chooseToRemove=false;
        click_location=pointer_location::store;
        QCursor s;
        this->main_screen->setCursor(s);
    }
    switch(store_pointer)
    {
    case 0:this->main_screen->ui->groupBox->setStyleSheet("background-color: lightgreen");break;
    case 1:this->main_screen->ui->groupBox_2->setStyleSheet("background-color: lightgreen");break;
    case 2:this->main_screen->ui->groupBox_3->setStyleSheet("background-color: lightgreen");break;
    case 3:this->main_screen->ui->groupBox_4->setStyleSheet("background-color: lightgreen");break;
    case 4:this->main_screen->ui->groupBox_5->setStyleSheet("background-color: lightgreen");break;
    case 5:this->main_screen->ui->groupBox_6->setStyleSheet("background-color: lightgreen");break;
    case 6:this->main_screen->ui->groupBox_7->setStyleSheet("background-color: lightgreen");break;
    case 7:this->main_screen->ui->groupBox_8->setStyleSheet("background-color: lightgreen");break;
    case 8:this->main_screen->ui->groupBox_9->setStyleSheet("background-color: lightgreen");break;
    case 9:this->main_screen->ui->groupBox_10->setStyleSheet("background-color: lightgreen");break;
    default:break;
    }
    store_pointer=n;
    switch(n)
    {
    case 0:this->main_screen->ui->groupBox->setStyleSheet("background-color: lightblue");break;
    case 1:this->main_screen->ui->groupBox_2->setStyleSheet("background-color: lightblue");break;
    case 2:this->main_screen->ui->groupBox_3->setStyleSheet("background-color: lightblue");break;
    case 3:this->main_screen->ui->groupBox_4->setStyleSheet("background-color: lightblue");break;
    case 4:this->main_screen->ui->groupBox_5->setStyleSheet("background-color: lightblue");break;
    case 5:this->main_screen->ui->groupBox_6->setStyleSheet("background-color: lightblue");break;
    case 6:this->main_screen->ui->groupBox_7->setStyleSheet("background-color: lightblue");break;
    case 7:this->main_screen->ui->groupBox_8->setStyleSheet("background-color: lightblue");break;
    case 8:this->main_screen->ui->groupBox_9->setStyleSheet("background-color: lightblue");break;
    case 9:this->main_screen->ui->groupBox_10->setStyleSheet("background-color: lightblue");break;
    default:break;
    }
    click_location=pointer_location::onyard;
}

void game::plantPrepared(int n)
{
    switch(n)
    {
    case 0:this->main_screen->ui->groupBox->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant0,SIGNAL(clicked()),this,SLOT(plant1BeSelected()));break;
    case 1:this->main_screen->ui->groupBox_2->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant1,SIGNAL(clicked()),this,SLOT(plant2BeSelected()));break;
    case 2:this->main_screen->ui->groupBox_3->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant2,SIGNAL(clicked()),this,SLOT(plant3BeSelected()));break;
    case 3:this->main_screen->ui->groupBox_4->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant3,SIGNAL(clicked()),this,SLOT(plant4BeSelected()));break;
    case 4:this->main_screen->ui->groupBox_5->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant4,SIGNAL(clicked()),this,SLOT(plant5BeSelected()));break;
    case 5:this->main_screen->ui->groupBox_6->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant5,SIGNAL(clicked()),this,SLOT(plant6BeSelected()));break;
    case 6:this->main_screen->ui->groupBox_7->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant6,SIGNAL(clicked()),this,SLOT(plant7BeSelected()));break;
    case 7:this->main_screen->ui->groupBox_8->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant7,SIGNAL(clicked()),this,SLOT(plant8BeSelected()));break;
    case 8:this->main_screen->ui->groupBox_9->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant8,SIGNAL(clicked()),this,SLOT(plant9BeSelected()));break;
    case 9:this->main_screen->ui->groupBox_10->setStyleSheet("background-color: lightgreen");
        connect(this->main_screen->ui->plant9,SIGNAL(clicked()),this,SLOT(plant10BeSelected()));break;
    default:break;
    }
}

void game::shovelClicked()
{
    if(this->chooseToRemove)
    {
        this->main_screen->ui->shovel->setStyleSheet("border-image: url(:/image/source/Shovel.png)");
        chooseToRemove=false;
        click_location=pointer_location::store;
        QCursor s;
        this->main_screen->setCursor(s);
    }
    else
    {
        if(click_location==pointer_location::onyard)
        {
            emit(plantSelectedChanged(store_pointer));
        }
        this->main_screen->ui->shovel->setStyleSheet("background-color: lightblue");
        click_location=pointer_location::onyard;
        chooseToRemove=true;
        QCursor s(QPixmap(":/image/source/Shovel.png"));
        this->main_screen->setCursor(s);

    }
}

void game::zombieSuccess()
{
    this->result=false;
    game_pause();
    screen::putResult(-1);

    role_body* c=new role_body;
    c->setMovie(":/image/source/ZombiesWon.gif");
    c->setTimer(new QTimer());
    c->Timer()->setInterval(1000);
    c->Timer()->start();
    c->setHeight(screen::size_info.screen_high*screen::YardSize().height());
    c->setWidth(screen::size_info.screen_width*screen::YardSize().width());
    c->setPos({(qreal)screen::PlantBase().width()+screen::size_info.screen_width*screen::YardSize().width()/2,
               (qreal)screen::ZombieBase().height()+screen::size_info.screen_high*screen::YardSize().height()/2});
    scene->addItem(c);
    c->show();
    exit_clock_->setInterval(2000);
    exit_clock_->start();
    connect(exit_clock_,SIGNAL(timeout()),this,SLOT(exit_clock_timeout()));
}

void game::plantSuccess()
{
    this->result=true;
    game_pause();
    screen::putResult(1);

    role_body* c=new role_body;
    c->setMovie(":/image/source/trophy.gif");
    c->setTimer(new QTimer());
    c->Timer()->setInterval(1000);
    c->Timer()->start();
    c->setHeight(screen::size_info.screen_high*screen::YardSize().height());
    c->setWidth(screen::size_info.screen_width*screen::YardSize().width());
    c->setPos({(qreal)screen::PlantBase().width()+screen::size_info.screen_width*screen::YardSize().width()/2,
               (qreal)screen::ZombieBase().height()+screen::size_info.screen_high*screen::YardSize().height()/2});
    scene->addItem(c);
    c->show();
    exit_clock_->setInterval(2000);
    exit_clock_->start();
    connect(exit_clock_,SIGNAL(timeout()),this,SLOT(exit_clock_timeout()));
}

void game::plant1BeSelected()
{
    QCursor s(QPixmap(":/image/plant/0/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(0));
}
void game::plant2BeSelected()
{
    QCursor s(QPixmap(":/image/plant/1/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(1));
}
void game::plant3BeSelected()
{
    QCursor s(QPixmap(":/image/plant/2/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(2));
}
void game::plant4BeSelected()
{
    QCursor s(QPixmap(":/image/plant/3/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(3));
}
void game::plant5BeSelected()
{
    QCursor s(QPixmap(":/image/plant/4/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(4));
}
void game::plant6BeSelected()
{
    QCursor s(QPixmap(":/image/plant/5/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(5));
}
void game::plant7BeSelected()
{
    QCursor s(QPixmap(":/image/plant/6/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(6));
}
void game::plant8BeSelected()
{
    QCursor s(QPixmap(":/image/plant/7/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(7));
}
void game::plant9BeSelected()
{
    QCursor s(QPixmap(":/image/plant/8/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(8));
}
void game::plant10BeSelected()
{
    QCursor s(QPixmap(":/image/plant/9/0.gif"));
    this->main_screen->setCursor(s);
    emit(plantSelectedChanged(9));
}

void game::dealPlantDead(plant *s)
{
    yard[s->getPosition().high][s->getPosition().width]->p=nullptr;
    s->disconnect();
    delete s;
}

void game::dealZombieDead(zombie *s)
{
    yard[s->get_position().high][(int)s->get_position().width]->pop_zombie(s);
    numZombieOnYard--;
    s->disconnect();
    this->dieAnimation(s);
    delete s;
}

void game::dealBulletDead(Bullet *s)
{
    s->disconnect();
    delete s;
}

void game::sunBeCollected(sun *s)
{
    user.inMoney(s->Value());
    s->disconnect();
    delete s;
}

void game::dealClickedRequest(QPoint a)
{
    yard_pointer.high=a.x();
    yard_pointer.width=a.y();
    if(click_location==pointer_location::onyard&&chooseToRemove==true)
    {
       remove_plant();
    }
    else if(click_location==pointer_location::onyard&&chooseToRemove==false)
    {
        purchase_plant();
    }
}

void game::dieAnimationEnd(role_body *s)
{
    s->Timer()->disconnect();
    s->disconnect();
    delete s;
}

void game::exit_clock_timeout()
{
    cout<<"game will exit"<<endl;
    emit(die(this));
}

void game::generate_sun_plant(plant *s)
{
    sun* soney=new sun;
    soney->setBotton(s->body->pos().y()+(qreal)screen::YardSize().height()/2);
    soney->setPos({s->body->pos().x()+(qreal)screen::YardSize().width()/2-QRandomGenerator::global()->bounded(screen::YardSize().width()),s->body->pos().y()});
    scene->addItem(soney);
    connect(soney,SIGNAL(beCollected(sun*)),this,SLOT(sunBeCollected(sun*)));
    connect(this,SIGNAL(pause()),soney,SLOT(pauseSlot()));
    connect(this,SIGNAL(gameContinue()),soney,SLOT(continueSlot()));
    main_screen->ui->sun->display(user.getMoney());
}

void game::zombie_check_timeout()
{
    if(zombie_info::ALL_ZOMBIE==true&&numZombieOnYard==0)
    {
        emit(allZombieBeKilled());
        zombie_check_->stop();
    }
}

void game::generate_money()
{
    sun* soney=new sun;
    soney->setPos(screen::PlantBase().width()+QRandomGenerator::global()->bounded(screen::size_info.screen_width*screen::YardSize().width()),screen::PlantBase().height());
    scene->addItem(soney);
    connect(soney,SIGNAL(beCollected(sun*)),this,SLOT(sunBeCollected(sun*)));
    connect(this,SIGNAL(pause()),soney,SLOT(pauseSlot()));
    connect(this,SIGNAL(gameContinue()),soney,SLOT(continueSlot()));
    main_screen->ui->sun->display(user.getMoney());
}



bool yard_node::push_zombie(zombie* zom)
{
    for (int i = 0; i < 10; i++)
	{
        if (z[i] == nullptr)
		{
			z[i] = zom;
			return true;
		}
    }
    errlog("yard_node:have too much zombie");
	return false;
}

bool yard_node::pop_zombie(zombie* zom)
{
	for (int i = 0; i < 10; i++)
	{
		if (z[i] == zom)
		{
            z[i] = nullptr;
			return true;
		}
	}
	errlog("yard_node:pop zombie failed,program should be checked to avoid bug!");
	return false;
}


bool game::remove_plant()
{
    if (yard[yard_pointer.high][yard_pointer.width]->p == NULL)
	{
        screen::putMessage("你这是要移除什么？");
		return false;
	}
	else
	{
        delete yard[yard_pointer.high][yard_pointer.width]->p;
        yard[yard_pointer.high][yard_pointer.width]->p = NULL;
		screen::putMessage("你成功移除了一颗植物");
        shovelClicked();
		return true;
    }
}

void game::dieAnimation(zombie *s)
{
    role_body* body1=nullptr,*body2=nullptr;
    if(s->getHealth()<-100&&s->getHealth()>=-1000)
    {
        if(s->getName()!=static_cast<string>("throwstone"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/5/BoomDie.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            Q_UNUSED(body2);
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
        else
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/4/BoomDie.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            Q_UNUSED(body2);
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
    }
    else if(s->getHealth()>=-100)
    {
        if(s->getName()==static_cast<string>("normal")||s->getName()==static_cast<string>("conehead"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/5/ZombieDie.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
            body2=new role_body();
            body2->setMovie(":/image/zombie/5/ZombieHead.gif");
            body2->setTimer(new QTimer());
            body2->Timer()->setInterval(1500);
            body2->Timer()->start();
            body2->setPos(s->body->pos());
            scene->addItem(body2);
            body2->show();
            connect(body2,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
        else if(s->getName()==static_cast<string>("reading"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/1/Die.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
            body2=new role_body();
            body2->setMovie(":/image/zombie/1/Head.gif");
            body2->setTimer(new QTimer());
            body2->Timer()->setInterval(1500);
            body2->Timer()->start();
            body2->setPos(s->body->pos());
            scene->addItem(body2);
            body2->show();
            connect(body2,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
        else if(s->getName()==static_cast<string>("throwstone"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/4/5.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
        else if(s->getName()==static_cast<string>("xiaochou"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/3/Die.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
            body2=new role_body();
            body2->setMovie(":/image/zombie/5/ZombieHead.gif");
            body2->setTimer(new QTimer());
            body2->Timer()->setInterval(1500);
            body2->Timer()->start();
            body2->setPos(s->body->pos());
            scene->addItem(body2);
            body2->show();
            connect(body2,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
        else if(s->getName()==static_cast<string>("pole"))
        {
            body1=new role_body();
            body1->setMovie(":/image/zombie/2/PoleVaultingZombieDie.gif");
            body1->setTimer(new QTimer());
            body1->Timer()->setInterval(1500);
            body1->Timer()->start();
            body1->setPos(s->body->pos());
            scene->addItem(body1);
            body1->show();
            connect(body1,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
            body2=new role_body();
            body2->setMovie(":/image/zombie/2/PoleVaultingZombieHead.gif");
            body2->setTimer(new QTimer());
            body2->Timer()->setInterval(1500);
            body2->Timer()->start();
            body2->setPos(s->body->pos());
            scene->addItem(body2);
            body2->show();
            connect(body2,SIGNAL(end(role_body*)),this,SLOT(dieAnimationEnd(role_body*)));
        }
    }
    else
        return;
}

void game::configToDisk()
{
	fstream outfile;

	string path = config_path + "env_config.txt";
	outfile.open(path, ios::out);
	outfile << "env_w" << endl << screen::size_info.screen_width << endl;
	outfile << "env_h" << endl << screen::size_info.screen_high << endl;
    outfile.close();
}

bool game::Result()
{
    return result;
}


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

yard_node** game::game_yard=nullptr;
game::game():QObject(),user()
{
	log("a game object be created");
    uint64_t now_time = GetTickCount64();
	count_clock.time_of_last_frame = now_time;
	count_clock.time_of_last_money = now_time;
	count_clock.time_of_last_zombie = now_time;
	
	clock_pause.s_1 = 0;
	clock_pause.s_2 = 0;
	clock_pause.s_3 = 0;

	zombie_count.ALL_zombie = false;
	zombie_count.lasted_zombie = 10;
	zombie_count.zombie_time = 3;

	game_mode = menu_mode;
	game_state = non;
	click_location = menu;
	exit_flag = false;
	game_finished = true;
	menu_root = 0;

	sunny_cycle = 2;
	zombie_cycle = 3;
	sunny_granularity = 50;

	grade = 0;

	store_pointer = 0;
	menu_pointer = 0;
	yard_pointer.width = 0;
	yard_pointer.high = 0;

	yard = new yard_node*[screen::size_info.screen_high];
	for (int i = 0; i < screen::size_info.screen_high; i++)
		yard[i] = new yard_node[screen::size_info.screen_width];

	for (int i = 0; i < screen::size_info.screen_high; i++)
		for (int j = 0; j < screen::size_info.screen_width; j++)
			yard[i][j] = yard_node();

    game_yard=yard;

	plant_list = new plant_info[10]{
        {"shooter",100,20,green,100},
        {"sunflower",50,0,green,50},
        {"repeater",100,20,green,200},
        {"iceshoot",100,30,green,300},
        {"nut",500,0,green,100},
        {"highnut",1000,0,green,200},
        {"wogua",1000,500,green,100},
        {"cherrybomb",1000,500,green,250},
        {"garlic",100,0,green,100},
        {"nanguatou",400,0,green,100}
	};
	zombie_list = new zombie_info[10]{
        {300,50,0.3,"conehead",grey,0},
        {150,50,0.3,"reading",grey,0},
        {100,40,0.5,"pole",grey,0},
        {100,30,0.3,"xiaochou",grey,0},
        {400,70,0.2,"throwstone",grey,1},
        {100,30,0.3,"normal",grey,0}
	};

	menu_list = new menu_entry[50];
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
    exit_clock_=new QTimer(this);
    scene=new GameScene();
    bk_yard_size={9,5};

    connect(this->zombie_timer,SIGNAL(timeout()),this,SLOT(create_zombie()));
    connect(this->sun_timer,SIGNAL(timeout()),this,SLOT(generate_money()));
    connect(this->plant_ice_action_,SIGNAL(timeout()),this,SLOT(plant_ice()));
    connect(this,SIGNAL(onePlantPrepared(int)),this,SLOT(plantPrepared(int)));
    connect(this,SIGNAL(plantSelectedChanged(int)),this,SLOT(changePlantSelected(int)));
    connect(this->main_screen->ui->shovel,SIGNAL(clicked()),this,SLOT(shovelClicked()));
    connect(this->scene,SIGNAL(beClicked(QPoint)),this,SLOT(dealClickedRequest(QPoint)));
    connect(this->main_screen->ui->pause,SIGNAL(clicked()),this,SLOT(game_pause()));
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
    delete exit_clock_;
    delete plant_ice_action_;
    delete sun_timer;
    delete zombie_timer;

    delete main_screen;
	delete[] yard;
	delete[] plant_list;
	delete[] zombie_list;
	delete[] menu_list;
    if(!bgItem)
        delete bgItem;
    scene->disconnect();
    delete scene;
    this->disconnect();
}

void game::game_init()
{

	log("game_init start");
	string path = config_path + "menu.txt";
	fstream infile;
	infile.open(path.c_str(), ios::in);
	if (!infile)
	{
		errlog("game_init:no menu.txt found,this is an important file!");
		game_exit();
		return;
	}
	string command = "";
	for (int i = 0; i < 50; i++) {
		infile >> command;
		if (command != "NULL")
		{
			menu_list[i].name = command;
			infile >> command;
			menu_list[i].next = stoi(command);
			infile >> command;
			menu_list[i].child = stoi(command);
		}
		else
		{
			menu_list[i].name = "NULL";
			menu_list[i].child = -1;
			menu_list[i].next = -1;
		}
	}
	infile.close();

	path = config_path + "info.txt";
	infile.open(path, ios::in);
	command = "";
    while (!infile.eof() && command != "zombie_num_")
		infile >> command;
	for (int i = 0; !infile.eof() && i < 10; i++)
	{
		infile >> zombie_list[i].number;
	}
	infile.close();
	log("game_init finished");

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
}


bool game::game_start()
{
	log("a game start now!");
    screen::init_game_screen();
	screen::putMessage("游戏开始！");
	game_mode = running_mode;
	game_state = running;
	click_location = store;
	exit_flag = false;
	game_finished = false;
	store_pointer = 0;
	yard_pointer.width = 0;
	yard_pointer.high = 0;
	menu_pointer = 0;
	chooseToRemove = false;
	menu_root = 0;
    sun_timer->start();
    zombie_timer->start();
    plant_ice_action_->start();


    return true;
}

void game::game_exit()
{
	log("game encount a deadly error,exit game anyway");
	cout << "your game encounter an error!" << endl;
	cout << "information be collected will be output to an error file.\nyou can find it in config_path " << endl;
	end();
	exit(0);
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
		if (yard[yard_pointer.high][yard_pointer.width].p != NULL&&store_pointer!=9)
		{
			errlog("try to create an plant on a unblank grace!");
			screen::putMessage("不能在非空的草地上种植物");
			return false;
		}
		else if (store_pointer == 9 && yard[yard_pointer.high][yard_pointer.width].p == NULL)
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
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case sunflower:
		np = new Sunflower(plant_list[sunflower], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case doubleshoot:
		np = new Doubleshoot(plant_list[doubleshoot], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case iceshoot:
		np = new Iceshoot(plant_list[iceshoot], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case nut:
		np = new Nut(plant_list[nut], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case highnut:
		np = new Highnut(plant_list[highnut], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case cherrybomb:
		np = new Cherrybomb(plant_list[cherrybomb], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case wogua:
		np = new Wogua(plant_list[wogua], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case garlic:
		np = new Garlic(plant_list[garlic], yard_pointer);
		yard[yard_pointer.high][yard_pointer.width].p = np;
		break;
	case pumpkin:
	{
		yard[yard_pointer.high][yard_pointer.width].p->inHealth(plant_list[pumpkin].health);
		break;
	}
	default:
		errlog("create_plant:try to create an plant unexisted");
		return false;
	}
	grade += 50;
    if(np!=nullptr){
        scene->addItem(np->body);
        connect(np,SIGNAL(createBullet(plant*)),this,SLOT(createBullet(plant*)));
        connect(this,SIGNAL(pause()),np,SLOT(pauseSlot()));
        connect(this,SIGNAL(gameContinue()),np,SLOT(continueSlot()));
    }
	return true;
}

bool game::create_zombie()
{
	zombie* nz = nullptr;
    int index = qrand()%10;
	for (int i = 0; i < 10; i++)
	{
        if (zombie_list[(i + index)%10].name !=(string) "NULL" && zombie_list[(i + index)%10].number > 0)
		{
			switch ((i + index)%10)
			{
			case normal:
				nz = new Normal(zombie_list[(i + index)%10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
				break;
			case conehead:
				nz = new Conehead(zombie_list[(i + index) % 10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
				break;
			case reading:
				nz = new Reading(zombie_list[(i+index) % 10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
				break;
			case pole:
				nz = new Pole(zombie_list[(i + index) % 10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
				break;
			case clown:
				nz = new Clown(zombie_list[(i + index) % 10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
				break;
			case throwstone:
				nz = new Throwstone(zombie_list[(i + index) % 10]);
				yard[nz->get_position().high][(int)nz->get_position().width].push_zombie(nz);
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
        b=new Bullet("GreenBullet",obj_color::green,30,16,{s->getPosition().high,(float)s->getPosition().width});

    }
    else if(s->getName()==(string)"iceshoot")
    {
        b=new Bullet("BlueBullet",obj_color::green,30,16,{s->getPosition().high,(float)s->getPosition().width});
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
        click_location=pointer_location::store;
        return ;
    }
    if(chooseToRemove==true&&click_location==pointer_location::onyard)
    {
        this->main_screen->ui->shovel->setStyleSheet("border-image: url(:/image/source/Shovel.png)");
        chooseToRemove=false;
        click_location=pointer_location::store;
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

    }
}

void game::zombieSuccess()
{
    game_pause();
    game_finished=true;
    screen::putResult(-1,grade);

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

void game::plant1BeSelected()
{
    emit(plantSelectedChanged(0));
}
void game::plant2BeSelected()
{
    emit(plantSelectedChanged(1));
}
void game::plant3BeSelected()
{
    emit(plantSelectedChanged(2));
}
void game::plant4BeSelected()
{
    emit(plantSelectedChanged(3));
}
void game::plant5BeSelected()
{
    emit(plantSelectedChanged(4));
}
void game::plant6BeSelected()
{
    emit(plantSelectedChanged(5));
}
void game::plant7BeSelected()
{
    emit(plantSelectedChanged(6));
}
void game::plant8BeSelected()
{
    emit(plantSelectedChanged(7));
}
void game::plant9BeSelected()
{
    emit(plantSelectedChanged(8));
}
void game::plant10BeSelected()
{
    emit(plantSelectedChanged(9));
}

void game::dealPlantDead(plant *s)
{
    yard[s->getPosition().high][s->getPosition().width].p=nullptr;
    s->disconnect();
    delete s;
}

void game::dealZombieDead(zombie *s)
{
    yard[s->get_position().high][(int)s->get_position().width].pop_zombie(s);
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
    emit(die(this));
    cout<<"game will exit"<<endl;
}

void game::generate_money()
{
    sun* soney=new sun;
    soney->setPos(screen::PlantBase().width()+qrand()%(screen::size_info.screen_width*screen::YardSize().width()),screen::PlantBase().height());
    scene->addItem(soney);
    connect(soney,SIGNAL(beCollected(sun*)),this,SLOT(sunBeCollected(sun*)));
    connect(this,SIGNAL(pause()),soney,SLOT(pauseSlot()));
    connect(this,SIGNAL(gameContinue()),soney,SLOT(continueSlot()));
    main_screen->ui->sun->display(user.getMoney());
}

bool game::control_keyboard()
{
	control command = this->getcommand();
	switch (command) 
	{
	case toup:
		pointerMove(toup);
		break;
	case todown:
		pointerMove(todown);
		break;
	case toback:
		switch (game_mode)
		{
		case running_mode:
			if (click_location == onyard) {
				if (chooseToRemove == true)
					chooseToRemove = false;
				yardtostore();
			}
			else
				runtomenu();
			break;
		case menu_mode:
			if (menu_root == 0)
			{
				menu_pointer = 0;
			}
			else
			{
				menu_pointer = menu_root;
				doMenufunc();
			}
			break;
		}
		break;
	case toenter:
		switch (click_location) {
		case onyard:
			if (!chooseToRemove)
				purchase_plant();
			else
				remove_plant();
			break;
		case menu:
			doMenufunc();
			break;
		case store:
			storetoyard();
			break;
		default:
			errlog("control_keyboard:click_location on an unexpected location!");
			game_exit();
			break;
		}
		break;
	case toleft:
		pointerMove(toleft);
		break;
	case toright:
		pointerMove(toright);
		break;
	case toremove:
		switch (game_mode) {
		case running_mode:
			chooseToRemove = true;
			if (click_location == store)
				storetoyard();
			break;
		case menu_mode:
			break;
		}
		break;
	default:
		errlog("control_keyboard:unexpected command!something may error!");
		return false;
		break;
	}
	return true;
}

control game::getcommand()
{
	control res = undef;
	char ch=0;
    while (false) {
        if (true)
            ch = 0;
		switch (ch)
		{
		case 119:res = toup; break;
		case 115:res = todown; break;
		case 97:res = toleft; break;
		case 100:res = toright; break;
		case 27:res = toback; break;
		case 13:res = toenter; break;
		case 72:res=toup; break;
		case 80:res = todown; break;
		case 75:res = toleft; break;
		case 77:res = toright; break;
		case 112:res = toremove; break;
		default:
			if (ch == 0)
				errlog("getcommand:get ch may failed.");
			else if (ch > 0)
				errlog("getcommand:get an ch i can not to understand.");
			break;
		}
		if (res != undef)
			break;
	}
	return res;
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

void game::menutorun()
{
	menu_root = 0;
	menu_pointer = 0;
	game_mode = running_mode;
	click_location = store;
	game_state = running;
	game_continue();
    cout<<"menu to running"<<endl;
}

void game::runtomenu()
{
	game_pause();
	game_mode = menu_mode;
	click_location = menu;
    game_state = on_pause;
    cout<<"run to menu"<<endl;
}

void game::storetomenu()
{
	runtomenu();
    cout<<"store to menu"<<endl;
}

void game::menutostore()
{
	menutorun();
    cout<<"menu to store"<<endl;
}

void game::yardtostore()
{
	click_location = store;
	yard[yard_pointer.high][yard_pointer.width].color = black;
    cout<<"yard to store"<<endl;
}

void game::storetoyard()
{
	click_location = onyard;
	yard[yard_pointer.high][yard_pointer.width].color = blue;
    cout<<"store to yard"<<endl;
}

void game::pointerMove(control c)
{
	switch (click_location)
	{
	case onyard:
    {auto pos = yard_pointer;
		switch(c)
		{	
		case toup:yard_pointer.high=(yard_pointer.high + screen::size_info.screen_high-1) % screen::size_info.screen_high; break;
		case todown:yard_pointer.high=(yard_pointer.high + 1) % screen::size_info.screen_high; break;
		case toleft:yard_pointer.width=(yard_pointer.width +screen::size_info.screen_width-1) % screen::size_info.screen_width; break;
		case toright:yard_pointer.width=(yard_pointer.width + 1) % screen::size_info.screen_width; break;
		default:
			return;
		}
        yard[yard_pointer.high][yard_pointer.width].color = yard[pos.high][pos.width].color;
        yard[pos.high][pos.width].color = black;
		yard[yard_pointer.high][yard_pointer.width].color = blue;
        break;}
	case store:
		switch(c)
		{
		case toup:
			if (store_pointer > 0)
				store_pointer -= 1;
			break;
		case todown:
			if (store_pointer > 9)
			{
				errlog("pointerMove:store_pointer>9!");
				game_exit();
			}
            if (store_pointer<9 &&plant_list[store_pointer].name !=(string) "NULL")
				store_pointer += 1;
			break;
		default:
			break;
		}
		break;
	case menu:
		switch (c)
		{
		case toup:
			if (menu_pointer > menu_root)
				menu_pointer -= 1;
			break;
		case todown:
			if (menu_list[menu_pointer].next != -1)
				menu_pointer = menu_list[menu_pointer].next;
		default:break;
		}
		break;
	default:break;
	}
	
}

bool game::remove_plant()
{
	if (yard[yard_pointer.high][yard_pointer.width].p == NULL)
	{
        screen::putMessage("你这是要移除什么？");
		return false;
	}
	else
	{
		delete yard[yard_pointer.high][yard_pointer.width].p;
		yard[yard_pointer.high][yard_pointer.width].p = NULL;
		screen::putMessage("你成功移除了一颗植物");
        shovelClicked();
		return true;
    }
}

void game::dieAnimation(zombie *s)
{
    role_body* body1=nullptr,*body2=nullptr;
    if(s->getHealth()<-100)
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
    else
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
}

void game::configToDisk()
{
	fstream outfile;

	string path = config_path + "env_config.txt";
	outfile.open(path, ios::out);
	outfile << "space_w" << endl << screen::size_info.node_width << endl;
	outfile << "space_h" << endl << screen::size_info.node_high << endl;
	outfile << "env_w" << endl << screen::size_info.screen_width << endl;
	outfile << "env_h" << endl << screen::size_info.screen_high << endl;

	outfile << "color" << endl<<screen::env_color << endl;
	outfile.close();
}

bool game::doMenufunc()
{
	string menu_command = menu_list[menu_pointer].name;
	if (menu_list[menu_pointer].child != -1)
	{
		menu_root = menu_list[menu_pointer].child;
		menu_pointer = menu_root;
	}
	else if (menu_command == "新游戏")
		menufunc_new();
	else if (menu_command == "继续游戏")
		menufunc_continue();
	else if (menu_command == "暂停游戏")
		menufunc_pause();
	else if (menu_command == "退出游戏")
		menufunc_exit();
	else if (menu_command == "更改环境宽高")
		menufunc_changeSize();
	else if (menu_command == "更改颜色")
		menufunc_changeColor();
	else if (menu_command == "更改僵尸数量")
		menufunc_zombieNum();
	else
	{
		errlog("doMenufunc:unexpected menu_command!");
		errlog((string)"doMenufunc:it is:" + menu_command);
		return false;
	}
	return true;
}

void game::menufunc_exit()
{
	exit_flag = true;
}
void game::menufunc_new()
{
    if (game_state == on_pause)
	{
		system("cls");
		cout << "检测到有未结束的游戏，请注意，正在关闭游戏。。。" << endl;
		game_finished = true;
	}
}
void game::menufunc_continue()
{
    if(game_state==on_pause)
		game_continue();
	else if (game_state == non)
	{
		system("cls");
		cout << "没有正在暂停的游戏，你这是要继续什么？" << endl;
	}
	else
	{
		errlog("menufunc_continue:there is something wrong,I find out that your game_state is running!");
		game_exit();
	}
}
void game::menufunc_pause()
{
	if (game_state == non)
	{
		system("cls");
		cout << "没有正在运行的游戏，你这是要暂停什么？" << endl;
	}
	else if (game_state == running)
	{
		errlog("menufunc_pause:you may get some errors,game_state shouldn't be running at any time.");
		game_exit();
	}
}
void game::menufunc_changeColor()
{
	system("cls");
	cout << "输入你需要的颜色" << endl;
	cout << "目前支持blue,green,purple,yellow,grey,white" << endl;
	string command = "";
	bool getLegalCommand = false;
	while (1) {
		cout << "*************************" << endl;
		cout << "<<<";
		cin >> command;
		getLegalCommand = true;
		if (command == "blue")
			screen::env_color = blue;
		else if (command == "green")
			screen::env_color = green;
		else if (command == "purple")
			screen::env_color = purple;
		else if (command == "yellow")
			screen::env_color = yellow;
		else if (command == "grey")
			screen::env_color = grey;
		else if (command == "white")
			screen::env_color = white;
		else
		{
			cout << "不合法的输入" << endl;
			getLegalCommand = false;
		}
		if (getLegalCommand == true)
			break;
		cout << "*************************" << endl;

	}
	configToDisk();
}
void game::menufunc_changeSize()
{
	system("cls");
	cout << "请输入需要调整的大小，鉴于屏幕的电脑屏幕的宽高，请不要与默认相差太大." << endl;
	cout << "虽然程序本身支持任意调整宽高，但受屏幕限制可能出现显示问题" << endl;
	cout << "*******************************************************" << endl;
	cout << "单块草地宽度(>=12):";
	cin >> screen::size_info.node_width;
	cout << "*******************************************************" << endl;
	cout << "单块草地高度:(>=2):";
	cin >> screen::size_info.node_high;
	cout << "*******************************************************" << endl;
	cout << "行数:" << endl;
	cin >> screen::size_info.screen_high;
	cout << "*******************************************************" << endl;
	cout << "列数:" << endl;
	cin >> screen::size_info.screen_width;
	cout << "*******************************************************" << endl;
	cout << "这项调整后，你至少需要重新启动一个新游戏" << endl;
	game_finished = true;
	configToDisk();
}
void game::menufunc_zombieNum()
{
	system("cls");
	for (int i = 0; i < 10; i++)
	{
        if ((string)zombie_list[i].name !=(string) "NULL")
		{
			cout << "********************" << endl;
			cout << zombie_list[i].name << ":";
			cin >> zombie_list[i].number;
		}
	}
	cout << "这项调整后，你至少需要重新启动一个新游戏" << endl;
	fstream outfile;
	string path = config_path + "info.txt";
	outfile.open(path, ios::out);

	outfile << "MAX_GRADE" << endl;
	outfile << MAX_GRADE << endl;

    outfile << "zombie_num_" << endl;
	for (int i = 0; i < 10; i++)
	{
		outfile << zombie_list[i].number << " ";
	}
	outfile << endl;
	outfile.close();
	game_finished = true;
	configToDisk();
}

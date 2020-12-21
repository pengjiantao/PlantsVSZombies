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
using namespace std;

template struct locate<int, int>;
template struct locate<int,float>;

game::game():QObject(),user()
{
	log("a game object be created");
	ULONGLONG now_time = GetTickCount64();
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

	plant_list = new plant_info[10]{
		{"豌豆射手",100,20,green,100},
		{"向日葵",50,0,green,50},
		{"双发射手",100,20,green,200},
		{"寒冰射手",100,30,green,300},
		{"坚果",500,0,green,100},
		{"高坚果",1000,0,green,200},
		{"窝瓜",1000,500,green,100},
		{"樱桃炸弹",1000,500,green,250},
		{"大蒜",100,0,green,100},
		{"南瓜头",400,0,green,100}
	};
	zombie_list = new zombie_info[10]{
		{300,50,0.3,"路障僵尸",grey,0},
		{150,50,0.3,"读报僵尸",grey,0},
		{100,40,0.5,"撑杆僵尸",grey,0},
		{100,30,0.3,"小丑僵尸",grey,0},
		{400,70,0.2,"投石僵尸",grey,1},
		{100,30,0.3,"普通僵尸",grey,0}
	};

	menu_list = new menu_entry[50];
	chooseToRemove = false;
	zombie_info::ALL_ZOMBIE = false;

    main_screen=new window;
    sun_timer=new QTimer(this);
    sun_timer->setInterval(sunny_cycle*1000);
    sun_timer->stop();
    zombie_timer=new QTimer(this);
    zombie_timer->setInterval(zombie_cycle*1000);
    zombie_timer->stop();
    scene=new GameScene();

    connect(this->zombie_timer,SIGNAL(timeout()),this,SLOT(create_zombie()));
    connect(this->sun_timer,SIGNAL(timeout()),this,SLOT(generate_money()));
}

game::~game()
{
	configToDisk();
	log("a game object be killed");
	for (int i =0;i< screen::size_info.screen_high; i++)
		delete[] yard[i];
    delete sun_timer;
    delete zombie_timer;
    delete main_screen;
	delete[] yard;
	delete[] plant_list;
	delete[] zombie_list;
	delete[] menu_list;
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

    main_screen->show();
    main_screen->ui->main_screen_view->setScene(scene);
    scene->setSceneRect(-100,-100,1000,800);
    //scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    main_screen->ui->main_screen_view->setCacheMode(QGraphicsView::CacheBackground);
    main_screen->ui->main_screen_view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
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
    cout<<"game will pause!"<<endl;
	clock_pause.s_1 = GetTickCount64() - count_clock.time_of_last_frame;
	clock_pause.s_2 = GetTickCount64() - count_clock.time_of_last_money;
	clock_pause.s_3 = GetTickCount64() - count_clock.time_of_last_zombie;
	game_mode = menu_mode;
    game_state = on_pause;
	click_location = menu;
	screen::init_menu(menu_list,menu_root,menu_pointer);
    cout<<"game pause!"<<endl;
}

bool game::game_continue()
{
    cout<<"game will continue!"<<endl;
	count_clock.time_of_last_frame = GetTickCount64() + clock_pause.s_1;
	count_clock.time_of_last_money = GetTickCount64() + clock_pause.s_2;
	count_clock.time_of_last_zombie = GetTickCount64() + clock_pause.s_3;
	game_mode = running_mode;
	game_state = running;
	click_location = store;
    screen::init_game_screen();
	for (int i = 0; i < screen::size_info.screen_high; i++)
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			locate<int, int> p = { i,j };
		}
    cout<<"game continue!"<<endl;
	return true;
}

bool game::purchase_plant() {
	if ((int)store_pointer >= 10 || plant_list[store_pointer].name == "NULL")
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
		else if ((GetTickCount64()-plant_list[store_pointer].last_create)/1000<plant_list[store_pointer].ice_time)
		{
			errlog("ice_time!");
			screen::putMessage("请注意冷却时间");
			return false;
		}
		else if (user.deMoney(plant_list[store_pointer].price))
		{
			plant_list[store_pointer].last_create = GetTickCount64();
			create_plant();
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
	yardtostore();
	grade += 50;
    cout<<"create plant success!"<<endl;
	return true;
}

bool game::create_zombie()
{
	zombie* nz = nullptr;
	int index = (int)(GetTickCount64() % 10);
	for (int i = 0; i < 10; i++)
	{
		if (zombie_list[(i + index)%10].name != "NULL" && zombie_list[(i + index)%10].number > 0)
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
            cout<<"create zombie success"<<endl;
			return true;
		}
	}
    cout<<"all zombie generated"<<endl;
	zombie_info::ALL_ZOMBIE = true;
	return true;
}


void game::generate_money()
{
	if (!user.inMoney(sunny_granularity))
	{
        errlog("generate_money:unexpected error when create sun!");
	}
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
				screen::flash_menu(menu_list,menu_root,menu_pointer);
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
		if (z[i] == NULL)
		{
			z[i] = zom;
			return true;
		}
	}
	log("zombies generate too much in an same space!");
	return false;
}

bool yard_node::pop_zombie(zombie* zom)
{
	for (int i = 0; i < 10; i++)
	{
		if (z[i] == zom)
		{
			z[i] = NULL;
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
			if (store_pointer<9 &&plant_list[store_pointer].name != "NULL")
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
		screen::flash_menu(menu_list,menu_root,menu_pointer);
		break;
	default:break;
	}
	
}

bool game::remove_plant()
{
	if (yard[yard_pointer.high][yard_pointer.width].p == NULL)
	{
		screen::putMessage("你这是要移除什么？");
		log("try to remove an unexsited plant!");
		return false;
	}
	else
	{
		delete yard[yard_pointer.high][yard_pointer.width].p;
		yard[yard_pointer.high][yard_pointer.width].p = NULL;
		screen::putMessage("你成功移除了一颗植物");
		yardtostore();
        cout<<"remove one plant"<<endl;
		return true;
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
		screen::flash_menu(menu_list, menu_root, menu_pointer);
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
		screen::flash_menu(menu_list,menu_root,menu_pointer);
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
		screen::flash_menu(menu_list, menu_root, menu_pointer);
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
	screen::flash_menu(menu_list,menu_root,menu_pointer);
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

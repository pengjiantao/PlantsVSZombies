#include"game.h"
#include"screen.h"
#include"swfunix.h"
#include"plant.h"
#include"toolfunc.h"
#include<fstream>
using namespace std;
/*显示实例化：plant locate*/
template struct locate<int, int>;

/*显式实例化：zombie locate*/
template struct locate<int,float>;
game::game():user()
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
	//cout << screen::size_info.screen_high << " " << screen::size_info.screen_width << endl;
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
}

game::~game()
{
	configToDisk();
	log("a game object be killed");
	for (int i =0;i< screen::size_info.screen_high; i++)
		delete[] yard[i];
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
	while (!infile.eof() && command != "ZOMBIENUM")
		infile >> command;
	for (int i = 0; !infile.eof() && i < 10; i++)
	{
		infile >> zombie_list[i].number;
	}
	infile.close();
	log("game_init finished");

}

void game::game_fresh()
{
	ULONGLONG this_time = GetTickCount64();
	double time_diff = this_time - count_clock.time_of_last_frame;
	//cout << time_diff << endl;
	screen::flash_store(plant_list,store_pointer);
	if (time_diff > 100)
		errlog("game_fresh:the frame rate is too slow!");
	bool finish = true;
	for(int i=0;i<screen::size_info.screen_high;i++)
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			yard[i][j].find_first(time_diff);
			if (yard[i][j].p != NULL)
			{
				if (!yard[i][j].p->attack(time_diff, yard))
					user.inMoney(50);
			}
			for (int k = 0; k < 10; k++)
				if (yard[i][j].z[k] != NULL)
				{
					if(yard[i][j].z[k]->getName()!="@")
						finish = false;

					if (yard[i][j].z[k]->attack(time_diff, yard))
					{
						if (yard[i][j].z[k]->getName() == "@")
						{
							delete yard[i][j].z[k];
							yard[i][j].z[k] = NULL;
						}
						else {
							MAX_GRADE = (grade > MAX_GRADE) ? grade : MAX_GRADE;
							screen::putResult(0, grade);
							runtomenu();
						}
					}
				}
			screen::flash_node({ i,j }, yard);
		}
	if (finish == true && zombie_info::ALL_ZOMBIE == true)
	{
		grade += 1000;
		screen::putResult(1, grade);
		runtomenu();
	}
	
	if ((this_time - count_clock.time_of_last_zombie) / 1000 >= zombie_cycle)
	{
		create_zombie();
		count_clock.time_of_last_zombie = this_time;
	}
	if ((this_time-count_clock.time_of_last_money)/1000 >= sunny_cycle)
	{
		generate_money();
		count_clock.time_of_last_money = this_time;
	}
	count_clock.time_of_last_frame = this_time;
}

void game::break_log()
{
	system("cls");
	cout << "GENERATING BLOCK FILE..." << endl;
	string path = config_path + "block.txt";
	fstream outfile;
	outfile.open(path.c_str(),ios::out);
	outfile << "Your game failed or you aimed to generate this file:" << endl;
	outfile << "These are what I can provide:" << endl;
	outfile << "Game class:" << endl;

	outfile << endl;
	outfile << "zombie_list" << endl;
	outfile << "name health attack_power color number speed" << endl;
	for (int i = 0; i < 10; i++)
		outfile << zombie_list[i].name << " " << zombie_list[i].health << " " << zombie_list[i].attack_power << " "
		<< zombie_list[i].color << " " << zombie_list[i].number << " " << zombie_list[i].speed << endl;
	outfile << "ALL_ZOMBIE"<<" "<<zombie_info::ALL_ZOMBIE << endl;

	outfile << endl;
	outfile << "plant_list" << endl;
	outfile << "attack_power color health ice_time last_create name price" << endl;
	for (int i = 0; i < 10; i++)
		outfile << plant_list[i].attack_power << " " << plant_list[i].color << " " << plant_list[i].health << " " << plant_list[i].ice_time <<
		" " << plant_list[i].last_create << " " << plant_list[i].name << " " << plant_list[i].price << endl;

	outfile << endl;
	outfile << "menu_list" << endl;
	outfile << "name next_menu child_menu" << endl;
	for (int i = 0; i < 50; i++)
		outfile << menu_list[i].name << " " << menu_list[i].next << " " << menu_list[i].child << endl;

	outfile << endl;
	outfile << "count_clock" << endl;
	outfile << "last_time_of_frame:" << count_clock.time_of_last_frame << endl;
	outfile << "last_time_of_zombie:" << count_clock.time_of_last_zombie << endl;
	outfile << "last_time_of_money:" << count_clock.time_of_last_money << endl;

	outfile << endl;
	outfile << "clock_pause" << endl;
	outfile << "s_1:" << clock_pause.s_1 << endl;
	outfile << "s_2:" << clock_pause.s_2 << endl;
	outfile << "s_3:" << clock_pause.s_3 << endl;
	
	outfile << endl;
	outfile << "game_mode: ";
	switch (game_mode)
	{
	case running_mode:outfile << "running_mode" << endl; break;
	case menu_mode:outfile << "menu_mode" << endl; break;
	default:
		outfile << "unexpected" << endl;
		break;
	}

	outfile << endl;
	outfile << "game state: ";
	switch (game_state) {
    case on_pause:outfile << "pause" << endl; break;
	case running:outfile << "running" << endl; break;
	case non:outfile << "non" << endl; break;
	default:outfile << "unexpected" << endl; break;
	}

	outfile << endl;
	outfile << "click location: ";
	switch (click_location) {
	case onyard:
		outfile << "onyard" << endl;
		break;
	case menu:
		outfile << "menu" << endl;
		break;
	case store:
		outfile << "store" << endl;
		break;
	default:
		outfile << "unexpected" << endl;
		break;
	}

	outfile << endl;
	outfile << "exitflag: " << exit_flag << endl;

	outfile << endl;
	outfile << "game_finished: " << game_finished << endl;

	outfile << endl;
	outfile << "menu_root: " << menu_root << endl;

	outfile << endl;
	outfile << "store_pointer " << store_pointer << endl;

	outfile << endl;
	outfile << "menu_pointer " <<menu_pointer<< endl;

	outfile << endl;
	outfile << "yard_pointer: " << endl;
	outfile << yard_pointer.high << " " << yard_pointer.width << endl;

	outfile << endl;
	outfile << "yard: " << endl;
	outfile << "         plant name" << endl;
	for (int i = 0; i < screen::size_info.screen_high; i++)
	{
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			if (yard[i][j].p != NULL)
				outfile << yard[i][j].p->getName() << " ";
			else
				outfile << "NULL" << " ";
		}
		outfile << endl;
	}
	outfile << "         plant health" << endl;
	for (int i = 0; i < screen::size_info.screen_high; i++)
	{
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			if (yard[i][j].p != NULL)
				outfile << yard[i][j].p->getHealth() << " ";
			else
				outfile << "NULL" << " ";
		}
		outfile << endl;
	}

	outfile << "      zombie num" << endl;
	for (int i = 0; i < screen::size_info.screen_high; i++)
	{
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			int num = 0;
			for (int k = 0; k < 10; k++)
			{
				if (yard[i][j].z[k] != NULL)
					num++;
			}
			outfile << num << " ";
		}
		outfile << endl;
	}

	outfile << endl;
	outfile << "user::money: " << user.getMoney() << endl;


	outfile << endl << "game::choosetoremove: " << this->chooseToRemove << endl;

	outfile.close();
}

bool game::game_start()
{
	log("a game start now!");
	screen::init_game_screen(plant_list, user.getMoney());
	screen::putMessage("游戏开始！");
	screen::flash_node({ 0,0 }, yard);
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
	ULONGLONG now_time = GetTickCount64();
	count_clock.time_of_last_frame = now_time;
	count_clock.time_of_last_money = now_time;
	count_clock.time_of_last_zombie = now_time;
	clock_pause.s_1 = 0;
	clock_pause.s_2 = 0;
	clock_pause.s_3 = 0;
	grade = 0;
	while (true)
	{
		while (kbhit())
		{
			if (!control_keyboard())
				errlog("control_keyboard:can't understand input!");
		}
		if (game_finished) {
			log("game_start:game finish,exit this game!");
			break;
		}
		if (exit_flag) {
			log("game_start:exit_flag has been set,exit game!");
			break;
		}
		if (game_mode == running_mode)
			game_fresh();
		//game_exit();
	}
	if (exit_flag) {
		log("exit_flag be true,game will exit.");
		return false;
	}
	else {
		log("game finished,but exit_flag not be set true,a new game will be started");
		return true;
	}
}

void game::game_exit()
{
	log("game encount a deadly error,exit game anyway");
	system("cls");
	cout << "your game encounter an error!" << endl;
	cout << "information be collected will be output to an error file.\nyou can find it in config_path " << endl;
	break_log();
	end();
	exit(0);
}

void game::game_pause()
{
	log("game pause");
	clock_pause.s_1 = GetTickCount64() - count_clock.time_of_last_frame;
	clock_pause.s_2 = GetTickCount64() - count_clock.time_of_last_money;
	clock_pause.s_3 = GetTickCount64() - count_clock.time_of_last_zombie;
	game_mode = menu_mode;
    game_state = on_pause;
	click_location = menu;
	fflush(stdout);
	screen::init_menu(menu_list,menu_root,menu_pointer);
}

bool game::game_continue()
{
	log("game continue");
	count_clock.time_of_last_frame = GetTickCount64() + clock_pause.s_1;
	count_clock.time_of_last_money = GetTickCount64() + clock_pause.s_2;
	count_clock.time_of_last_zombie = GetTickCount64() + clock_pause.s_3;
	game_mode = running_mode;
	game_state = running;
	click_location = store;
	screen::init_game_screen(plant_list,user.getMoney());
	screen::flash_store(plant_list, store_pointer);
	for (int i = 0; i < screen::size_info.screen_high; i++)
		for (int j = 0; j < screen::size_info.screen_width; j++)
		{
			locate<int, int> p = { i,j };
			screen::flash_node(p, yard);
		}
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
			zombie_list[(i + index)%10].number--;
			return true;
		}
	}
	zombie_info::ALL_ZOMBIE = true;
	return true;
}


void game::generate_money()
{
	log("generate sunney");
	if (!user.inMoney(sunny_granularity))
	{
		errlog("generate_money:unexpected error curror,exit game!");
		game_exit();
	}
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
	while (kbhit()) {
		if (kbhit())
			ch = getch();
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
}

void game::runtomenu()
{
	game_pause();
	game_mode = menu_mode;
	click_location = menu;
    game_state = on_pause;
}

void game::storetomenu()
{
	runtomenu();
}

void game::menutostore()
{
	menutorun();
}

void game::yardtostore()
{
	click_location = store;
	screen::flash_store(plant_list, store_pointer);
	yard[yard_pointer.high][yard_pointer.width].color = black;
	screen::flash_node(yard_pointer, yard);
}

void game::storetoyard()
{
	click_location = onyard;
	screen::flash_store(plant_list, -1);
	yard[yard_pointer.high][yard_pointer.width].color = blue;
	screen::flash_node(yard_pointer, yard);
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
		screen::flash_store(plant_list,store_pointer);
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
		Sleep(1000);
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
		Sleep(1000);
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
		Sleep(1000);
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
	Sleep(1000);
	game_finished = true;
	configToDisk();
}
void game::menufunc_zombieNum()
{
	system("cls");
	for (int i = 0; i < 10; i++)
	{
		if (zombie_list[i].name != "NULL")
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

	outfile << "ZOMBIENUM" << endl;
	for (int i = 0; i < 10; i++)
	{
		outfile << zombie_list[i].number << " ";
	}
	outfile << endl;
	outfile.close();
	Sleep(1000);
	game_finished = true;
	configToDisk();
}
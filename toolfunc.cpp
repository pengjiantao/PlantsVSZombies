#include"toolfunc.h"
#include<fstream>
#include<iostream>
#include<string>
#include<stdio.h>
#include"swfunix.h"
#include"plant.h"
#include<time.h>
#include"PlantVSZombie.h"
#include"screen.h"
using namespace std;
bool init()
{
	log("init start");
	string path = config_path + "info.txt";
	fstream infile;
	try {
		/*read info.txt*/
		infile.open(path.c_str(), ios::in);
		if (!infile)
			throw (string)"no info.txt file find,use default config";
		else
		{
			string commad="";
			int data=0;
			while (infile>>commad)
			{
				infile >> data;
				if (commad == "MAX_GRADE")
					MAX_GRADE = data;
			}
		}
		log("read info.txt");
		infile.close();
	}
	catch (string s) {
		cout << s << endl;
	}
	try {
		/*read env_config.txt*/
		path = config_path + "env_config.txt";
		infile.open(path.c_str(), ios::in);
		if (!infile)
			throw (string)"no env_config.txt file find use default config";
		else
		{
			string command = "";
			int data = 0;
			while (infile >> command)
			{
				infile >> data;
				if (command == "space_w")
					space_w = data;
				else if (command == "space_h")
					space_h = data;
				else if (command == "env_w")
					env_w = data;
				else if (command == "env_h")
					env_h = data;
				else if (command == "color")
					screen::env_color = (obj_color)data;
			}
		}
		log("read env_config.txt");
		infile.close();
	}
	catch (string s) {
		cout << s << endl;
	}
	return true;
}

void log(string log)
{
    cout<<"LOG: "<<log<<endl;
	log_entry* p = new log_entry;
	time_t t = time(0);
    char tmp[32] = { 0 };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
	p->long_info = (string)tmp + " "+log;
	p->next = log_list;
	log_list = p;
}

void errlog(string log)
{
    cout<<"ERROR LOG: "<<log<<endl;
	log_entry* p = new log_entry;
	time_t t = time(0);
    char tmp[32] = { 0 };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
	p->long_info = (string)tmp + (string)" Error: " + log;
	p->next = log_list;
	log_list = p;
}

void end()
{
	log("game end,print log info");
	string path = config_path + "log.txt";
	fstream outfile(path.c_str(), ios::out);
	log_entry* current = log_list, * del = NULL;
	while (current != NULL)
	{
		outfile << current->long_info << endl;
		del = current;
		current = current->next;
		delete del;
	}
	outfile.close();
	log_list = NULL;
}

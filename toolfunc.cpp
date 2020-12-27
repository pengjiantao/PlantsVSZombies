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
#include<QtDebug>
#include<QString>
using namespace std;
bool init()
{
	log("init start");
	string path = config_path + "info.txt";
	fstream infile;
	try {
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
                if (command == "env_w")
					env_w = data;
				else if (command == "env_h")
					env_h = data;
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
}

void errlog(string log)
{
    qDebug() << log.c_str();
}

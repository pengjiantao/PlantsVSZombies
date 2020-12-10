#pragma once
#include"plant.h"
#include<string>
using namespace std;

/*normal log*/
void log(string log);

/*error log*/
void errlog(string log);

/*write log_info to log.txt*/
void end();

/*game init,read config file*/
bool init();

/*print colorful characters*/
void printf_plus(const char* s, const obj_color c);

/*print colorful characters with colorful background*/
void printf_plus(const char* s, const obj_color c, const obj_color b);
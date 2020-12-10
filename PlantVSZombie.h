#pragma once
#include<string>
using namespace std;
extern const char* version;
struct log_entry {
	string long_info;
	log_entry* next;
};
extern int space_w;
extern int space_h;
extern int env_w;
extern int env_h;
extern string config_path;
extern int MAX_GRADE;
extern log_entry* log_list;
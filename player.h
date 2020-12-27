#pragma once
#include<string>
using namespace std;
class player {
public:
	bool deMoney(int n);

	bool inMoney(int n);

	player(string _name, int _money);
	player();
	~player();

	int getMoney() const;
private:
	string name;
	int money;
};

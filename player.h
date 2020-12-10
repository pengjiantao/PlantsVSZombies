#pragma once
#include<string>
using namespace std;
class player {
public:
	/*从账号减少n money,余额不足扣款失败返回0，否则返回1*/
	bool deMoney(int n);

	/*向账户中加n money，返回操作结果*/
	bool inMoney(int n);

	player(string _name, int _money);
	player();
	~player();

	/*获取余额*/
	int getMoney() const;
private:

	/*player name*/
	string name;
	int money;
};
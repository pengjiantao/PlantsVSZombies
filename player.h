#pragma once
#include<string>
#include<QObject>
using namespace std;
class player :public QObject{
    Q_OBJECT
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
signals:
    void sunChanged(int);
};

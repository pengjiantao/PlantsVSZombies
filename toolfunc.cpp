#include"toolfunc.h"
#include<iostream>
#include<QtDebug>
#include<QString>
using namespace std;

void log(string log)
{
    cout<<"LOG: "<<log<<endl;
}

void errlog(string log)
{
    qDebug() << log.c_str();
}

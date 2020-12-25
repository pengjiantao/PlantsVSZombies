#pragma once
#include"PlantVSZombie.h"
#include"player.h"
#include"plant.h"
#include<ctime>
#include"unistd.h"
#include"swfunix.h"
#include<QObject>
#include"window.h"
#include<QTimer>
#include<QTime>
#include<QGraphicsScene>
#include"gamescene.h"
#include"sun.h"
#include"pumpkin.h"



enum pointer_location {
    store,onyard
};


class game:public QObject
{
    Q_OBJECT
public:
	bool game_start();

	void game_init();


	void configToDisk();

    window* main_screen;

    static yard_node*** game_yard;

    bool Result();

	game();
	~game();
private:
	player user;

	bool purchase_plant();

	bool create_plant();

	bool remove_plant();

    void dieAnimation(zombie* s);
private:
	pointer_location click_location;

	bool exit_flag;

	bool game_finished;

	int store_pointer;

	locate<int, int> yard_pointer;

    yard_node*** yard;

	plant_info* plant_list;

	zombie_info* zombie_list;

	float sunny_cycle = 2;

	int sunny_granularity = 50;

	float zombie_cycle = 3;

	bool chooseToRemove = false;

	int grade = 0;

    QTimer *sun_timer;
    QTimer *zombie_timer;
    QTimer *plant_ice_action_;
    QTimer *exit_clock_;
    QTimer *zombie_check_;
    GameScene* scene;

    QGraphicsPixmapItem* bgItem ;
    QSize bk_yard_size;

    int numZombieOnYard;
    bool result=true;


private slots:
    void game_pause();

    bool game_continue();

    void generate_money();

    bool create_zombie();

    void createBullet(plant* s);

    void plant_ice();

    void changePlantSelected(int n);
    void plantPrepared(int n);
    void shovelClicked();

    void zombieSuccess();
    void plantSuccess();

    void plant1BeSelected();
    void plant2BeSelected();
    void plant3BeSelected();
    void plant4BeSelected();
    void plant5BeSelected();
    void plant6BeSelected();
    void plant7BeSelected();
    void plant8BeSelected();
    void plant9BeSelected();
    void plant10BeSelected();

    void dealPlantDead(plant* s);
    void dealZombieDead(zombie* s);
    void dealBulletDead(Bullet* s);
    void sunBeCollected(sun* s);

    void dealClickedRequest(QPoint a);

    void dieAnimationEnd(role_body* s);
    void exit_clock_timeout();

    void generate_sun_plant(plant* s);
    void zombie_check_timeout();
signals:
    void plantSelectedChanged(int n);
    void onePlantPrepared(int n);
    void pause();
    void gameContinue();
    void allZombieBeKilled();
    void die(game*);
};

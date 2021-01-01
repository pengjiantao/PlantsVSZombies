#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include<game.h>
#include<QtMultimedia/QSound>
#include<QTimer>
#include"Info.h"
#include"InfoRead.h"
namespace Ui {
class settings;
}

class settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();
    void initProgram();
    string getConfigPath();
private:
    Ui::settings *ui;
    int current_mode_;
    bool start_movie_=true;
    int pass_num_;
    bool background_music_=true;
    int current_num_=0;
    QTimer* back_music_listener_;
    QSound* back_music_=nullptr;
    vector<int> max_grade_;

    void initScreen(int n);
    void playStartMovie();
    void playBackMusic();
    game* current_game_=nullptr;
    void writeConfig();
protected:
    void closeEvent(QCloseEvent* event);
private slots:
    void gameEnd(game* ,int);
    void mode1BeSelected();
    void mode2BeSelected();
    void back_music_listener_slot_();
};

#endif // SETTINGS_H

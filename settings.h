#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include<game.h>
#include<QtMultimedia/QSound>
#include<QTimer>
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
    int current_num_=1;
    QTimer* back_music_listener_;
    QSound* back_music_=nullptr;
    void initScreen(int n);
    void playStartMovie();
    void playBackMusic();
    game* current_game_=nullptr;
private slots:
    void gameEnd();
    void mode1BeSelected();
    void mode2BeSelected();
    void back_music_listener_slot_();
};

#endif // SETTINGS_H

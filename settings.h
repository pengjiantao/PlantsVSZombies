#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include<game.h>
#include<QtMultimedia/QSound>
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
    QSound* back_music_=nullptr;
    void initScreen(int n);
    void playStartMovie();
    void playBackMusic();
    game* current_game_=nullptr;
private slots:
    void gameEnd();
    void mode1BeSelected();
    void mode2BeSelected();
};

#endif // SETTINGS_H

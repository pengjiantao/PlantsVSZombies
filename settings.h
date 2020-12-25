#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include<game.h>
namespace Ui {
class settings;
}

class settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = nullptr);
    ~settings();

private:
    Ui::settings *ui;
    int current_mode_;
private slots:
    void gameEnd(game* s);
    void mode1BeSelected();
    void mode2BeSelected();
};

#endif // SETTINGS_H

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include<QCloseEvent>
namespace Ui {
class window;
}

class window : public QMainWindow
{
    Q_OBJECT

public:
    explicit window(QWidget *parent = nullptr);
    ~window();
protected:
    void closeEvent(QCloseEvent* event);
public:
    Ui::window *ui;
signals:
    void closed();
};

#endif // WINDOW_H

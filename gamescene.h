#ifndef GAMESCENE_H
#define GAMESCENE_H
#include<QGraphicsScene>
#include<QtOpenGL/QGLWidget>

class GameScene:public QGraphicsScene
{
    Q_OBJECT
signals:
    void beClicked(QPointF a);
public:
    GameScene();
    ~GameScene();
protected:
private slots:


};

#endif // GAMESCENE_H

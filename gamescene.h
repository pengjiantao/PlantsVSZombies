#ifndef GAMESCENE_H
#define GAMESCENE_H
#include<QGraphicsScene>
#include<QtOpenGL/QGLWidget>

class GameScene:public QGraphicsScene
{
    Q_OBJECT
signals:
    void beClicked(QPoint a);
public:
    GameScene();
    ~GameScene();
protected:
private slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // GAMESCENE_H

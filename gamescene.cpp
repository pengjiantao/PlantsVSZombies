#include "gamescene.h"
#include"screen.h"
#include<QGraphicsSceneMouseEvent>
GameScene::GameScene()
{

}
GameScene::~GameScene()
{

}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF p=event->scenePos();
    QPoint k={
        ((qint32)p.y()-(qint32)screen::PlantBase().height())/screen::YardSize().height(),
              ((qint32)p.x()-(qint32)screen::PlantBase().width())/screen::YardSize().width()
             };
    QGraphicsScene::mousePressEvent(event);
    emit(beClicked(k));
}


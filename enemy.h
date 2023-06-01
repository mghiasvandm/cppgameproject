#ifndef ENEMY_H
#define ENEMY_H

#include <vector>

#include <QRect>

#include <QPoint>

#include "ship.h"

using namespace std;

enum Status{
    alive,
    dead
};

class Enemy {
public: static vector < Enemy > enemies;
    QRect body;
    QColor color;
    Direction direction;
    Status status;
    Enemy();
    void move();
    void setMovementDirection(Direction direction);
    void changeMovementDirection();
};

#endif // ENEMY_H

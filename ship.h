#ifndef SHIP_H
#define SHIP_H

#include <iostream>

#include <QPoint>

#include <QRect>

#include <QColor>

#include <QKeyEvent>

using namespace std;

enum Direction {
    Left,
    Right,
    None
};

class Ship {
public: size_t score;
    size_t lives;
    QRect body;
    QRect shooter;
    Direction direction;
    QColor color;
    Ship();
    void setDirection(QKeyEvent * event);
    void move(int panelWidth);
};

#endif // SHIP_H

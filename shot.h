#ifndef SHOT_H
#define SHOT_H

#include <iostream>

#include <QPoint>

using namespace std;

enum Goal {
    ship,
    ememy
};

class Shot {
public: Goal goal;
    bool hit = false;
    bool shotFinish = false;
    QPoint startLocation;
    Shot(Goal goal, QPoint startLocation);
};

#endif // SHOT_H

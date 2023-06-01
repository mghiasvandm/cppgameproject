#ifndef PANEL_H
#define PANEL_H

#include <QWidget>

#include <QKeyEvent>

#include "ship.h"

#include "shot.h"

class Panel: public QWidget {
    Q_OBJECT

public:
    explicit Panel(QWidget * parent = nullptr);

protected:
    void paintEvent(QPaintEvent * event) override;
    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

private:
    Ship ship;
    QTimer * timer;
    QVector < Shot > shots;
    QVector < Shot > enemyShots;
    QPair < int, int > line1Range;
    QPair < int, int > line2Range;
    QPair < int, int > line3Range;
    int dynamicYCoordinate;
    bool firstShot;
    bool canShoot;

private slots:
    void updateGame();
    void createShot();
    void handleCollisions();
};

#endif // PANEL_H

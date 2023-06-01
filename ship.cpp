#include "ship.h"

Ship::Ship() {
    score = 0;
    lives = 3;
    color = Qt::green;
    direction = Direction::None;
    int width = 50;
    int height = 30;
    int shooterWidth = 10;
    int shooterHeight = 10;
    int x = 400;
    int y = 550;
    body = QRect(x - width / 2, y - height / 2, width, height);
    shooter = QRect(x - shooterWidth / 2, y - height / 2 - shooterHeight, shooterWidth, shooterHeight);
}

void Ship::setDirection(QKeyEvent * event) {
    if (event -> type() == QEvent::KeyPress) {
        if (event -> key() == Qt::Key_Left) {
            direction = Direction::Left;
        } else if (event -> key() == Qt::Key_Right) {
            direction = Direction::Right;
        }
    } else if (event -> type() == QEvent::KeyRelease) {
        direction = Direction::None;
    }
}

void Ship::move(int panelWidth) {
    int movementSpeed = 25;
    int margin = 25;

    if (direction == Direction::Right) {
        if (body.right() + movementSpeed <= panelWidth - margin) {
            body.moveRight(body.right() + movementSpeed);
            shooter.moveRight(shooter.right() + movementSpeed);
        }
    } else if (direction == Direction::Left) {
        if (body.left() - movementSpeed >= margin) {
            body.moveLeft(body.left() - movementSpeed);
            shooter.moveLeft(shooter.left() - movementSpeed);
        }
    }
}

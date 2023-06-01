#include "enemy.h"

vector < Enemy > Enemy::enemies;

Enemy::Enemy() {
    this->status = alive;
}

void Enemy::move() {
    int dx = (direction == Direction::Right) ? 1 : -1;
    for (Enemy & enemy: enemies) {
        enemy.body.translate(dx, 0);
    }
}

void Enemy::setMovementDirection(Direction newDirection) {
    direction = newDirection;
}

void Enemy::changeMovementDirection() {
    if (direction == Direction::Right) {
        direction = Direction::Left;
    } else {
        direction = Direction::Right;
    }
}

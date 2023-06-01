#include "panel.h"

#include "enemy.h"

#include "shot.h"

#include <QPainter>

#include <QTimer>

#include <QMap>

#include <QFont>

#include <random>

#include <algorithm>

#include <QMessageBox>

#include <cstdlib>

int dynamicYCoordinate;
bool firstShot = true;

Panel::Panel(QWidget * parent): QWidget(parent), canShoot(true) {
    setFixedSize(850, 620);
    ship = Ship();
    setFocusPolicy(Qt::StrongFocus);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGame()));
    timer -> start(100);
    int enemyWidth = 30;
    int enemyHeight = 30;
    int enemySpacing = 20;
    int enemyMargin = 85;
    int x = enemyMargin;
    int y = enemyMargin;

    for (int row = 0; row < 4; row++) {
        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution < int > nDist(3, 4);
        int n = nDist(rng);
        vector < int > indices(6);
        iota(indices.begin(), indices.end(), 0);
        shuffle(indices.begin(), indices.end(), rng);
        for (int i = 0; i < n; ++i) {
            int col = indices[i];
            Enemy enemy;
            enemy.body = QRect(x + col * (enemyWidth + enemySpacing), y + row * (enemyHeight + enemySpacing),
                               enemyWidth, enemyHeight);
            enemy.color = Qt::red;
            Enemy::enemies.push_back(enemy);
        }
        for (int i = n; i < 6; ++i) {
            int col = indices[i];
            Enemy enemy;
            enemy.body = QRect(x + col * (enemyWidth + enemySpacing), y + row * (enemyHeight + enemySpacing),
                               enemyWidth, enemyHeight);
            enemy.color = Qt::blue;
            Enemy::enemies.push_back(enemy);
        }
    }
    for (Enemy & enemy: Enemy::enemies) {
        enemy.setMovementDirection(Direction::Right);
    }

    int lineLength = 80;
    int lineSeparation = 40;
    int line1StartX = ship.body.left() - lineLength - lineSeparation;
    int line2StartX = ship.body.left() + ship.body.width() + lineSeparation;
    int line3StartX = ship.body.left() + (ship.body.width() - lineLength) / 2;

    line1Range = {
        line1StartX - 65,
        line1StartX + lineLength - 85
    };
    line2Range = {
        line2StartX - 75,
        line2StartX - 95 + lineLength
    };
    line3Range = {
        line3StartX + 220,
        line3StartX + lineLength + 200
    };

    QVector<std::pair<Enemy*, int>> redEnemiesToShoot;

    for (Enemy& enemy : Enemy::enemies) {
        if (enemy.color == Qt::red) {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> shootInterval(2000, 4000);
            int interval = shootInterval(rng);

            redEnemiesToShoot.push_back(std::make_pair(&enemy, interval));
        }
    }

    for (auto& pair : redEnemiesToShoot) {
        Enemy* enemy = pair.first;
        int shootTime = pair.second;

        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this, enemy]() {
            if (enemy && enemy->color == Qt::red) {
                QPoint startLocation((enemy->body.right() + enemy->body.left()) / 2, enemy->body.bottom());
                Shot shot(Goal::ship, startLocation);
                shots.push_back(shot);
            }
        });

        timer->start(shootTime);
    }

}

void Panel::paintEvent(QPaintEvent * event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
    painter.setPen(Qt::white);
    painter.setBrush(ship.color);
    painter.drawRect(ship.body);
    QColor boldGreen(0, 128, 0);
    QBrush brush(boldGreen);
    painter.setBrush(brush);
    painter.drawPolygon(ship.shooter);
    QFont font;
    font.setPointSize(16);
    font.setBold(true);
    painter.setFont(font);

    painter.setPen(Qt::white);

    QString livesText = "Lives: " + QString::number(ship.lives);
    QString scoreText = "Score: " + QString::number(ship.score);
    painter.drawText(10, 30, livesText);
    painter.drawText(width() - 120, 30, scoreText);

    for (const Enemy & enemy: Enemy::enemies) {
        QColor color = enemy.color;
        painter.setBrush(color);
        painter.drawRect(enemy.body);
    }

    painter.setPen(Qt::NoPen);
    for (const Shot & shot: shots) {
        if (shot.goal == Goal::ememy) {
            painter.setBrush(Qt::yellow);
        } else if (shot.goal == Goal::ship) {
            painter.setBrush(Qt::white);
        }
        painter.drawRect(QRect(shot.startLocation, QSize(3, 8)));
    }

    painter.setPen(Qt::red);
    painter.drawLine(line1Range.first, ship.body.top() - 60, line1Range.second, ship.body.top() - 60);
    painter.drawLine(line2Range.first, ship.body.top() - 60, line2Range.second, ship.body.top() - 60);
    painter.drawLine(line3Range.first, ship.body.top() - 60, line3Range.second, ship.body.top() - 60);
}

void Panel::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (canShoot && (firstShot || dynamicYCoordinate == -10)) {
            int shipX = ship.body.left() + ship.body.width() / 2;
            if ((shipX >= line1Range.first && shipX <= line1Range.second) ||
                (shipX >= line2Range.first && shipX <= line2Range.second) ||
                (shipX >= line3Range.first && shipX <= line3Range.second)) {
            } else {
                QPoint startLocation(ship.body.left() + ship.body.width() / 2 - 2, ship.body.top() - 10);
                Shot shot(Goal::ememy, startLocation);
                shots.push_back(shot);
                firstShot = false;
                canShoot = false;
                QTimer::singleShot(500, this, [this]() { canShoot = true; });
            }
        }
    }
    ship.setDirection(event);
}


void Panel::keyReleaseEvent(QKeyEvent * event) {
    ship.setDirection(event);
}

void Panel::updateGame() {
    ship.move(width());

    int enemyMargin = 50;
    int panelWidth = width();

    bool changeDirection = false;
    Direction newDirection;

    bool enemiesAtMargin = false;
    for (const Enemy& enemy : Enemy::enemies) {
        int enemyRight = enemy.body.right();
        int enemyLeft = enemy.body.left();

        if (enemyRight >= (panelWidth - enemyMargin) || enemyLeft <= enemyMargin) {
            enemiesAtMargin = true;
            break;
        }
    }

    if (enemiesAtMargin) {
        int nearestColumn = panelWidth / 2;
        int nearestColumnEnemies = 0;
        for (const Enemy& enemy : Enemy::enemies) {
            int enemyCenterX = enemy.body.left() + enemy.body.width() / 2;
            if (std::abs(enemyCenterX - nearestColumn) < std::abs(nearestColumnEnemies)) {
                nearestColumnEnemies = enemyCenterX - nearestColumn;
            }
        }

        if (nearestColumnEnemies == 0) {
            changeDirection = true;
            newDirection = (Enemy::enemies[0].direction == Direction::Right) ? Direction::Left : Direction::Right;
        }
    }

    if (changeDirection) {
        for (Enemy& enemy : Enemy::enemies) {
            enemy.setMovementDirection(newDirection);
            enemy.move();
        }
    } else {
        for (Enemy& enemy : Enemy::enemies) {
            enemy.move();
        }
    }

    createShot();
    handleCollisions();

    update();
}


void Panel::createShot() {
    QVector < Shot > newShots;

    for (const Shot & shot: shots) {
        QPoint newLocation = shot.startLocation;
        int movementSpeed = 50;
        Shot newShot = shot;

        if (newLocation.y() > -10) {
            if (shot.goal == Goal::ememy) {
                newLocation.setY(newLocation.y() - movementSpeed);
                dynamicYCoordinate = newLocation.y() - movementSpeed;
            } else if (shot.goal == Goal::ship) {
                newLocation.setY(newLocation.y() + 8);
            }
        } else if (shot.goal == Goal::ememy) {
            newLocation.setY(-10);
            dynamicYCoordinate = -10;
            newShot.shotFinish = true;
        }

        newShot.startLocation = newLocation;
        newShots.push_back(newShot);
    }
    shots = newShots;
}

void Panel::handleCollisions() {
    bool shotCollisionDetected = false;

    for (auto enemyIt = Enemy::enemies.begin(); enemyIt != Enemy::enemies.end();) {
        Enemy & enemy = * enemyIt;
        Goal shotGoal;
        int right = enemy.body.right();
        int left = enemy.body.left();
        for (auto shotIt = shots.begin(); shotIt != shots.end();) {
            Shot & shot = * shotIt;
            int shotX = shot.startLocation.x();
            int shotY = shot.startLocation.y();
            shotGoal = shot.goal;
            if (shotX >= left && shotX <= right && shotY >= enemy.body.bottom() - 40 && shotY <= enemy.body.bottom() && shotGoal == Goal::ememy) {
                shotCollisionDetected = true;
                shotIt = shots.erase(shotIt);
                enemyIt->status = dead;
                dynamicYCoordinate = -10;
                ship.score++;
                if (ship.score == 24) {
                    QMessageBox::information(this, "Congratulations", "You won the match!");
                    std::exit(0);
                }
                break;
            } else {
                ++shotIt;
            }
        }

        if (shotCollisionDetected && shotGoal == Goal::ememy) {
            enemyIt = Enemy::enemies.erase(enemyIt);
            enemyIt->status = dead;
            shotCollisionDetected = false;
        } else {
            ++enemyIt;
        }
    }

    for (auto shotIt = shots.begin(); shotIt != shots.end();) {
        Shot & shot = * shotIt;
        int shotX = shot.startLocation.x();
        int shotY = shot.startLocation.y();

        if (shot.goal == Goal::ship && ship.body.contains(shotX, shotY)) {
            shotIt = shots.erase(shotIt);
            dynamicYCoordinate = -10;
            ship.lives--;
            if (ship.lives == 0) {
                QMessageBox::information(this, "Game Over", "You lost the match!");
                std::exit(0);
            }
        } else {
            ++shotIt;
        }
    }
    for (auto shotIt = shots.begin(); shotIt != shots.end();) {
        Shot & shot = * shotIt;
        int shotX = shot.startLocation.x();
        int shotY = shot.startLocation.y();

        if ((shotX >= line1Range.first && shotX <= line1Range.second && shotY >= ship.body.top() - 60) ||
            (shotX >= line2Range.first && shotX <= line2Range.second && shotY >= ship.body.top() - 60) ||
            (shotX >= line3Range.first && shotX <= line3Range.second && shotY >= ship.body.top() - 60)) {
            shotIt = shots.erase(shotIt);
            dynamicYCoordinate = -10;
        } else {
            ++shotIt;
        }
    }
}

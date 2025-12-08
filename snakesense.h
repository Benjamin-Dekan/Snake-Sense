#ifndef SnakeSense_H
#define SnakeSense_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QRandomGenerator>
#include <QList>
#include <QPoint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <cstdint>
#include <cstdio>

class SnakeSense : public QWidget {
    Q_OBJECT

public:
    explicit SnakeSense(QWidget *parent = nullptr);
    ~SnakeSense();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    void startGame();
    void moveSnake();
    void generateFood();
    bool checkCollision();
    void gameOver();
    void initSensor();
    void readSensor();

    enum Direction { UP, DOWN, LEFT, RIGHT };

    static const int CELL_SIZE = 24;
    static const int BOARD_WIDTH = 20;
    static const int BOARD_HEIGHT = 11;
    static const int GAME_SPEED = 200;

    QList<QPoint> snake;
    QPoint food;
    Direction direction;
    Direction next_direction;
    QTimer *timer;
    bool running;
    int score;

    int i2cF;
    static const uint8_t accelerator_addr = 0x1D;
    static const uint8_t control_addr = 0x20;
    static const uint8_t data_addr = 0x28;
};

#endif
#include "snakesense.h"
#include <QMessageBox>

SnakeSense::SnakeSense(QWidget *parent)
    : QWidget(parent), running(false), score(0), i2cF(-1) {
    
    setFixedSize(BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE);
    setWindowTitle("Snake Sense");
    
    initSensor(); 
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeSense::gameLoop);
    
    startGame();
}

SnakeSense::~SnakeSense() {
    delete timer;
    if (i2cF >= 0) ::close(i2cF); 
}

// Sets up the I2C / Accelerometer registers
void SnakeSense::initSensor() {
    const char* f = "/dev/i2c-2";

    if ((i2cF = open(f, O_RDWR)) < 0) {
        printf("Failed to open I2C for accelerometer\n");
        return;
    }

    if (ioctl(i2cF, I2C_SLAVE, accelerator_addr) < 0) {
        printf("Failed to connect to accelerometer\n");
        return;
    }

    uint8_t pos[2] = {control_addr, 0x57};
    write(i2cF, pos, 2);
}

// Reads raw data from the sensor and updates the direction based on tilt
void SnakeSense::readSensor() {
    uint8_t r = data_addr | 0x80;
    uint8_t data[6];

    write(i2cF, &r, 1);
    read(i2cF, data, 6);

    // Combine data bytes to get full 16 bit values
    int16_t x = (int16_t)(data[0] | (data[1] << 8));
    int16_t y = (int16_t)(data[2] | (data[3] << 8));
    // int16_t z = (int16_t)(data[4] | (data[5] << 8));

    // change calibration offset/threshold
    int16_t new_x = x + 0;
    int16_t new_y = y + 0; 
    const int THRESHOLD = 8500;

    if (new_x > THRESHOLD && direction != DOWN) {
        next_direction = UP;
    } 
    if (new_x < -THRESHOLD && direction != UP) {
        next_direction = DOWN;
    }
    
    if (new_y > THRESHOLD && direction != RIGHT) {
        next_direction = LEFT;
    } 
    if (new_y < -THRESHOLD && direction != LEFT) {
        next_direction = RIGHT; 
    }
}

// Resets the snake and starts the game loop
void SnakeSense::startGame() {
    snake.clear();
    snake.append(QPoint(BOARD_WIDTH / 2, BOARD_HEIGHT / 2));
    snake.append(QPoint(BOARD_WIDTH / 2 - 1, BOARD_HEIGHT / 2));
    snake.append(QPoint(BOARD_WIDTH / 2 - 2, BOARD_HEIGHT / 2));
    direction = RIGHT;
    next_direction = RIGHT;
    score = 0; 
    running = true;
    
    generateFood();
    timer->start(GAME_SPEED);
}

// Create apple randomly within board boundaries
void SnakeSense::generateFood() {
    bool check_pos = false;
    while (!check_pos) { 
        food.setX(QRandomGenerator::global()->bounded(BOARD_WIDTH));
        food.setY(QRandomGenerator::global()->bounded(BOARD_HEIGHT));

        // Ensure food spawn doesn't overlap with the snake
        check_pos = true;
        for (int i = 0; i < snake.size(); i++) {
            if (snake[i] == food) {
                check_pos = false;
                break;
            }
        }
    }
}

// Reads input, moves snake, checks for collisions, and updates GUI
void SnakeSense::gameLoop() {
    if (!running) {
        return;
    }

    readSensor();
    direction = next_direction;
    moveSnake();
    
    if (checkCollision()) {
        gameOver();
        return;
    }
    
    update();
}

// Calculates updated head pos and handles eating logic
void SnakeSense::moveSnake() {
    QPoint head = snake.first();
    QPoint new_head = head;
    
    switch (direction) { 
        case UP: 
            new_head.setY(head.y() - 1);
            break;
        case DOWN:
            new_head.setY(head.y() + 1);
            break;
        case LEFT:
            new_head.setX(head.x() - 1);
            break;
        case RIGHT:
            new_head.setX(head.x() + 1);
            break;
    }
    
    snake.prepend(new_head);
    
    if (new_head == food) {
        score++;
        generateFood();
    } else {
        snake.removeLast();
    }
}

// Checks if snake collides with walls or itself
bool SnakeSense::checkCollision() {
    QPoint head = snake.first();
    
    if (head.x() < 0 || head.x() >= BOARD_WIDTH ||
        head.y() < 0 || head.y() >= BOARD_HEIGHT) {
        return true;
    }
    
    for (int i = 1; i < snake.size(); ++i) {
        if (head == snake[i]) {
            return true;
        }
    }
    
    return false;
}

// Displays pop-up game-over screen
void SnakeSense::gameOver() {
    running = false;
    timer->stop();
    
    QMessageBox msg;
    msg.setWindowTitle("Game Over");
    msg.setText(QString("Game Over!\nYour score: %1\n\nPress Space to restart").arg(score)); 
    msg.exec();
}

// Qt function to draw the graphics
void SnakeSense::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    painter.fillRect(0, 0, width(), height(), Qt::black);
    
    for (int i = 0; i < snake.size(); ++i) {
        QPoint segment = snake[i];
        
        if (i == 0) {
            painter.setBrush(Qt::darkGreen);
        } else {
            painter.setBrush(Qt::green); 
        }
        
        painter.drawRect(segment.x() * CELL_SIZE, 
                        segment.y() * CELL_SIZE, 
                        CELL_SIZE - 1, 
                        CELL_SIZE - 1);
    }
    
    painter.setBrush(Qt::red);
    painter.drawEllipse(food.x() * CELL_SIZE, 
                       food.y() * CELL_SIZE, 
                       CELL_SIZE - 1, 
                       CELL_SIZE - 1);
    
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(10, 20, QString("Score: %1").arg(score));
    
    if (!running) {
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "Press SPACE to restart");
    }
}

// Handles keyboard inputs (arrows for testing, space for restarting)
void SnakeSense::keyPressEvent(QKeyEvent *event) {
    if (!running && event->key() == Qt::Key_Space) {
        startGame();
        return;
    }
    
    if (!running) {
        return;
    }
    
    switch (event->key()) {
        case Qt::Key_Up:
            if (direction != DOWN) {
                next_direction = UP; 
            }
            break;
        case Qt::Key_Down:
            if (direction != UP) {
                next_direction = DOWN;
            }
            break;
        case Qt::Key_Left:
            if (direction != RIGHT) {
                next_direction = LEFT;
            }
            break;
        case Qt::Key_Right:
            if (direction != LEFT) {
                next_direction = RIGHT;
            }
            break;
        case Qt::Key_Space: // pause mid-game
            if (timer->isActive()) {
                timer->stop();
            } else {
                timer->start(GAME_SPEED); 
            }
            break;
    }

}

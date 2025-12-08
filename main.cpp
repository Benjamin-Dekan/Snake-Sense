#include <QApplication>
#include "snakesense.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    SnakeSense game;
    game.show();
    
    return app.exec();
}
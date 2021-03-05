#include "A_Star_Pathfinding.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    A_Star_Pathfinding w;
    w.resize(1920, 1080);
    w.show();
    return a.exec();
}

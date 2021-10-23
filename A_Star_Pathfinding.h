#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QHeaderView>

#include "ui_A_Star_Pathfinding.h"

class A_Star_Pathfinding : public QMainWindow
{
    Q_OBJECT

public:
    A_Star_Pathfinding(QWidget *parent = Q_NULLPTR);
    ~A_Star_Pathfinding();

private:
#define ROW 38
#define COL 40

    /* Description of the Grid-
     1--> The cell is not blocked
    0--> The cell is blocked */
    int grid[ROW][COL];

    // Creating a shortcut for int, int pair type
    typedef std::pair<int, int> Pair;

    // Creating a shortcut for pair<double, pair<int, int>> type
    typedef std::pair<double, std::pair<int, int>> pPair;

    // A structure to hold the neccesary parameters
    struct cell
    {
        int parent_i, parent_j;
        double f, g, h;
    };

    QMessageBox* messageBox;

    bool isValid(int row, int col);
    bool isUnBlocked(int grid[ROW][COL], int row, int col);
    bool isDestination(int row, int col, Pair dest);
    double calculateHValue(int row, int col, Pair dest);
    void tracePath(cell cellDetails[ROW][COL], int grid[ROW][COL], Pair dest);
    void aStarSearch(int grid[ROW][COL], Pair src, Pair dest);
    void updateBoxColor(int x, int y, QColor color);
    void displayMessage(const QString& message);

    Ui::A_Star_PathfindingClass ui;

private slots:
    void onButtonSolveClicked();
    void onButtonResetClicked();
};

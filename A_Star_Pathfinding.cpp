#include "A_Star_Pathfinding.h"

#include <iostream>
#include <set>
#include <stack>
#include <limits>
#include <ctime>
#include <Windows.h>

A_Star_Pathfinding::A_Star_Pathfinding(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	ui.table->setRowCount(ROW);
	ui.table->setColumnCount(COL);
	
	ui.table->horizontalHeader()->setVisible(false);
	ui.table->verticalHeader()->setVisible(false);

	ui.table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

	ui.table->horizontalHeader()->setDefaultSectionSize(1);
	ui.table->verticalHeader()->setDefaultSectionSize(1);

	ui.table->setShowGrid(false);

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			QTableWidgetItem* item = new QTableWidgetItem;
			ui.table->setItem(i, j, item);
		}
	}

	messageBox = new QMessageBox;

	onButtonResetClicked();

	connect(ui.buttonSolve, &QPushButton::released, this, &A_Star_Pathfinding::onButtonSolveClicked);
	connect(ui.buttonReset, &QPushButton::released, this, &A_Star_Pathfinding::onButtonResetClicked);
}


A_Star_Pathfinding::~A_Star_Pathfinding()
{
	delete messageBox;
}

// A Utility Function to check whether given cell (row, col) 
// is a valid cell or not
bool A_Star_Pathfinding::isValid(int row, int col)
{
	return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

// A Utility Function to check whether the given cell is 
// blocked or not
bool A_Star_Pathfinding::isUnBlocked(int grid[ROW][COL], int row, int col)
{
	return grid[row][col] == 1;
}

// A Utility Function to check whether destination cell has 
// been reached or not
bool A_Star_Pathfinding::isDestination(int row, int col, Pair dest)
{
	return row == dest.first && col == dest.second;
}

// A Utility Function to calculate the 'h' heuristics
double A_Star_Pathfinding::calculateHValue(int row, int col, Pair dest)
{
	// Return using the distance formula 
	return ((double)sqrt((row - dest.first) * (row - dest.first) + (col - dest.second) * (col - dest.second)));
}

// A Utility Function to trace the path from the source 
// to the destination
void A_Star_Pathfinding::tracePath(cell cellDetails[ROW][COL], int grid[ROW][COL], Pair dest)
{
	int row = dest.first;
	int col = dest.second;

	std::stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
		&& cellDetails[row][col].parent_j == col))
	{
		Path.push(std::make_pair(row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	Path.push(std::make_pair(row, col));
	while (!Path.empty())
	{
		std::pair<int, int> p = Path.top();
		grid[p.first][p.second] = 3;
		Path.pop();
	}


	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (grid[i][j] == 3 && !((i == 0 && j == 0) || (i == ROW - 1 && j == COL - 1)))
			{
				updateBoxColor(i, j, QColor(0, 255, 0));
			}
		}
	}

	displayMessage("The destination cell is found!");

	return;
}

void A_Star_Pathfinding::aStarSearch(int grid[ROW][COL], Pair src, Pair dest)
{
	// Either the source or the destination is invalid
	if (!isValid(src.first, src.second) || !isValid(dest.first, dest.second))
	{
		displayMessage("Source or Destination is invalid.");
		return;
	}

	// Either the source or the destination is blocked
	if (!isUnBlocked(grid, src.first, src.second) || !isUnBlocked(grid, dest.first, dest.second))
	{
		displayMessage("Source or Destination is blocked.");
		return;
	}

	// If the destination cell is the same as source cell
	if (isDestination(src.first, src.second, dest))
	{
		displayMessage("We are already at the destination.");
		return;
	}

	// Create a closed list and initialise it to false which means 
	// that no cell has been included yet 
	// This closed list is implemented as a boolean 2D array 
	bool closedList[ROW][COL];
	memset(closedList, false, sizeof(closedList));

	// Declare a 2D array of structure to hold the details 
	// of that cell
	cell cellDetails[ROW][COL];

	// Initializing all nodes to have infinite distance to destination (unknown distance), and -1 as parent node (unknown parent)
	int i, j;

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			cellDetails[i][j].f = FLT_MAX;
			cellDetails[i][j].g = FLT_MAX;
			cellDetails[i][j].h = FLT_MAX;
			cellDetails[i][j].parent_i = -1;
			cellDetails[i][j].parent_j = -1;
		}
	}

	// Initialising the parameters of the starting node
	i = src.first, j = src.second;
	cellDetails[i][j].f = 0.0;
	cellDetails[i][j].g = 0.0;
	cellDetails[i][j].h = 0.0;
	cellDetails[i][j].parent_i = i;
	cellDetails[i][j].parent_j = j;

	/*
	 Create an open list having information as-
	 <f, <i, j>>
	 where f = g + h,
	 and i, j are the row and column index of that cell
	 Note that 0 <= i < ROW & 0 <= j < COL
	 This open list is implemented as a set of pair of pair.*/
	std::set<pPair> openList;

	// Put the starting node on the open list and set its 
	// 'f' as 0
	openList.insert(std::make_pair(0.0, std::make_pair(i, j)));

	while (!openList.empty())
	{
		//Sleep(50);

		pPair p = *openList.begin();

		// Remove this vertex from the open list
		openList.erase(openList.begin());

		// Add this vertex to the closed list
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		/*
		Generating all the 8 successor of this cell

			N.W   N   N.E
			  \   |   /
			   \  |  /
			W----Cell----E
				 / | \
			   /   |  \
			S.W    S   S.E

		Cell-->Popped Cell (i, j)
		N -->  North       (i-1, j)
		S -->  South       (i+1, j)
		E -->  East        (i, j+1)
		W -->  West           (i, j-1)
		N.E--> North-East  (i-1, j+1)
		N.W--> North-West  (i-1, j-1)
		S.E--> South-East  (i+1, j+1)
		S.W--> South-West  (i+1, j-1)*/

		// To store the 'g', 'h' and 'f' of the 8 successors 
		double gNew, hNew, fNew;

		//----------- 1st Successor (North) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j].parent_i = i;
				cellDetails[i - 1][j].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i - 1][j] && isUnBlocked(grid, i - 1, j))
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i - 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j].f == FLT_MAX ||
					cellDetails[i - 1][j].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i - 1, j)));

					// Update the details of this cell 
					cellDetails[i - 1][j].f = fNew;
					cellDetails[i - 1][j].g = gNew;
					cellDetails[i - 1][j].h = hNew;
					cellDetails[i - 1][j].parent_i = i;
					cellDetails[i - 1][j].parent_j = j;

					updateBoxColor(i - 1, j, QColor(255, 0, 0));
				}
			}
		}

		//----------- 2nd Successor (South) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j].parent_i = i;
				cellDetails[i + 1][j].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i + 1][j] && isUnBlocked(grid, i + 1, j))
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i + 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j].f == FLT_MAX ||
					cellDetails[i + 1][j].f > fNew)
				{
					openList.insert(std::make_pair(fNew, std::make_pair(i + 1, j)));
					// Update the details of this cell 
					cellDetails[i + 1][j].f = fNew;
					cellDetails[i + 1][j].g = gNew;
					cellDetails[i + 1][j].h = hNew;
					cellDetails[i + 1][j].parent_i = i;
					cellDetails[i + 1][j].parent_j = j;
					updateBoxColor(i + 1, j, QColor(255, 0, 0));
				}
			}
		}

		//----------- 3rd Successor (East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j + 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j + 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j + 1].parent_i = i;
				cellDetails[i][j + 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i][j + 1] && isUnBlocked(grid, i, j + 1))
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j + 1].f == FLT_MAX ||
					cellDetails[i][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i, j + 1)));

					// Update the details of this cell 
					cellDetails[i][j + 1].f = fNew;
					cellDetails[i][j + 1].g = gNew;
					cellDetails[i][j + 1].h = hNew;
					cellDetails[i][j + 1].parent_i = i;
					cellDetails[i][j + 1].parent_j = j;
					updateBoxColor(i, j + 1, QColor(255, 0, 0));
				}
			}
		}

		//----------- 4th Successor (West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j - 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j - 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j - 1].parent_i = i;
				cellDetails[i][j - 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i][j - 1] && isUnBlocked(grid, i, j - 1))
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j - 1].f == FLT_MAX ||
					cellDetails[i][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i, j - 1)));

					// Update the details of this cell 
					cellDetails[i][j - 1].f = fNew;
					cellDetails[i][j - 1].g = gNew;
					cellDetails[i][j - 1].h = hNew;
					cellDetails[i][j - 1].parent_i = i;
					cellDetails[i][j - 1].parent_j = j;
					updateBoxColor(i, j - 1, QColor(255, 0, 0));
				}
			}
		}

		//----------- 5th Successor (North-East) ------------ 

		// Only process this cell if this is a valid one 
		/*if (isValid(i - 1, j + 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j + 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j + 1].parent_i = i;
				cellDetails[i - 1][j + 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i - 1][j + 1] && isUnBlocked(grid, i - 1, j + 1))
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j + 1].f == FLT_MAX ||
					cellDetails[i - 1][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i - 1, j + 1)));

					// Update the details of this cell 
					cellDetails[i - 1][j + 1].f = fNew;
					cellDetails[i - 1][j + 1].g = gNew;
					cellDetails[i - 1][j + 1].h = hNew;
					cellDetails[i - 1][j + 1].parent_i = i;
					cellDetails[i - 1][j + 1].parent_j = j;
					updateBoxColor(i - 1, j + 1, QColor(255, 0, 0));
				}
			}
		}

		//----------- 6th Successor (North-West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j - 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j - 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j - 1].parent_i = i;
				cellDetails[i - 1][j - 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i - 1][j - 1] && isUnBlocked(grid, i - 1, j - 1))
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i - 1, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j - 1].f == FLT_MAX ||
					cellDetails[i - 1][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew, std::make_pair(i - 1, j - 1)));
					// Update the details of this cell 
					cellDetails[i - 1][j - 1].f = fNew;
					cellDetails[i - 1][j - 1].g = gNew;
					cellDetails[i - 1][j - 1].h = hNew;
					cellDetails[i - 1][j - 1].parent_i = i;
					cellDetails[i - 1][j - 1].parent_j = j;
					updateBoxColor(i - 1, j - 1, QColor(255, 0, 0));
				}
			}
		}

		//----------- 7th Successor (South-East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j + 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j + 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j + 1].parent_i = i;
				cellDetails[i + 1][j + 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i + 1][j + 1] && isUnBlocked(grid, i + 1, j + 1))
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j + 1].f == FLT_MAX ||
					cellDetails[i + 1][j + 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i + 1, j + 1)));

					// Update the details of this cell 
					cellDetails[i + 1][j + 1].f = fNew;
					cellDetails[i + 1][j + 1].g = gNew;
					cellDetails[i + 1][j + 1].h = hNew;
					cellDetails[i + 1][j + 1].parent_i = i;
					cellDetails[i + 1][j + 1].parent_j = j;
					updateBoxColor(i + 1, j + 1, QColor(255, 0, 0));
				}
			}
		}

		//----------- 8th Successor (South-West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j - 1))
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j - 1, dest))
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j - 1].parent_i = i;
				cellDetails[i + 1][j - 1].parent_j = j;
				tracePath(cellDetails, grid, dest);
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (!closedList[i + 1][j - 1] && isUnBlocked(grid, i + 1, j - 1))
			{
				gNew = cellDetails[i][j].g + 1.414;
				hNew = calculateHValue(i + 1, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j - 1].f == FLT_MAX ||
					cellDetails[i + 1][j - 1].f > fNew)
				{
					openList.insert(std::make_pair(fNew,
						std::make_pair(i + 1, j - 1)));

					// Update the details of this cell 
					cellDetails[i + 1][j - 1].f = fNew;
					cellDetails[i + 1][j - 1].g = gNew;
					cellDetails[i + 1][j - 1].h = hNew;
					cellDetails[i + 1][j - 1].parent_i = i;
					cellDetails[i + 1][j - 1].parent_j = j;
					updateBoxColor(i + 1, j - 1, QColor(255, 0, 0));
				}
			}
		}*/
	}

	// When the destination cell is not found and the open 
	// list is empty, then we conclude that we failed to 
	// reach the destination cell. This may happen when the 
	// there is no way to the destination cell (due to blockages) 
	displayMessage("Failed to find the destination cell...");

	return;
}

void A_Star_Pathfinding::onButtonSolveClicked()
{
	// Source is the top-left corner
	Pair src = std::make_pair(0, 0);

	// Destination is the bottom-right corner 
	Pair dest = std::make_pair(ROW - 1, COL - 1);

	// Starting the A* pathfinding algorithm
	aStarSearch(grid, src, dest);
}

void A_Star_Pathfinding::onButtonResetClicked()
{
	// Using the rand() function to generate a random maze
	time_t t;
	srand((unsigned)time(&t));

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int r = rand() % 10;
			if (r < 3) {
				grid[i][j] = 0;
				ui.table->item(i, j)->setBackground(QColor(0, 0, 0));
			}
			else {
				grid[i][j] = 1;
				ui.table->item(i, j)->setBackground(QColor(255, 255, 255));
			}
		}
	}

	// Make sure to set source and destination as unblocked cells
	grid[0][0] = 1;
	ui.table->item(0, 0)->setBackground(QColor(0, 0, 255));

	grid[ROW - 1][COL - 1] = 1;
	ui.table->item(ROW - 1, COL - 1)->setBackground(QColor(0, 0, 255));
}

void A_Star_Pathfinding::updateBoxColor(int x, int y, QColor color)
{
	ui.table->item(x, y)->setBackground(color);
	ui.table->repaint();
}

void A_Star_Pathfinding::displayMessage(const QString& message)
{
	messageBox->setText(message);
	messageBox->exec();
}
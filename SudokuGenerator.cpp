#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

const int GRID_SIZE = 9;
int Answer[GRID_SIZE][GRID_SIZE]; // Global array to store the correct solution
enum difficulty {
    easy = 20,
    medium = 30,
    hard = 40
};
difficulty selectedDifficulty;
// a code where it searches the row and colnum and 3*3 tables so there is no incorrectness.
bool Checker(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num) {
    for (int x = 0; x < GRID_SIZE; x++) {
        if (grid[row][x] == num) {
            return false;
        }
    }


    for (int x = 0; x < GRID_SIZE; x++) {
        if (grid[x][col] == num) {
            return false;
        }
    }


    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }


    return true;
}
// a code where you generate the grid.
bool fillGrid(int grid[GRID_SIZE][GRID_SIZE], int row, int col) {
    if (row == GRID_SIZE - 1 && col == GRID_SIZE) {
        return true;
    }


    if (col == GRID_SIZE) {
        row++;
        col = 0;
    }


    if (grid[row][col] != 0) {
        return fillGrid(grid, row, col + 1);
    }


    for (int num = 1; num <= GRID_SIZE; num++) {
        if (Checker(grid, row, col, num)) {
            grid[row][col] = num;


            if (fillGrid(grid, row, col + 1)) {
                return true;
            }


            grid[row][col] = 0;
        }
    }


    return false;
}
// a sudoko solver where it solve the Sudoko and fidnd out dose it have one solutoin or else
bool solveSudoku(int grid[GRID_SIZE][GRID_SIZE], int &solutionCount, int row = 0, int col = 0) {
    if (row == GRID_SIZE - 1 && col == GRID_SIZE) {
        solutionCount++;
        return true;
    }


    if (col == GRID_SIZE) {
        row++;
        col = 0;
    }


    if (grid[row][col] != 0) {
        return solveSudoku(grid, solutionCount, row, col + 1);
    }


    for (int num = 1; num <= GRID_SIZE; num++) {
        if (Checker(grid, row, col, num)) {
            grid[row][col] = num;


            if (solveSudoku(grid, solutionCount, row, col + 1)) {
                if (solutionCount > 1) {
                    grid[row][col] = 0;
                    return false;
                }
            }


            grid[row][col] = 0;
        }
    }


    return false;
}
//remove the numbers
void removeNumbers(int grid[GRID_SIZE][GRID_SIZE],    difficulty level) {
    int cellsToRemove = level;
    while (cellsToRemove > 0) {
        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;


        if (grid[row][col] != 0) {
            int temp = grid[row][col];
            grid[row][col] = 0;
            //Check if the grid that was removed didn't create 2 solutions
            int solutionCount = 0;
            int testGrid[GRID_SIZE][GRID_SIZE];
            std::copy(&grid[0][0], &grid[0][0] + GRID_SIZE * GRID_SIZE, &testGrid[0][0]);
            solveSudoku(testGrid, solutionCount);


            if (solutionCount != 1) {
                grid[row][col] = temp;
            } else {
                cellsToRemove--;
            }
        }
    }
}
//main function where we create gather all functions
void generateSudoku(int grid[GRID_SIZE][GRID_SIZE],  difficulty  level ) {
    selectedDifficulty = level;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = 0;
        }
    }
    srand(time(0));


    fillGrid(grid, 0, 0);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            Answer[i][j] = grid[i][j];
        }
    }
    removeNumbers(grid, level);


}

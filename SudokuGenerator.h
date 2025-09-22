#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

const int GRID_SIZE = 9;
extern int Answer[9][9];
enum difficulty {
    easy = 20,
    medium = 30,
    hard = 40
};
extern difficulty selectedDifficulty;

// Function declarations
bool Checker(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num);
bool fillGrid(int grid[GRID_SIZE][GRID_SIZE], int row, int col);
bool solveSudoku(int grid[GRID_SIZE][GRID_SIZE], int &solutionCount, int row = 0, int col = 0);
void removeNumbers(int grid[GRID_SIZE][GRID_SIZE], difficulty level);
void generateSudoku(int grid[GRID_SIZE][GRID_SIZE], difficulty level);

#endif // SUDOKU_GENERATOR_H

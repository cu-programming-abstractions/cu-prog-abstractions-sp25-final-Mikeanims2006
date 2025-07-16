#include "generator.h"
#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

// Directions for maze carving: North, East, South, West (2-step moves)
const int CARVE_DIRECTIONS[4][2] = {{-2, 0}, {0, 2}, {2, 0}, {0, -2}};
static mt19937 rng(random_device{}());

bool isInBounds(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

bool isCarved(const vector<string>& maze, int row, int col) {
    return maze[row][col] == ' ';
}

void carvePassage(vector<string>& maze, int startRow, int startCol, int endRow, int endCol) {
    maze[endRow][endCol] = ' ';
    int wallRow = (startRow + endRow) / 2;
    int wallCol = (startCol + endCol) / 2;
    maze[wallRow][wallCol] = ' ';
}

vector<pair<int, int>> getUnvisitedNeighbors(const vector<string>& maze, int row, int col) {
    vector<pair<int, int>> neighbors;
    int rows = maze.size();
    int cols = maze[0].size();

    for (int i = 0; i < 4; i++) {
        int newRow = row + CARVE_DIRECTIONS[i][0];
        int newCol = col + CARVE_DIRECTIONS[i][1];

        if (isInBounds(newRow, newCol, rows, cols) && !isCarved(maze, newRow, newCol)) {
            neighbors.emplace_back(newRow, newCol);
        }
    }

    return neighbors;
}

template<typename T>
void shuffleVector(vector<T>& vec) {
    shuffle(vec.begin(), vec.end(), rng);
}

// ✅ Fully implemented
void recursiveBacktrack(vector<string>& maze, int row, int col) {
    maze[row][col] = ' ';
    auto neighbors = getUnvisitedNeighbors(maze, row, col);
    shuffleVector(neighbors);

    for (auto [newRow, newCol] : neighbors) {
        if (!isCarved(maze, newRow, newCol)) {
            carvePassage(maze, row, col, newRow, newCol);
            recursiveBacktrack(maze, newRow, newCol);
        }
    }
}

void addRandomRooms(vector<string>& maze, int roomRate) {
    int rows = maze.size();
    int cols = maze[0].size();
    int totalWalls = (rows * cols) / 4;
    int roomsToAdd = (totalWalls * roomRate) / 100;

    for (int i = 0; i < roomsToAdd; i++) {
        int row = 2 + (rng() % (rows - 4));
        int col = 2 + (rng() % (cols - 4));

        if (maze[row][col] == '#') {
            maze[row][col] = ' ';
        }
    }
}

void placeStartAndExit(vector<string>& maze) {
    vector<pair<int, int>> openCells;
    int rows = maze.size();
    int cols = maze[0].size();

    for (int r = 1; r < rows - 1; r++) {
        for (int c = 1; c < cols - 1; c++) {
            if (maze[r][c] == ' ') {
                openCells.emplace_back(r, c);
            }
        }
    }

    if (openCells.size() < 2) {
        cout << "Warning: Not enough open cells for start/exit placement!" << endl;
        return;
    }

    maze[openCells[0].first][openCells[0].second] = 'S';
    maze[openCells.back().first][openCells.back().second] = 'E';
}

std::vector<std::string> generateDungeon(int rows, int cols, int roomRate) {
    if (rows % 2 == 0) rows++;
    if (cols % 2 == 0) cols++;
    if (rows < 5 || cols < 5) {
        rows = max(rows, 5);
        cols = max(cols, 5);
    }

    vector<string> maze(rows, string(cols, '#'));
    recursiveBacktrack(maze, 1, 1);        // Start maze generation
    addRandomRooms(maze, roomRate);       // Add rooms for gameplay
    placeStartAndExit(maze);              // Mark start and end points
    return maze;
}

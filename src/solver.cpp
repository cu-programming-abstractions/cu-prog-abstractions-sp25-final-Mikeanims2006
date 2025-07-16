#include "solver.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

Cell findPosition(const std::vector<std::string>& dungeon, char target) {
    for (int r = 0; r < (int)dungeon.size(); r++) {
        for (int c = 0; c < (int)dungeon[r].size(); c++) {
            if (dungeon[r][c] == target) return Cell(r, c);
        }
    }
    return Cell(-1, -1);
}

namespace {
const int NUM_DIRECTIONS = 4;
const int DIRECTIONS[NUM_DIRECTIONS][2] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
};

struct KeyState {
    int row, col, keyMask;
    KeyState() : row(0), col(0), keyMask(0) {}
    KeyState(int r, int c, int k) : row(r), col(c), keyMask(k) {}

    bool operator==(const KeyState& other) const {
        return row == other.row && col == other.col && keyMask == other.keyMask;
    }
};

struct KeyStateHash {
    size_t operator()(const KeyState& k) const {
        return (static_cast<size_t>(k.keyMask) << 16) ^ (static_cast<size_t>(k.row) << 8) ^ static_cast<size_t>(k.col);
    }
};

bool canPassDoor(char door, int keyMask) {
    if (door < 'A' || door > 'F') return true;
    char requiredKey = door - 'A' + 'a';
    return (keyMask >> (requiredKey - 'a')) & 1;
}

int collectKey(char key, int keyMask) {
    if (key >= 'a' && key <= 'f') {
        return keyMask | (1 << (key - 'a'));
    }
    return keyMask;
}

bool isValidPosition(const std::vector<std::string>& dungeon, int r, int c) {
    return r >= 0 && r < (int)dungeon.size() && c >= 0 && c < (int)dungeon[0].size() && dungeon[r][c] != '#';
}

std::vector<Cell> reconstructPath(const std::unordered_map<Cell, Cell, CellHash>& parents,
                                  const Cell& start, const Cell& goal) {
    std::vector<Cell> path;
    Cell current = goal;
    while (!(current == start)) {
        path.push_back(current);
        current = parents.at(current);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<Cell> reconstructKeyPath(const std::unordered_map<KeyState, KeyState, KeyStateHash>& parents,
                                     const KeyState& start, const KeyState& goal) {
    std::vector<Cell> path;
    KeyState current = goal;
    while (!(current.row == start.row && current.col == start.col && current.keyMask == start.keyMask)) {
        path.emplace_back(current.row, current.col);
        current = parents.at(current);
    }
    path.emplace_back(start.row, start.col);
    std::reverse(path.begin(), path.end());
    return path;
}
} // end namespace

std::vector<Cell> bfsPath(const std::vector<std::string>& dungeon) {
    Cell start = findPosition(dungeon, 'S');
    Cell goal = findPosition(dungeon, 'E');
    if (start.r == -1 || goal.r == -1) return {};

    std::queue<Cell> q;
    std::unordered_set<Cell, CellHash> visited;
    std::unordered_map<Cell, Cell, CellHash> parent;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        Cell current = q.front(); q.pop();
        if (current == goal) {
            return reconstructPath(parent, start, goal);
        }

        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            int nr = current.r + DIRECTIONS[i][0];
            int nc = current.c + DIRECTIONS[i][1];
            Cell neighbor(nr, nc);
            if (!isValidPosition(dungeon, nr, nc) || visited.count(neighbor)) continue;
            visited.insert(neighbor);
            parent[neighbor] = current;
            q.push(neighbor);
        }
    }

    return {};
}

std::vector<Cell> bfsPathKeys(const std::vector<std::string>& dungeon) {
    Cell start = findPosition(dungeon, 'S');
    Cell goal = findPosition(dungeon, 'E');
    if (start.r == -1 || goal.r == -1) return {};

    std::queue<KeyState> q;
    std::unordered_set<KeyState, KeyStateHash> visited;
    std::unordered_map<KeyState, KeyState, KeyStateHash> parent;

    KeyState startState(start.r, start.c, 0);
    q.push(startState);
    visited.insert(startState);

    while (!q.empty()) {
        KeyState current = q.front(); q.pop();

        if (current.row == goal.r && current.col == goal.c) {
            return reconstructKeyPath(parent, startState, current);
        }

        for (int i = 0; i < NUM_DIRECTIONS; ++i) {
            int nr = current.row + DIRECTIONS[i][0];
            int nc = current.col + DIRECTIONS[i][1];
            if (!isValidPosition(dungeon, nr, nc)) continue;

            char ch = dungeon[nr][nc];
            if (ch >= 'A' && ch <= 'F' && !canPassDoor(ch, current.keyMask)) continue;

            int newMask = collectKey(ch, current.keyMask);
            KeyState next(nr, nc, newMask);
            if (visited.count(next)) continue;

            visited.insert(next);
            parent[next] = current;
            q.push(next);
        }
    }

    return {};
}

int countReachableKeys(const std::vector<std::string>& dungeon) {
    Cell start = findPosition(dungeon, 'S');
    if (start.r == -1) return 0;

    std::queue<Cell> q;
    std::unordered_set<Cell, CellHash> visited;
    int keyMask = 0;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        Cell current = q.front(); q.pop();
        char ch = dungeon[current.r][current.c];
        keyMask = collectKey(ch, keyMask);

        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            int nr = current.r + DIRECTIONS[i][0];
            int nc = current.c + DIRECTIONS[i][1];
            Cell neighbor(nr, nc);
            if (!isValidPosition(dungeon, nr, nc) || visited.count(neighbor)) continue;
            visited.insert(neighbor);
            q.push(neighbor);
        }
    }

    int count = 0;
    for (int i = 0; i < 6; i++) {
        if ((keyMask >> i) & 1) count++;
    }
    return count;
}

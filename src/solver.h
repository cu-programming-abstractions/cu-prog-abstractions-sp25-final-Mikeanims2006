#pragma once

#include <vector>
#include <string>
#include "cell.h"

std::vector<Cell> bfsPath(const std::vector<std::string>& dungeon);
std::vector<Cell> bfsPathKeys(const std::vector<std::string>& dungeon);
int countReachableKeys(const std::vector<std::string>& dungeon);
Cell findPosition(const std::vector<std::string>& dungeon, char target);
#define IMPLEMENT_OPTIONAL_FUNCTIONS


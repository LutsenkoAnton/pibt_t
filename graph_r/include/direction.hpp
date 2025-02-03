#pragma once

#include <iostream>
#include <vector>

enum Direction {
    X_PLUS = 0,
    Y_PLUS = 1,
    X_MINUS = 2,
    Y_MINUS = 3,
};

int operator-(const Direction& d1, const Direction& d2);
std::vector<Direction> rotate(Direction from, Direction to);

std::ostream& operator<<(std::ostream& out, const Direction& d);




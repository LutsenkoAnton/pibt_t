#include "../include/direction.hpp"
#include <cmath>

int operator-(const Direction& d1, const Direction& d2) {
    int diff =  std::abs(static_cast<int>(d1) - static_cast<int>(d2));
    return std::min(diff, 4-diff);
    // // Returns 0, if directions are along the same axis, otherwise - 1
    //
    // return (static_cast<int>(d1) & 1) ^ (static_cast<int>(d2) & 1);
}

std::vector<Direction> rotate(Direction from, Direction to) {
    if (from - to == 1) {
        return {to};
    } else if (from - to == 2) {
        return {static_cast<Direction>(static_cast<int>(from) ^ 1), to};
    }
    return {};
}

std::ostream& operator<<(std::ostream& out, const Direction& d) {
    switch (d) {
        case X_PLUS:
            out << "X_PLUS";
            break;
        case Y_PLUS:
            out << "Y_PLUS";
            break;
        case X_MINUS:
            out << "X_MINUS";
            break;
        case Y_MINUS:
            out << "Y_MINUS";
            break;
    }
    return out;
}

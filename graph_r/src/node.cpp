#include "../include/node.hpp"

#include <cmath>

int Node::dist(const Node& other) const {
    return std::abs(x - other.x) + std::abs(y - other.y);
}


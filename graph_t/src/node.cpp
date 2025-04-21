#include "../include/node.hpp"

#include <cmath>

int Node::dist(const Node& other) const {
    return std::abs(x - other.x) + std::abs(y - other.y);
}


std::ostream& operator<<(std::ostream& out, const Node& n) {
    return out << "(" << n.x << "," << n.y << ")";
}


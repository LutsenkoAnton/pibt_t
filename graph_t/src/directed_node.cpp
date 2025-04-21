#include "../include/directed_node.hpp"

int DirectedNode::dist(const Node& other) const {
    return n.dist(other);
}
int DirectedNode::dist(const DirectedNode& other) const {
    return n.dist(other.n) + (d - other.d);
}

std::ostream& operator<<(std::ostream& out, const DirectedNode& d) {
    return out << "(" << d.n.x << "," << d.n.y << "," << d.d << ")";
}

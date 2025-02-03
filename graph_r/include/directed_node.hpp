#pragma once

#include "node.hpp"
#include "direction.hpp"

#include <iostream>

struct DirectedNode {
    Node n;
    Direction d;
    DirectedNode(): n(), d(X_PLUS) {}
    DirectedNode(const Node& n, Direction d): n(n), d(d) {}
    DirectedNode(int id, int x, int y, Direction d): n(id, x, y), d(d) {}

    bool operator==(const Node& other) const {
        return n == other;
    }
    bool operator!=(const Node& other) const {
        return n != other;
    }

    bool operator==(const DirectedNode& other) const = default;
    bool operator!=(const DirectedNode& other) const = default;

    int dist(const Node& other) const;
    int dist(const DirectedNode& other) const;
};

std::ostream& operator<<(std::ostream& out, const DirectedNode& d);

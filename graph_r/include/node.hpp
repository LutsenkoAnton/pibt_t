#pragma once

struct Node {
    int id;
    int x;
    int y;

    Node(): id(-1), x(-1), y(-1) {}
    Node(int id, int x, int y): id(id), x(x), y(y) {}

    int dist(const Node& other) const;

    bool operator==(const Node& other) const = default;
    bool operator!=(const Node& other) const = default;
};


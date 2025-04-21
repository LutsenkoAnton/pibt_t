#pragma once
#include <random>
#include <vector>

#include "directed_node.hpp"
#include "node.hpp"

using Path = std::vector<DirectedNode>;

class Graph {
public:
    Graph();
    Graph(const std::string& filename);
    Path getPath(const DirectedNode& start, const Node& target, const std::vector<Node>& prohibited);
    std::string getName() const {
        return name;
    }
    int getDist(const DirectedNode& start, const Node& target) {
        return getPath(start, target, {}).size();
    }

    int getSize() const {
        return height * width;
    }
    bool existNode(int x, int y) const;
    int getID(int x, int y) const;
    int getID(int x, int y, const Direction& d) const;
    int getID(const DirectedNode& d) const;
    Node getNode(int id) const;
    Node getRandomNode();
    DirectedNode getDirectedNode(int id) const;
    std::vector<std::pair<int, DirectedNode>> getNeighbours(const DirectedNode& n) const;
    std::vector<std::pair<int, DirectedNode>> getUndirectedNeighbours(const DirectedNode& n) const;
    std::vector<std::pair<int, DirectedNode>> getInNeighbours(const DirectedNode& n) const;

    void halt(const std::string& msg);
private:
    std::string name;
    int height;
    int width;
    std::vector<std::vector<bool>> exist;
    std::mt19937 gn;
};

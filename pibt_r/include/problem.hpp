#pragma once

#include <graph.hpp>

#include <random>
#include <string>
#include <vector>

class Problem {
public:
    Problem() {};
    Problem(const std::string& name, int limit_agents);
    Problem(const std::string& name, const Graph& g, const std::vector<DirectedNode>& starting_nodes, const std::vector<Node>& target_nodes): gn(20072007), name(name), g(g), starting_nodes(starting_nodes), target_nodes(target_nodes) {}

    void setRandomStartsAndTargets();
    void setWellFromedInstance();

    int getNum() const { return starting_nodes.size(); }
    int getSize() const { return g.getSize(); }
    DirectedNode getStart(int i) const { return starting_nodes[i]; }
    Node getTarget(int i) const { return target_nodes[i]; }
    std::vector<DirectedNode> getStarts() const { return starting_nodes; }
    std::vector<Node> getTargets() const { return target_nodes; }
    Graph getG() const { return g; }
    std::mt19937 gn;
    void write_instance(const std::string& filename);

private:
    std::string name;
    Graph g;
    std::vector<DirectedNode> starting_nodes;
    std::vector<Node> target_nodes;

};

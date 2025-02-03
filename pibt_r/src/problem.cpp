#include "../include/problem.hpp"

#include <algorithm>
#include <iostream>

void Problem::setRandomStartsAndTargets() {
    // initialize
    int num_agents = starting_nodes.size();

    starting_nodes.clear();
    target_nodes.clear();

    // get grid size
    const int N = g.getSize();

    // set starts
    std::vector<int> starts(N);
    std::iota(starts.begin(), starts.end(), 0);
    std::shuffle(starts.begin(), starts.end(), gn);
    int i = 0;
    while (true) {
        Node cur_node = g.getNode(starts[i]);
        while (!g.existNode(cur_node.x, cur_node.y)) {
            ++i;
            cur_node = g.getNode(starts[i]);
            if (i >= N) {
                std::cerr << "number of agents is too large." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        starting_nodes.push_back(DirectedNode(cur_node, Direction::X_PLUS));
        if (static_cast<int>(starting_nodes.size()) == num_agents) break;
        ++i;
    }

    // set goals
    std::vector<int> goals(N);
    std::iota(goals.begin(), goals.end(), 0);
    std::shuffle(goals.begin(), goals.end(), gn);
    int j = 0;
    while (true) {
        Node cur_node = g.getNode(goals[j]);
        while (!g.existNode(cur_node.x, cur_node.y)) {
            ++j;
            cur_node = g.getNode(goals[j]);
            if (j >= N) {
                std::cerr << "set target, number of agents is too large." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        // retry
        if (g.getNode(goals[j]) == starting_nodes[target_nodes.size()].n) {
            target_nodes.clear();
            std::shuffle(goals.begin(), goals.end(), gn);
            j = 0;
            continue;
        }
        target_nodes.push_back(g.getNode(goals[j]));
        if (static_cast<int>(target_nodes.size()) == num_agents) break;
        ++j;
    }
}

void Problem::setWellFromedInstance() {
    // initialize
    int num_agents = starting_nodes.size();

    starting_nodes.clear();
    target_nodes.clear();

    // get grid size
    const int N = g.getSize();
    std::vector<Node> prohibited, starts_goals;
    std::uniform_int_distribution<int> distr(0, N - 1);

    while (static_cast<int>(target_nodes.size()) < num_agents) {
        while (true) {
            // determine start
            Node s;
            do {
                s = g.getNode(distr(gn));
            } while (!g.existNode(s.x, s.y) || std::find(prohibited.begin(), prohibited.end(), s) != prohibited.end());
            DirectedNode ds(s, Direction::X_PLUS);

            // determine goal
            Node t;
            do {
                t = g.getNode(distr(gn));
            } while (!g.existNode(t.x, t.y) || t == s || std::find(prohibited.begin(), prohibited.end(), t) != prohibited.end());

            // ensure well formed property
            auto path = g.getPath(ds, t, starts_goals);
            if (!path.empty()) {
                starting_nodes.push_back(ds);
                target_nodes.push_back(t);
                starts_goals.push_back(s);
                starts_goals.push_back(t);
                for (auto v : path) {
                    if (std::find(prohibited.begin(), prohibited.end(), v.n) == prohibited.end()) prohibited.push_back(v.n);
                }
                break;
            }
        }
    }
}

#include "../include/problem.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

Problem::Problem(const std::string& name, int limit_agents): gn(20072007), name(name) {
#ifdef _SCENDIR_
    std::ifstream file(_SCENDIR_ + name); 
    // std::cerr << _SCENDIR_ << std::endl;
#else 
    std::ifstream file(name);

    // std::cerr << name << std::endl;
#endif
    std::string line;
    getline(file, line); // Skipping version line
    int bucket;
    std::string map_file;
    int map_width;
    int map_height;
    file >> bucket >> map_file >> map_width >> map_height;
    g = Graph(map_file);
    int x, y;
    file >> x >> y;
    starting_nodes.emplace_back(g.getNode(g.getID(x, y)), Direction::X_PLUS);
    file >> x >> y;
    target_nodes.emplace_back(g.getNode(g.getID(x, y)));
    double optimal_dist;
    file >> optimal_dist;
    limit_agents--;
    while ((file >> bucket) && limit_agents--) {
        file >> map_file >> map_width >> map_height;
        file >> x >> y;
        starting_nodes.emplace_back(g.getNode(g.getID(x, y)), Direction::X_PLUS);
        file >> x >> y;
        target_nodes.emplace_back(g.getNode(g.getID(x, y)));
        file >> optimal_dist;
    }
}

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

void Problem::write_instance(const std::string& filename) {
    std::ofstream fout(filename);
    fout << "map_file=" << g.getName() << std::endl;
    fout << "agents=" << starting_nodes.size() << std::endl;
    fout << "seed=20072007" << std::endl;
    fout << "random_problem=0" << std::endl;
    fout << "max_timestep=1000" << std::endl;
    fout << "max_comp_time=5000" << std::endl;
    for (size_t i = 0; i < starting_nodes.size(); ++i) {
        fout << starting_nodes[i].n.x << ',' << starting_nodes[i].n.y << ',' << target_nodes[i].x << ',' << target_nodes[i].y << '\n';
    }
}

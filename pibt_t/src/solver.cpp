#include "../include/solver.hpp"
#include <algorithm>

void print_solution(const Paths& answer, const Problem& p) {
    for (int i = 0; i < answer[0].size(); ++i) {
        std::cout << i << ":";
        for (int j = 0; j < answer.size(); ++j) {
            std::cout << answer[j][i] << ",";
        }
        std::cout << '\n';
    }
    std::cout << answer[0].size() << ":";
    for (int j = 0; j < answer.size(); ++j) {
        std::cout << p.getTarget(j) << ",";
    }
}

Paths normalize_solution(Paths paths) {
    for (auto& path: paths) {
        while (path.size() > 1 && path.back().n == path[path.size() - 2].n) {
            path.pop_back();
        }
    }
    return paths;
}

bool check_solution(const Paths& paths, const Problem& p) {
    if (paths.size() != p.getNum()) return false;
    for (int i = 0; i < p.getNum(); ++i) {
        if (paths[i].back().n != p.getTarget(i)) return false;
    }
    for (size_t ai = 0; ai < p.getNum(); ++ai) {
        const auto& path = paths[ai];
        for (size_t i = 1; i < path.size(); ++i) {
            if (path[i] == path[i - 1]) continue;
            auto neighbours = p.getG().getNeighbours(path[i - 1]);
            bool found = false;
            for (auto [_, v] : neighbours) {
                if (v == path[i]) {
                    found = true;
                    break;
                }
            }
            if (!found){
                std::cerr << "Incorrect move: agent=" << ai << "; time=" << i - 1 << "; from=" << path[i - 1] << "; to=" << path[i] << std::endl;
                return false;
            }
        }
    }
    for (size_t ai = 0; ai < p.getNum(); ++ai) {
        for (size_t aj = ai + 1; aj < p.getNum(); ++aj) {
            for (size_t t = 0; t < std::max(paths[ai].size(), paths[aj].size()); ++t) {
                DirectedNode current_node_i;
                if (paths[ai].size() <= t) {
                    current_node_i = paths[ai].back();
                } else {
                    current_node_i = paths[ai][t];
                }
                DirectedNode current_node_j;
                if (paths[aj].size() <= t) {
                    current_node_j = paths[aj].back();
                } else {
                    current_node_j = paths[aj][t];
                }
                if (current_node_i.n == current_node_j.n) {
                    std::cerr << "Agent vertex collision: node=" << current_node_i.n << "; agent_i=" << ai << "; agent_j=" << aj << "; time=" << t<< std::endl;
                    return false;
                }
                if (t + 1 >= paths[ai].size() || t + 1 >= paths[aj].size()) continue;
                if (paths[ai][t].n == paths[aj][t + 1].n && paths[ai][t + 1].n == paths[aj][t].n) {
                    std::cerr << "Agent edge collision: edge=" << paths[ai][t].n << "," << paths[ai][t + 1].n << "; agent_i=" << ai << "; agent_j=" << aj << "; time=" << t << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}

size_t makespan(const Paths& paths) {
    size_t answer = std::numeric_limits<size_t>::min();
    for (const auto& path: paths) {
        answer = std::max(path.size(), answer);
    }
    return answer;
}

size_t sum_of_costs(const Paths& paths) {
    size_t answer = 0;
    for (const auto& path: paths) {
        answer += path.size();
    }
    return answer;

}

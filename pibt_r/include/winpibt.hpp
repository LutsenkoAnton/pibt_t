#pragma once

#include <graph.hpp>
#include <string>
#include <vector>

#include "solver.hpp"

class WinPIBT: public Solver {
public:
    WinPIBT(const Problem& p, const int WINDOW_LIMIT);
    static const std::string SOLVER_NAME;
    const int WINDOW_LIMIT;
    Paths solve() override;
private:
    struct Agent {
        int id;
        Node target;
        int elapsed;
        int l;
        int init_d;
        float tie_breaker;
    };

    std::vector<Agent> agents;

    Paths planned_paths;

    int getTmax();
    bool checkSolved();

    bool winPIBT(int ai, int t_to, bool please_move);
    int getPosessingAgent(Node d, int time);
    Path getPath(int ai, int t_from, int t_max, Node banned, int owner_ai);
    bool checkValidPath(int ai, const Path& path, int t_from , int t_max, int owner_ai);
};

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

    bool stop_till_new_iteration = false;

    int getTmax();
    bool checkSolved();

    bool winPIBT(int ai, int t_to, bool please_move);
    int getPosessingAgent(Node d, int time);
    bool isVertexConflict(Node n, int time, int ai);
    bool canStay(int ai);
    void stay(int ai, int t_to);
    Path getPath(int ai, int t_from, int t_max, bool please_move, Node banned);
    bool checkValidPath(int ai, const Path& path, int t_from , int t_max);
};

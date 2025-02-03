#pragma once

#include <graph.hpp>
#include <string>
#include <vector>

#include "solver.hpp"

class PIBT: public Solver {
public:
    PIBT(const Problem& p);
    static const std::string SOLVER_NAME;
    Paths solve() override;
private:
    struct Agent {
        int id;
        DirectedNode now;
        DirectedNode next;
        Node target;
        int elapsed;
        int init_d;
        float tie_breaker;
    };

    std::vector<Agent> agents;
    
    std::vector<int> occupied_now;
    std::vector<int> occupied_next;

    bool prioriy_inheritance(int ai, int aj = -1);
};

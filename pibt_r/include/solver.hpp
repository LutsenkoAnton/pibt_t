#pragma once

#include <graph.hpp>

#include "problem.hpp"

using Paths = std::vector<Path>;

class Solver {
public:
    Solver(const Problem& p): p(p) {}
    virtual Paths solve() = 0;
protected:
    Problem p;
};

void print_solution(const Paths& answer, const Problem& p);
Paths normalize_solution(Paths paths);
size_t makespan(const Paths& paths);
size_t sum_of_costs(const Paths& paths);
bool check_solution(const Paths& paths, const Problem& p);

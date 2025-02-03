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

void print_solution(const Paths& answer);

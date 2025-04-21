#include <graph.hpp>
#include <string>

#include "solver.hpp"

class RotateResult: public Solver {
public:
    RotateResult(const Problem& p, const std::string& exec_file);
    static const std::string SOLVER_NAME;
    Paths solve() override;
private:
    std::string exec_file;
};

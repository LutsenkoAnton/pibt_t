#include <pibt.hpp>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    Graph g(argv[1]);
    Problem p(argv[1], g, std::vector<DirectedNode>(100), std::vector<Node>(100));
    p.setRandomStartsAndTargets();
    PIBT solver(p);
    print_solution(solver.solve());
    return 0;
}

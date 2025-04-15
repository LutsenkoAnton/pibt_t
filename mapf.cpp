#include <winpibt.hpp>
#include "pibt.hpp"
#include <rotate_result.hpp>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Wrong number of arguments" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    const int agents_num = std::stoi(argv[3]);
    // Graph g(argv[1]);
    // Problem p(argv[1], g, std::vector<DirectedNode>(agents_num), std::vector<Node>(agents_num));
    Problem p(argv[1], agents_num);
    // p.setRandomStartsAndTargets();
    // p.write_instance("instance.txt");
    Paths solution;
    if (std::string(argv[2]) == "pibt") {
        PIBT solver(p);
        solution = solver.solve();
    } else if (std::string(argv[2]) == "rotated-pibt") {
        RotateResult solver(p, "./mapf");
        solution = solver.solve();
    } else {
        WinPIBT solver(p, 20);
        solution = solver.solve();
    }
    if (check_solution(solution, p)) {
        std::cerr << "SOLVED" << std::endl;
    } else {
        std::cerr << "WRONG SOLUTION" << std::endl;
    }
    print_solution(solution, p);
    solution = normalize_solution(solution);
    std::cerr << makespan(solution) << ' ' << sum_of_costs(solution) << std::endl;
    // } else {
    //     std::cout << "No solution" << std::endl;
    // }
    return 0;
}

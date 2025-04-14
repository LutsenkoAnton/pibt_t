#include <rotate_result.hpp>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int agents_num = 5;
    Problem p(argv[1], agents_num);
    RotateResult solver(p, "./mapf");
    auto solution = solver.solve();
    if (check_solution(solution, p)) {
        print_solution(solution, p);
    } else {
        std::cerr << "NO SOLUTION" << std::endl;
    }
    return 0;
}


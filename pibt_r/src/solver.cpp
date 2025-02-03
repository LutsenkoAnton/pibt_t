#include "../include/solver.hpp"

void print_solution(const Paths& answer) {
    for (int i = 0; i < answer[0].size(); ++i) {
        std::cout << i << ":";
        for (int j = 0; j < answer.size(); ++j) {
            std::cout << answer[j][i] << ",";
        }
        std::cout << '\n';
    }
}

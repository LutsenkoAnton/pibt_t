#include <winpibt.hpp>
#include <pibt.hpp>
#include <solver.hpp>
#include <problem.hpp>
#include <rotate_result.hpp>
#include <filesystem>
#include <vector>

using Metadata = std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<double>>;

Metadata run_solver(const std::string& solver_str, int limit_agents, const std::filesystem::path& scen_dir) {
    // std::cerr << "Current agent count:" << limit_agents << std::endl;
    std::vector<size_t> makespan_vec;
    std::vector<size_t> sum_of_costs_vec;
    std::vector<double> time_taken;
    for (const auto& dir_entry : std::filesystem::directory_iterator{scen_dir}) {
        Problem p(dir_entry.path(), limit_agents);

        Paths solution;
        if (solver_str == "pibt") {
            PIBT solver(p);
            auto start = std::chrono::steady_clock::now();
            solution = solver.solve();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken.push_back(diff.count());
        } else if (solver_str == "winpibt5") {
            WinPIBT solver(p, 5);
            auto start = std::chrono::steady_clock::now();
            solution = solver.solve();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken.push_back(diff.count());
        } else if (solver_str == "winpibt10") {
            WinPIBT solver(p, 10);
            auto start = std::chrono::steady_clock::now();
            solution = solver.solve();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken.push_back(diff.count());
        } else if (solver_str == "winpibt20") {
            WinPIBT solver(p, 20);
            auto start = std::chrono::steady_clock::now();
            solution = solver.solve();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken.push_back(diff.count());
        } else if (solver_str == "rotate_result") {
            RotateResult solver(p, "./mapf");
            auto start = std::chrono::steady_clock::now();
            solution = solver.solve();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken.push_back(diff.count());
        }

        if (check_solution(solution, p)) {
            solution = normalize_solution(solution);
            makespan_vec.emplace_back(makespan(solution));
            sum_of_costs_vec.emplace_back(sum_of_costs(solution));
        } else {
            makespan_vec.emplace_back(0);
            sum_of_costs_vec.emplace_back(0);
        }
    }
    return std::make_tuple(makespan_vec, sum_of_costs_vec, time_taken);
    // size_t makespan_sum = 0;
    // size_t sum_of_costs_sum = 0;
    // for (const auto mkspan : makespan_vec) {
    //     makespan_sum += mkspan;
    // }
    // for (const auto soc : sum_of_costs_vec) {
    //     sum_of_costs_sum += soc;
    // }
    // std::cout << ',' << 100.0 * makespan_vec.size() / scen_count << ','
    //           << static_cast<double>(makespan_sum) / makespan_vec.size() << ','
    //           << static_cast<double>(sum_of_costs_sum) / sum_of_costs_vec.size();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    int agent_num = std::stoi(argv[2]);
    const std::filesystem::path scen_dir(std::string("../scen/") + argv[1] + "/scen-even/");
    std::cout << "agent_num";
    // std::vector<std::string> names = {"winpibt10", "pibt", "rotate_result"};
    std::vector<std::string> names = {"winpibt10", "winpibt20", "pibt",  "rotate_result"};
    if (agent_num > 20) {
        names = {"winpibt10", "winpibt20",  "rotate_result"};
    }
    for (const auto& name : names) {
        std::cout << ",solved_" << name;
        std::cout << ",makespan_" << name;
        std::cout << ",sum_of_costs_" << name;
        std::cout << ",time_taken_" << name;
    }
    std::cout << std::endl;
    for (int limit_agents = 1; limit_agents <= agent_num; ++limit_agents) {
        std::cerr << "Current agent count: " << limit_agents << std::endl;
        std::cout << limit_agents;
        std::vector<Metadata> metadatas;
        std::vector<bool> solved_instances;
        for (const auto& name : names) {
            metadatas.emplace_back(run_solver(name, limit_agents, scen_dir));
            if (solved_instances.empty()) {
                solved_instances.assign(get<0>(metadatas.back()).size(), true); 
            }
            size_t solved_count = 0;
            for (size_t mkspan: get<0>(metadatas.back())) {
                solved_count += (mkspan != 0);
            }
            if (static_cast<double>(solved_count) / solved_instances.size() < 0.2) {
                get<0>(metadatas.back()).assign(solved_instances.size(), 0);
                get<1>(metadatas.back()).assign(solved_instances.size(), 0);
                get<2>(metadatas.back()).assign(solved_instances.size(), 0);
            } else {
                for (size_t i = 0; i < solved_instances.size(); ++i) {
                    solved_instances[i] = solved_instances[i] && (get<0>(metadatas.back())[i] != 0);
                }
            }
        }
        size_t all_solved = 0;
        for (auto is_solved : solved_instances) {
            all_solved += is_solved;
        }

        for (const auto& [makespan_vec, sum_of_costs_vec, time_taken_vec] : metadatas) {
            size_t solved_count = 0;
            for (size_t mkspan: makespan_vec) {
                solved_count += (mkspan != 0);
            }
            std::cout << ',' << 100.0 * solved_count / solved_instances.size();
            size_t makespan_sum = 0;
            size_t sum_of_costs_sum = 0;
            double time_taken_sum = 0;
            for (size_t i = 0; i < solved_instances.size(); ++i) {
                if (!solved_instances[i]) continue;
                makespan_sum += makespan_vec[i];
                sum_of_costs_sum += sum_of_costs_vec[i];
                time_taken_sum += time_taken_vec[i];
            }
            std::cout << ',' << static_cast<double>(makespan_sum) / all_solved;
            std::cout << ',' << static_cast<double>(sum_of_costs_sum) / all_solved;
            std::cout << ',' << time_taken_sum / all_solved;
        }

        std::cout << '\n';
    }
    // std::cout << makespan(normalize_solution(solver.solve())) << std::endl;
    return 0;
}

#include "../include/rotate_result.hpp"

#include <fstream>
#include <regex>

const std::string RotateResult::SOLVER_NAME = "RotateResult";

RotateResult::RotateResult(const Problem& p, const std::string& exec_file): Solver(p), exec_file(exec_file) {
}

Paths RotateResult::solve() {
    p.write_instance("instance.txt");
    system((exec_file + " -i instance.txt -s PIBT -o mapf-out.txt 1>&2").c_str());
    std::ifstream fin("mapf-out.txt");
    std::string line;
    std::smatch results;
    std::regex r_agents = std::regex(R"(agents=(\d+))");
    std::regex r_makespan = std::regex(R"(makespan=(\d+))");
    int agent_num = 0;
    int makespan = 0;
    while (getline(fin, line)) {
        if (line.back() == 0x0d) line.pop_back();
        if (std::regex_match(line, results, r_agents)) {
            agent_num = std::stoi(results[1].str());
        }
        if (std::regex_match(line, results, r_makespan)) {
            makespan = std::stoi(results[1].str());
        }
        if (line == "solution=") break;
    }
    std::regex r_position = std::regex(R"((?:\((\d+),(\d+)\),))");
    getline(fin, line);
    Paths answer(agent_num);
    std::vector<DirectedNode> agent_positions(agent_num);
    for (int j = 0; j < agent_num; ++j) {
        if (std::regex_search(line, results, r_position)) {
            agent_positions[j].n.x = std::stoi(results[1].str());
            agent_positions[j].n.y = std::stoi(results[2].str());
            agent_positions[j].d = Direction::X_PLUS;
            answer[j].emplace_back(agent_positions[j]);
            line = results.suffix().str();
        }
    }
    for (int i = 1; i <= makespan; ++i) {
        getline(fin, line);
        // std::cerr << line << std::endl;
        std::vector<std::vector<Direction>> rotations(agent_num);
        size_t max_rotation = 0;

        for (int j = 0; j < agent_num; ++j) {
            if (std::regex_search(line, results, r_position)) {
                agent_positions[j].n.x = std::stoi(results[1].str());
                agent_positions[j].n.y = std::stoi(results[2].str());
                agent_positions[j].d = (answer[j].back().n == agent_positions[j].n ? answer[j].back().d : get_direction(answer[j].back().n, agent_positions[j].n));
                rotations[j] = rotate(answer[j].back().d, agent_positions[j].d);
                max_rotation = std::max(max_rotation, rotations[j].size());
                line = results.suffix().str();
            }
        }
        for (int j = 0; j < agent_num; ++j) {
            for (int k = 0; k < rotations[j].size(); ++k) {
                answer[j].emplace_back(answer[j].back().n, rotations[j][k]);
            }
            for (int k = rotations[j].size(); k < max_rotation; ++k) {
                answer[j].emplace_back(answer[j].back());
            }
            answer[j].emplace_back(agent_positions[j]);
        }
    }
    return answer;
}

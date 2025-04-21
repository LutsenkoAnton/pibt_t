#include "../include/winpibt.hpp"
#include <algorithm>
#include <map>
#include <queue>
#include <utility>

#include <graph.hpp>

const std::string WinPIBT::SOLVER_NAME = "WinPIBT";

WinPIBT::WinPIBT(const Problem& p, const int WINDOW_LIMIT)
    : Solver(p), WINDOW_LIMIT(WINDOW_LIMIT), agents(p.getNum()), planned_paths(p.getNum()) {
    std::uniform_real_distribution distr;
    for (int i = 0; i < p.getNum(); ++i) {
        agents[i].id = i;
        planned_paths[i].emplace_back(p.getStart(i));
        agents[i].target = p.getTarget(i);
        agents[i].elapsed = 0;
        agents[i].l = 0;
        agents[i].init_d = p.getG().getDist(planned_paths[i][0], agents[i].target);  // Strange thing
        agents[i].tie_breaker = distr((this->p).gn);
    }
}

bool WinPIBT::checkValidPath(int ai, const Path& path, int t_from, int t_max) {
    for (int j = 1; j < path.size(); ++j) {
        DirectedNode v1 = path[j - 1];
        DirectedNode v2 = path[j];
        for (int i = 0; i < agents.size(); ++i) {
            if (i == ai)
                continue;
            if (planned_paths[i].size() <= j + t_from)
                continue;
            if (planned_paths[i][j + t_from].n == v2.n) {
                return false;
            }
            if (v1.n == planned_paths[i][j + t_from].n && v2.n == planned_paths[i][j + t_from - 1].n) {
                return false;
            }
        }
    }
    return true;
}

Path WinPIBT::getPath(int ai, int t_from, int t_max, bool please_move = false, Node banned = Node()) {
    if (t_from >= t_max || agents[ai].l >= t_max) {
        std::cerr << "Incorrect call of getPath with arguments: ai=" << ai << "; t_from=" << t_from
                  << "; t_max=" << t_max << std::endl;
        std::exit(EXIT_FAILURE);
    }
    DirectedNode start = planned_paths[ai][t_from];
    struct AstarNode {
        DirectedNode d;
        int time;  // time = g
        int f;
    };
    auto compare = [](AstarNode a, AstarNode b) {
        if (a.f != b.f)
            return a.f > b.f;
        // g-value
        if (a.time != b.time)
            return a.time < b.time;
        return false;
    };
    std::priority_queue<AstarNode, std::vector<AstarNode>, decltype(compare)> OPEN(compare);
    std::map<int, int> dist;
    // initial node
    Node target = agents[ai].target;
    if (please_move) {
        target = p.getG().getRandomNode();
    }
    OPEN.push(AstarNode{start, t_from, start.dist(target)});
    bool invalid = true;
    AstarNode last;
    Path path;

    while (!OPEN.empty()) {
        auto current_node = OPEN.top();
        OPEN.pop();
        if (current_node.d == banned) continue;
        if (dist.contains(p.getG().getID(current_node.d)))
            continue;
        dist[p.getG().getID(current_node.d)] = current_node.time;
        if (current_node.time >= t_max) {
            invalid = false;
            last = current_node;
            break;
        }
        auto tmpPath = p.getG().getPath(current_node.d, target, {banned});
        if (!tmpPath.empty()) {
            while (current_node.time + tmpPath.size() - 1 < t_max)
                tmpPath.push_back(tmpPath.back());
            while (current_node.time + tmpPath.size() - 1 > t_max)
                tmpPath.pop_back();
            if (checkValidPath(ai, tmpPath, current_node.time, t_max)) {
                path = tmpPath;
                std::reverse(path.begin(), path.end());
                path.pop_back();
                last = current_node;
                invalid = false;
                break;
            }
        }
        auto neighbours = p.getG().getNeighbours(current_node.d);
        std::shuffle(neighbours.begin(), neighbours.end(), p.gn);
        if (!please_move) {
            neighbours.emplace_back(0, current_node.d);
        }
        for (auto [w, u] : neighbours) {
            int g_cost = current_node.time + w;
            if (dist.contains(p.getG().getID(u)))
                continue;
            Path tmpPath = {current_node.d, u};
            if (!checkValidPath(ai, tmpPath, current_node.time, t_max))
                continue;
            OPEN.emplace(u, g_cost, g_cost + u.dist(target));
        }
    }
    if (invalid)
        return {};

    path.emplace_back(last.d);
    auto n = last.d;
    int cur_time = last.time;
    for (int abc = t_from; abc <= last.time; ++abc) {
        for (auto [dm, m] : p.getG().getInNeighbours(n)) {
            if (dist.contains(p.getG().getID(m)) && dist[p.getG().getID(m)] == cur_time - 1) {
                n = m;
                --cur_time;
                path.push_back(n);
                break;
            }
        }
    }
    std::reverse(path.begin(), path.end());
    return path;
}

int WinPIBT::getPosessingAgent(Node n, int time) {
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i].l <= time && planned_paths[i][agents[i].l] == n)
            return i;
    }
    return -1;
}

bool WinPIBT::isVertexConflict(Node n, int time, int ai) {
    for (int i = 0; i < agents.size(); ++i) {
        if (i == ai)
            continue;
        if (planned_paths[i].size() <= time && planned_paths[i].back() == n)
            return true;
        if (planned_paths[i].size() > time && planned_paths[i][time] == n) {
            return true;
        }
    }
    return false;
}

int WinPIBT::getTmax() {
    size_t ans = 0;
    for (int i = 0; i < agents.size(); ++i) {
        ans = std::max(ans, planned_paths[i].size());
    }
    return ans;
}

bool WinPIBT::canStay(int ai) {
    for (int i = 0; i < agents.size(); ++i) {
        if (i == ai)
            continue;
        for (int j = agents[ai].l; j <= agents[i].l; ++j) {
            if (planned_paths[i][j].n == planned_paths[ai][agents[ai].l].n) {
                return false;
            }
        }
    }
    return true;
}

void WinPIBT::stay(int ai, int t_to) {
    int previous_l = agents[ai].l;
    while (agents[ai].l > 0 && !canStay(ai)) {
        --agents[ai].l;
    }
    if (!canStay(ai)) {
        agents[ai].l = previous_l;
        for (int i = 0; i < agents.size(); ++i) {
            agents[i].l = std::min(agents[ai].l, agents[i].l);
            planned_paths[i].resize(std::min(agents[ai].l + 1, static_cast<int>(planned_paths[i].size())));
        }
        int mx_elapsed = 0;
        for (int i = 0; i < agents.size(); ++i) {
            mx_elapsed = std::max(mx_elapsed, agents[i].elapsed);
        }
        agents[ai].elapsed = mx_elapsed + 1;
        stop_till_new_iteration = true;
        return;
    }
    planned_paths[ai].resize(agents[ai].l + 1);
    for (int t = agents[ai].l + 1; t <= t_to; ++t) {
        planned_paths[ai].push_back(planned_paths[ai].back());
    }
    agents[ai].l = t_to;
}

bool WinPIBT::winPIBT(int ai, int t_to, bool please_move = false) {
    if (agents[ai].l >= t_to)
        return true;
    // auto compare = [&](int a, int b) {
    //     if (agents[a].elapsed != agents[b].elapsed)
    //         return agents[a].elapsed > agents[b].elapsed;
    //     // use initial distance
    //     if (agents[a].init_d != agents[b].init_d)
    //         return agents[a].init_d > agents[b].init_d;
    //     return agents[a].tie_breaker > agents[b].tie_breaker;
    // };
    int t_max = std::max(t_to, getTmax());
    int t = agents[ai].l + 1;
    bool planned = false;
    if (!please_move && planned_paths[ai][agents[ai].l] == agents[ai].target) {
        Path newPath(t_max - agents[ai].l + 1, planned_paths[ai].back());
        if (checkValidPath(ai, newPath, agents[ai].l, t_max)) {
            planned_paths[ai].resize(agents[ai].l + 1);
            for (int j = agents[ai].l + 1; j <= t_to; ++j) {
                planned_paths[ai].push_back(planned_paths[ai].back());
            }
            planned = true;
        }
    }
    if (!planned) {
        Path wanted_path = getPath(ai, agents[ai].l, t_max, please_move && planned_paths[ai][agents[ai].l] == agents[ai].target);
        if (wanted_path.empty()) {
            stay(ai, t_to);
            return false;
        }
        planned_paths[ai].resize(t);
        for (int j = t; j <= t_to; ++j) {
            planned_paths[ai].push_back(wanted_path[j - agents[ai].l]);
            if (!please_move && wanted_path[j - agents[ai].l] == agents[ai].target) {
                Path newPath(t_max - j + 1, wanted_path[j - agents[ai].l]);
                if (checkValidPath(ai, newPath, j, t_max)) {
                    ++j;
                    for (; j <= t_to; ++j) {
                        planned_paths[ai].push_back(planned_paths[ai].back());
                    }
                }
            }
        }
    }
    while (t <= t_to) {
        DirectedNode v = planned_paths[ai][t];
        int aj = getPosessingAgent(v.n, t - 1);
        while (aj != -1 && aj != ai) {
            // if (compare(aj, ai))
            //     break;
            winPIBT(aj, t_max, true);
            if (stop_till_new_iteration) {
                return false;
            }
            aj = getPosessingAgent(v.n, t - 1);
        }
        aj = getPosessingAgent(v.n, t);
        if ((aj != -1 && aj != ai) || isVertexConflict(v.n, t, ai)) {
            std::vector<int> good_nodes {-1, ai};
            if ((aj != -1 && !winPIBT(aj, t_max, true)) || std::find(good_nodes.begin(), good_nodes.end(), getPosessingAgent(v.n, t)) == good_nodes.end()  ||
                isVertexConflict(v.n, t, ai)) {
                if (stop_till_new_iteration) {
                    return false;
                }
                bool staying = true;
                for (int j = std::max(0, t - 6); j < t; ++j) {
                    if (planned_paths[ai][j].n != planned_paths[ai][t - 1].n) {
                        staying = false;
                        break;
                    }
                }
                for (int j = t; j <= t_to; ++j) {
                    planned_paths[ai].pop_back();
                }
                agents[ai].l = t - 1;
                Node banned;
                if (staying) {
                    banned = v.n;
                    Path newPath = getPath(ai, t - 1, t_max, false, banned);
                    if (newPath.empty()) {
                        stay(ai, t_to);
                        return false;
                    } else {
                        for (int j = t; j <= t_to; ++j) {
                            planned_paths[ai].push_back(newPath[j - t + 1]);
                        }
                    }
                    continue;
                }
                stay(ai, t_to);
                return false;
            }
        }
        agents[ai].l = t;
        ++t;
    }
    return true;
}

bool WinPIBT::checkSolved() {
    for (int i = 0; i < agents.size(); ++i) {
        if (planned_paths[i].back() != agents[i].target) {
            return false;
        }
    }
    return true;
}

Paths WinPIBT::solve() {
    auto compare = [&](int a, int b) {
        if (agents[a].elapsed != agents[b].elapsed)
            return agents[a].elapsed > agents[b].elapsed;
        // use initial distance
        if (agents[a].init_d != agents[b].init_d)
            return agents[a].init_d > agents[b].init_d;
        return agents[a].tie_breaker > agents[b].tie_breaker;
    };
    int t_sup = 0;
    int t = 0;
    std::vector<int> l_snapshot(agents.size());
    while (!checkSolved()) {
        stop_till_new_iteration = false;
        for (int i = 0; i < agents.size(); ++i) {
            for (int j = l_snapshot[i] + 1; j <= agents[i].l; ++j) {
                if (planned_paths[i][j] == agents[i].target) {
                    agents[i].elapsed = -1;
                    break;
                }
            }
            agents[i].elapsed++;
            l_snapshot[i] = agents[i].l;
        }
        std::vector<int> agent_inds(p.getNum());
        std::iota(agent_inds.begin(), agent_inds.end(), 0);
        std::sort(agent_inds.begin(), agent_inds.end(), compare);
        for (int j = 0; j < agents.size(); ++j) {
            int ai = agent_inds[j];
            if (agents[ai].l <= t) {
                if (j == 0) {
                    winPIBT(ai, t + WINDOW_LIMIT);
                    if (stop_till_new_iteration) break;
                } else {
                    winPIBT(ai, std::min(t + WINDOW_LIMIT, t_sup));
                    if (stop_till_new_iteration) break;
                }
            }
            if (j == 0) {
                t_sup = agents[ai].l;
            } else {
                t_sup = std::min(t_sup, agents[ai].l);
            }
        }
        ++t;
        if (t >= 500) {
            break;
        }
    }
    for (int i = 0; i < agents.size(); ++i) {
        while (planned_paths[i].size() > 1000) {
            planned_paths[i].pop_back();
        }
    }
    int t_max = getTmax();
    for (int i = 0; i < agents.size(); ++i) {
        while (planned_paths[i].size() != t_max) {
            planned_paths[i].push_back(planned_paths[i].back());
        }
    }
    return planned_paths;
}

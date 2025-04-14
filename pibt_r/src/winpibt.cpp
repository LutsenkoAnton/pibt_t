#include "../include/winpibt.hpp"
#include <algorithm>
#include <map>
#include <queue>
#include <set>

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

bool WinPIBT::checkValidPath(int ai, const Path& path, int t_from, int t_max, int owner_ai = -1) {
    // if (ai == 11) {
    //     std::cerr << owner_ai << std::endl;
    // }
    for (int j = 1; j < path.size(); ++j) {
        DirectedNode v1 = path[j - 1];
        DirectedNode v2 = path[j];
        for (int i = 0; i < agents.size(); ++i) {
            if (i == ai)
                continue;
            int k = agents[i].l;
            if (i != owner_ai) {
                for (int t = j + t_from + 1; t <= t_max; ++t) {
                    if (k < t) {
                        break;
                    }
                    // if (ai == 0 && i == 3 && t_max == 302) {
                    //     std::cerr << "Checking " << planned_paths[i][t].n << ' ' << v2 << std::endl;
                    // }
                    if (planned_paths[i][t].n == v2.n) {
                        // std::cerr << "REASON FUTURE " << ai << ' ' << i << ' ' << t << ' ' << v2.n << ' ' << planned_paths[ai][agents[ai].l] << std::endl;
                        return false;
                    }
                }
            }
            // if (ai == 0 && i == 3 && t_max == 302) {
            //     std::cerr << "Checking size " << planned_paths[i].size() << ' ' << j + t_from << std::endl;
            // }
            if (planned_paths[i].size() <= j + t_from)
                continue;
            // if (ai == 0 && i == 3 && t_max == 302) {
            //     std::cerr << "Checking " << planned_paths[i][j + t_from] << ' ' << v2 << std::endl;
            // }
            if (planned_paths[i][j + t_from].n == v2.n) {
                // std::cerr << "REASON VERTEX CONFLICT" << std::endl;
                return false;
            }
            if (v1.n == planned_paths[i][j + t_from].n && v2.n == planned_paths[i][j + t_from - 1].n) {
                // std::cerr << "REASON EDGE CONFLICT" << std::endl;
                return false;
            }
        }
    }
    return true;
}

Path WinPIBT::getPath(int ai, int t_from, int t_max, Node banned = Node(),int owner_ai = -1) {
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
    OPEN.push(AstarNode{start, t_from, start.dist(agents[ai].target)});
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
        auto tmpPath = p.getG().getPath(current_node.d, agents[ai].target, {banned});
        if (!tmpPath.empty()) {
            while (current_node.time + tmpPath.size() - 1 < t_max)
                tmpPath.push_back(tmpPath.back());
            while (current_node.time + tmpPath.size() - 1 > t_max)
                tmpPath.pop_back();
            if (checkValidPath(ai, tmpPath, current_node.time, t_max, owner_ai)) {
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
        for (auto [w, u] : neighbours) {
            int g_cost = current_node.time + w;
            if (dist.contains(p.getG().getID(u)))
                continue;
            Path tmpPath = {current_node.d, u};
            if (!checkValidPath(ai, tmpPath, current_node.time, t_max, owner_ai))
                continue;
            OPEN.emplace(u, g_cost, g_cost + u.dist(agents[ai].target));
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
        if (agents[i].l < time && planned_paths[i].back() == n)
            return i;
    }
    return -1;
}

int WinPIBT::getTmax() {
    size_t ans = 0;
    for (int i = 0; i < agents.size(); ++i) {
        ans = std::max(ans, planned_paths[i].size());
    }
    return ans;
}

bool WinPIBT::winPIBT(int ai, int t_to, bool please_move = false) {
    if (agents[ai].l >= t_to)
        return true;
    int t_max = std::max(t_to, getTmax());
    int t = agents[ai].l + 1;
    bool planned = false;
    int owner_ai = -1;
    for (int i = 0; i < agents.size(); ++i) {
        if (i == ai) continue;
        if (owner_ai != -1) break;
        for (int j = agents[ai].l; j <= std::min(static_cast<int>(planned_paths[i].size()) - 1, t_to); ++j) {
            if (planned_paths[i][j].n == planned_paths[ai][agents[ai].l].n) {
                owner_ai = i;
                break;
            }
        }
    }
    // if (owner_ai != -1) {
    //     std::cerr << ai << ' ' << owner_ai << std::endl;
    // }
    if (!please_move && planned_paths[ai].back() == agents[ai].target) {
        Path newPath(t_max - agents[ai].l + 1, planned_paths[ai].back());
        if (checkValidPath(ai, newPath, agents[ai].l, t_max, owner_ai)) {
            for (int j = agents[ai].l + 1; j <= t_to; ++j) {
                planned_paths[ai].push_back(planned_paths[ai].back());
            }
            planned = true;
        }
    }
    if (!planned) {
        Path wanted_path = getPath(ai, agents[ai].l, t_max, Node(), owner_ai);
        // if (ai == 11) {
        //     std::cerr << "WANTED PATH " << ai << ' ' << t_to << ": ";
        //     for (auto d: wanted_path) {
        //         std::cerr << d << ' ';
        //     }
        //     std::cerr << std::endl;
        // }
        if (wanted_path.empty()) {
            for (int t = agents[ai].l + 1; t <= t_to; ++t) {
                planned_paths[ai].emplace_back(planned_paths[ai].back());
            }
            agents[ai].l = t_to;
            return false;
        }

        for (int j = t; j <= t_to; ++j) {
            planned_paths[ai].push_back(wanted_path[j - agents[ai].l]);
            if (!please_move && wanted_path[j - agents[ai].l] == agents[ai].target) {
                Path newPath(t_max - j + 1, wanted_path[j - agents[ai].l]);
                if (checkValidPath(ai, newPath, j, t_max, owner_ai)) {
                    ++j;
                    for (; j <= t_to; ++j) {
                        planned_paths[ai].push_back(planned_paths[ai].back());
                    }
                }
            }
        }
        // if (ai == 7 || ai == 17) {
        // // if (ai == 0) {
        //     std::cerr << "WANTED PATH " << ai << ' ' << t_to << ": ";
        //     for (auto d: wanted_path) {
        //         std::cerr << d << ' ';
        //     }
        //     std::cerr << std::endl;
        // }
    }
    while (t <= t_to) {
        DirectedNode v = planned_paths[ai][t];
        agents[ai].l = t;
        int aj = getPosessingAgent(v.n, t - 1);
        while (aj != -1 && aj != ai) {
            winPIBT(aj, agents[aj].l + 1, true);
            aj = getPosessingAgent(v.n, t - 1);
        }
        aj = getPosessingAgent(v.n, t);
        if (aj != -1 && aj != ai) {
            if (!winPIBT(aj, t, true) || getPosessingAgent(v.n, t) != -1) {
                bool staying = true;
                for (int j = std::max(0, t - 6); j < t; ++j) {
                    // if (ai == 2) {
                    //     std::cerr << planned_paths[ai][j].n << ' ' << planned_paths[ai][t - 1].n << std::endl;
                    // }
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
                    // std::cerr << banned << std::endl;
                }
                Path newPath = getPath(ai, t - 1, t_max, banned, owner_ai);
                // if (ai == 2) {
                //     std::cerr << "REPLANING " << ai << ' ' << aj << ' ' << t_to << ": ";
                //     for (auto d: newPath) {
                //         std::cerr << d << std::endl;
                //     }
                // }
                if (newPath.empty()) {
                    for (int j = t; j <= t_to; ++j) {
                        planned_paths[ai].push_back(planned_paths[ai].back());
                    }
                    agents[ai].l = t_to;
                    return false;
                } else {
                    for (int j = t; j <= t_to; ++j) {
                        planned_paths[ai].push_back(newPath[j - t + 1]);
                    }
                }
                continue;
            }
        }
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
                } else {
                    winPIBT(ai, std::min(t + WINDOW_LIMIT, t_sup));
                }
            }
            if (j == 0) {
                t_sup = agents[ai].l;
            } else {
                t_sup = std::min(t_sup, agents[ai].l);
            }
        }
        ++t;
        // if (t % 100 == 0) std::cerr << "ITER " << t << std::endl;
        if (t >= 300) {
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

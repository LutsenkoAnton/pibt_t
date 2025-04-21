#include "../include/pibt.hpp"

#include <algorithm>

const std::string PIBT::SOLVER_NAME = "PIBT";

PIBT::PIBT(const Problem& p): Solver(p),
    agents(p.getNum()),
    occupied_now(p.getSize(), -1),
    occupied_next(p.getSize(), -1) {
    std::uniform_real_distribution distr;
    for (int i = 0; i < p.getNum(); ++i) {
        agents[i].id = i;
        agents[i].now = p.getStart(i);
        agents[i].target = p.getTarget(i);
        agents[i].next = DirectedNode();
        agents[i].elapsed = 0;
        agents[i].init_d = p.getG().getDist(agents[i].now, agents[i].target); // Strange thing
        agents[i].tie_breaker = distr((this -> p).gn);
        occupied_now[agents[i].now.n.id] = i;
    }
}

Paths PIBT::solve() {
    auto compare = [&](int a, int b) {
        if (agents[a].elapsed != agents[b].elapsed) return agents[a].elapsed > agents[b].elapsed;
        // use initial distance
        if (agents[a].init_d != agents[b].init_d) return agents[a].init_d > agents[b].init_d;
        return agents[a].tie_breaker > agents[b].tie_breaker;
    };
    Paths solution(p.getNum());
    for (int i = 0; i < p.getNum(); ++i) {
        solution[i].push_back(agents[i].now);
    }
    std::vector<int> agent_inds(p.getNum());
    std::iota(agent_inds.begin(), agent_inds.end(), 0);
    int cnt = 0;
    while (true) {
        std::sort(agent_inds.begin(), agent_inds.end(), compare);
        for (const auto& a : agent_inds) {
            if (agents[a].next == DirectedNode()) {
                prioriy_inheritance(a);
            }
        }
        bool to_finish = true;
        for (const auto& a: agent_inds) {
            occupied_now[agents[a].now.n.id] = -1;
            occupied_next[agents[a].next.n.id] = -1;
            occupied_now[agents[a].next.n.id] = a;
            solution[a].push_back(agents[a].next);
            if (agents[a].next == agents[a].target) {
                agents[a].elapsed = 0;
            } else {
                ++agents[a].elapsed;
                to_finish = false;
            }
            agents[a].now = agents[a].next;
            agents[a].next = DirectedNode();
        }
        bool found = false;
        for (int i = 0; i < occupied_next.size(); ++i) {
            if (occupied_next[i] != -1) {
                found = true;
            }
        }
        if (cnt++ >= 300) break;
        if (found) break;
        if (to_finish) {
            break;
        }
    }
    return solution;
}

bool PIBT::prioriy_inheritance(int ai, int aj) {
    auto compare = [&](const std::pair<int, DirectedNode>& v, const std::pair<int, DirectedNode>& u) {
        int d_v = p.getG().getDist(v.second, agents[ai].target) + v.first;
        int d_u = p.getG().getDist(u.second, agents[ai].target) + u.first;
        if (d_v != d_u) return d_v < d_u;
        if (occupied_now[v.second.n.id] != -1 && occupied_now[u.second.n.id] == -1)
            return false;
        if (occupied_now[v.second.n.id] == -1 && occupied_now[u.second.n.id] != -1)
            return true;
        return false;
    };

    auto neighbours = p.getG().getUndirectedNeighbours(agents[ai].now);
    neighbours.emplace_back(2, agents[ai].now);
    std::sort(neighbours.begin(), neighbours.end(), compare);
    for (const auto& [w, d]: neighbours) {
        if (w > 1 && agents[ai].now.d != d.d) {
            occupied_next[agents[ai].now.n.id] = ai;
            agents[ai].next = agents[ai].now;
            agents[ai].next.d = rotate(agents[ai].now.d, d.d)[0];
            return false;
        }
        if (occupied_next[d.n.id] != -1) continue;
        if (aj != -1 && d.n == agents[aj].now.n) continue;

        occupied_next[d.n.id] = ai;
        agents[ai].next = d;

        int ak = occupied_now[d.n.id];
        if (ak != -1 && agents[ak].next == DirectedNode()) {
            if (!prioriy_inheritance(ak, ai)) continue;
        } else if (ak != -1 && ak != ai && agents[ak].next.n == d.n) {
            continue;
        }
        return true;
    }
    occupied_next[agents[ai].now.n.id] = ai;
    agents[ai].next = agents[ai].now;
    return false;
}



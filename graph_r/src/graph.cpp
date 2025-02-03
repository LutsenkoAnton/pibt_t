#include <fstream>
#include <iostream>
#include <queue>
#include <regex>

#include "../include/graph.hpp"

void Graph::halt(const std::string& msg) {
    std::cout << "Graph error: " << msg << std::endl;
    std::exit(EXIT_FAILURE);
}

bool Graph::existNode(int x, int y) const {
    return 0 <= x && x < width && 0 <= y && y < height && exist[y][x]; 
}

std::vector<std::pair<int, DirectedNode>> Graph::getNeighbours(const DirectedNode& n) const {
    int x = n.n.x;
    int y = n.n.y;
    std::vector<std::pair<int, DirectedNode>> neighbours;
    for (int i = 0; i < 4; ++i) {
        Direction d = static_cast<Direction>(i);
        if (n.d - d != 1) continue;
        neighbours.emplace_back(n.d - d, DirectedNode(getID(x, y), x, y, d));
    }
    switch (n.d) {
        case X_PLUS:
            if (existNode(x + 1, y)) neighbours.emplace_back(1, DirectedNode(getID(x + 1, y), x + 1, y, n.d));
            break;
        case X_MINUS:
            if (existNode(x - 1, y)) neighbours.emplace_back(1, DirectedNode(getID(x - 1, y), x - 1, y, n.d));
            break;
        case Y_PLUS:
            if (existNode(x, y + 1)) neighbours.emplace_back(1, DirectedNode(getID(x, y + 1), x, y + 1, n.d));
            break;
        case Y_MINUS:
            if (existNode(x, y - 1)) neighbours.emplace_back(1, DirectedNode(getID(x, y - 1), x, y - 1, n.d));
            break;
    }
    return neighbours;
}

std::vector<std::pair<int, DirectedNode>> Graph::getUndirectedNeighbours(const DirectedNode& n) const {
    int x = n.n.x;
    int y = n.n.y;
    std::vector<std::pair<int, DirectedNode>> neighbours;
    if (existNode(x + 1, y)) {
        Direction d(X_PLUS);
        neighbours.emplace_back(n.d - d + 1, DirectedNode(getID(x + 1, y), x + 1, y, d));
    }
    if (existNode(x - 1, y)) {
        Direction d(X_MINUS);
        neighbours.emplace_back(n.d - d + 1, DirectedNode(getID(x - 1, y), x - 1, y, d));
    }
    if (existNode(x, y + 1)) {
        Direction d(Y_PLUS);
        neighbours.emplace_back(n.d - d + 1, DirectedNode(getID(x, y + 1), x, y + 1, d));
    }
    if (existNode(x, y - 1)) {
        Direction d(Y_MINUS);
        neighbours.emplace_back(n.d - d + 1, DirectedNode(getID(x, y - 1), x, y - 1, d));
    }
    return neighbours;
}

std::vector<std::pair<int, DirectedNode>> Graph::getInNeighbours(const DirectedNode& n) const {
    int x = n.n.x;
    int y = n.n.y;
    std::vector<std::pair<int, DirectedNode>> neighbours;
    for (int i = 0; i < 4; ++i) {
        Direction d = static_cast<Direction>(i);
        if (n.d - d != 1) continue;
        neighbours.emplace_back(n.d - d, DirectedNode(getID(x, y), x, y, d));
    }
    switch (n.d) {
        case X_PLUS:
            if (existNode(x - 1, y)) neighbours.emplace_back(1, DirectedNode(getID(x - 1, y), x - 1, y, n.d));
            break;
        case X_MINUS:
            if (existNode(x + 1, y)) neighbours.emplace_back(1, DirectedNode(getID(x + 1, y), x + 1, y, n.d));
            break;
        case Y_PLUS:
            if (existNode(x, y - 1)) neighbours.emplace_back(1, DirectedNode(getID(x, y - 1), x, y - 1, n.d));
            break;
        case Y_MINUS:
            if (existNode(x, y + 1)) neighbours.emplace_back(1, DirectedNode(getID(x, y + 1), x, y + 1, n.d));
            break;
    }
    // int x = n.n.x;
    // int y = n.n.y;
    // std::vector<std::pair<int, DirectedNode>> neighbours;
    // if (existNode(x + 1, y)) {
    //     Direction d(X_MINUS);
    //     neighbours.emplace_back(n.d - d + 1, DirectedNode(x + 1, y, d));
    // }
    // if (existNode(x - 1, y)) {
    //     Direction d(X_PLUS);
    //     neighbours.emplace_back(n.d - d + 1, DirectedNode(x - 1, y, d));
    // }
    // if (existNode(x, y + 1)) {
    //     Direction d(Y_MINUS);
    //     neighbours.emplace_back(n.d - d + 1, DirectedNode(x, y + 1, d));
    // }
    // if (existNode(x, y - 1)) {
    //     Direction d(Y_PLUS);
    //     neighbours.emplace_back(n.d - d + 1, DirectedNode(x, y - 1, d));
    // }
    return neighbours;
}

Graph::Graph(): gn(20072007) {}

Graph::Graph(const std::string& filename): gn(20072007) {
#ifdef _MAPDIR_
    std::ifstream file(_MAPDIR_ + filename);
#else
    std::ifstream file(filename);
#endif

    if (!file) halt("file " + filename + " is not found.");

    std::string line;
    std::smatch results;
    std::regex r_height = std::regex(R"(height\s(\d+))");
    std::regex r_width = std::regex(R"(width\s(\d+))");
    std::regex r_map = std::regex(R"(map)");

    // fundamental graph params
    while (getline(file, line)) {
        // for CRLF coding
        if (*(line.end() - 1) == 0x0d) line.pop_back();

        if (std::regex_match(line, results, r_height)) {
            height = std::stoi(results[1].str());
        }
        if (std::regex_match(line, results, r_width)) {
            width = std::stoi(results[1].str());
        }
        if (std::regex_match(line, results, r_map)) break;
    }
    if (!(width > 0 && height > 0)) halt("failed to load width/height.");

    // create nodes
    int y = 0;
    exist.assign(height, std::vector<bool>(width, false));
    while (getline(file, line)) {
        // for CRLF coding
        if (*(line.end() - 1) == 0x0d) line.pop_back();

        if (static_cast<int>(line.size()) != width) halt("map format is invalid");
        for (int x = 0; x < width; ++x) {
            char s = line[x];
            if (s == 'T' or s == '@') continue;  // object
            exist[y][x] = true;
        }
        ++y;
    }
    if (y != height) halt("map format is invalid");
    file.close();
}

int Graph::getID(int x, int y) const {
    return y * width + x;
}

int Graph::getID(int x, int y, const Direction& d) const {
    return getID(x, y) * 4 + static_cast<int>(d);
}

int Graph::getID(const DirectedNode& d) const {
    return d.n.id * 4 + static_cast<int>(d.d);
}

Node Graph::getNode(int id) const {
    return Node(id, id % width, id / width);
}

DirectedNode Graph::getDirectedNode(int id) const {
    return DirectedNode(getNode(id / 4), static_cast<Direction>(id % 4));
}

Path Graph::getPath(const DirectedNode& start, const Node& target, const std::vector<Node>& prohibited) {
    if (start == target) return {};
    using AstarNode = std::tuple<DirectedNode, int, int>;  // node, g, f
    auto compare = [&](AstarNode a, AstarNode b) {
        // f-value
        if (std::get<2>(a) != std::get<2>(b))
            return std::get<2>(a) > std::get<2>(b);
        // g-value
        if (std::get<1>(a) != std::get<1>(b))
            return std::get<1>(a) < std::get<1>(b);
        return false;
    };

    // OPEN and CLOSE list
    constexpr int NIL = -1;
    std::vector<int> dist(width * height * 4, NIL);
    std::priority_queue<AstarNode, std::vector<AstarNode>, decltype(compare)> OPEN(compare);
    // initial node
    OPEN.push(std::make_tuple(start, 0, start.dist(target)));
    // std::cout << "DIST" <<  start << ' ' << DirectedNode(target, X_PLUS) << ' ' << start.dist(target) << std::endl;

    DirectedNode directed_target;

    // main loop
    bool invalid = true;
    // std::cerr << "START SEARCH" << std::endl;
    while (!OPEN.empty()) {
        // minimum node
        auto [d, g, f] = OPEN.top();
        // std::cerr << d << ' ' << g << ' ' << f << std::endl;
        OPEN.pop();

        // check CLOSE list
        if (dist[getID(d)] != NIL) continue;
        dist[getID(d)] = g;

        // check goal condition
        if (d == target) {
            directed_target = d;
            // std::cerr << target.x << ' ' << target.y <<  ' ' << g << std::endl;
            invalid = false;
            break;
        }

        // expand
        auto neighbours = getNeighbours(d);
        // std::cerr << d << ' ' << neighbours.size() << std::endl;
        std::shuffle(neighbours.begin(), neighbours.end(), gn);  // randomize
        for (auto [w, u] : neighbours) {
            int g_cost = g + w;
            // std::cerr << w << ' ' << u << std::endl;
            // already searched?
            if (dist[getID(u)] != NIL) continue;
            if (std::find(prohibited.begin(), prohibited.end(), u.n) != prohibited.end()) continue;
            // check constraints
            OPEN.emplace(u, g_cost, g_cost + u.dist(target));
        }
    }

    if (invalid) return {};

    // success
    Path path = {directed_target};
    auto n = directed_target;
    while (n != start) {
        for (auto [dm, m] : getInNeighbours(n)) {
            if (dist[getID(m)] == dist[getID(n)] - 1) {
                n = m;
                path.push_back(n);
                break;
            }
        }
    }
    std::reverse(path.begin(), path.end());

    return path;
}

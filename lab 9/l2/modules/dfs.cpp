#include <iostream>
#include <vector>
#include <list>
#include <sstream>


std::list<std::vector<uint32_t>> dfs(const std::vector<std::vector<uint32_t>> &graph,
                                     std::vector<bool> nodes_cond,
                                     std::vector<uint32_t> curr_path,
                                     uint32_t curr_node,
                                     uint32_t target_node);

int main() {
    std::stringstream stream;
    stream.exceptions(std::ios::failbit);
    std::string data;

    std::vector<std::vector<uint32_t>> graph;
    uint32_t begin_node, current_node, end_node;

    // input data
    try {
        uint32_t capacity;

        // input begin and end nodes
        std::getline(std::cin, data);
        stream << data;

        stream >> begin_node >> current_node >> end_node;

        // clear stream
        stream.str("");
        stream.clear();

        // input capacity (num of nodes)
        std::getline(std::cin, data);
        stream << data;

        stream >> capacity;

        // reserve memory
        graph.reserve(capacity);

        std::vector<uint32_t> nodes;
        uint32_t node;

        for (size_t i = 0; i < capacity; i++) {
            // clear stream
            stream.str("");
            stream.clear();

            // clear nodes vector
            nodes.clear();

            std::getline(std::cin, data);
            stream << data;

            while (!stream.eof()) {
                stream >> node;
                nodes.push_back(node);
            }

            graph.push_back(nodes);
        }

    } catch (std::exception &e) {
        std::cout << "(dfs) Conversion error" << std::endl;
        return -1;
    }

    std::list<std::vector<uint32_t>> all_paths;
    std::vector<bool> nodes_cond(graph.size(), false);
    std::vector<uint32_t> path;


    nodes_cond[begin_node] = true;
    path.push_back(begin_node);
    all_paths = dfs(graph, nodes_cond, path, current_node, end_node);

    for (auto &nodes : all_paths) {
        for (size_t i = 0; i < nodes.size() - 1; i++) {
            std::cout << nodes[i] << " ";
        }
        std::cout << nodes[nodes.size() - 1] << std::endl;
    }

    return 0;
}


std::list<std::vector<uint32_t>> dfs(const std::vector<std::vector<uint32_t>> &graph,
                                     std::vector<bool> nodes_cond,
                                     std::vector<uint32_t> curr_path,
                                     uint32_t curr_node,
                                     uint32_t target_node) {
    curr_path.push_back(curr_node);

    if (curr_node == target_node) {
        return std::list<std::vector<uint32_t>>{curr_path};
    }

    nodes_cond[curr_node] = true;

    std::list<std::vector<uint32_t>> all_paths;

    for (const auto& node : graph[curr_node]) {
        if (!nodes_cond[node]) {
            auto paths = dfs(graph, nodes_cond, curr_path, node, target_node);

            all_paths.merge(paths);
        }
    }

    return all_paths;
}
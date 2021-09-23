#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>


using adj_list_t = std::vector<std::vector<uint32_t>>;
using path_list_t = std::list<std::vector<uint32_t>>;


void thread_find_paths(const adj_list_t &_graph,
                       path_list_t &_paths,
                       std::mutex &_mutex,
                       std::atomic<uint32_t> &_threads_limit,
                       uint32_t cur_node,
                       uint32_t begin_node,
                       uint32_t end_node);

adj_list_t generate_graph(uint32_t size);

void print_graph(const adj_list_t &graph);
void print_paths(const path_list_t &paths);

path_list_t dfs(const adj_list_t &graph,
                std::vector<bool> nodes_condition,
                std::vector<uint32_t> path,
                uint32_t curr_node,
                uint32_t target_node);


int main() {
    uint32_t begin_node = 1;
    uint32_t end_node = 8;
    uint32_t num_of_node = 9;
    std::atomic<uint32_t> num_threads_limit = 3;

    path_list_t paths;
    auto graph = generate_graph(num_of_node);

    std::vector<std::thread> threads;
    std::mutex mutex;


    for (const auto &node: graph[begin_node]) {
        threads.emplace_back(thread_find_paths,
                             std::cref(graph),
                             std::ref(paths),
                             std::ref(mutex),
                             std::ref(num_threads_limit),
                             node,
                             begin_node,
                             end_node);
    }

    for (auto &thread: threads) {
        thread.join();
    }

    print_graph(graph);
    print_paths(paths);

    return 0;
}


void thread_find_paths(const adj_list_t &_graph,
                       path_list_t &_paths,
                       std::mutex &_mutex,
                       std::atomic<uint32_t> &_threads_limit,
                       uint32_t cur_node,
                       uint32_t begin_node,
                       uint32_t end_node) {
    while (_threads_limit == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        {
            std::lock_guard<std::mutex> lg(_mutex);
            std::cout << std::this_thread::get_id() << " - sleep" << std::endl;
            std::cout << "Threads work: " << _threads_limit << std::endl << std::endl;
        }
    }
    _threads_limit--;

    std::vector<bool> nodes_condition(_graph.size(), false);
    std::vector<uint32_t> path;

    nodes_condition[begin_node] = true;
    path.push_back(begin_node);

    auto paths = dfs(_graph, nodes_condition, path, cur_node, end_node);

    {
        std::lock_guard<std::mutex> lg(_mutex);
        _paths.merge(paths);
    }

    _threads_limit++;
}


path_list_t dfs(const adj_list_t &graph,
                std::vector<bool> nodes_condition,
                std::vector<uint32_t> path,
                uint32_t curr_node,
                uint32_t target_node) {
    path.push_back(curr_node);

    if (curr_node == target_node) {
        return path_list_t{path};
    }

    nodes_condition[curr_node] = true;

    path_list_t all_paths;

    for (const auto& node : graph[curr_node]) {
        if (!nodes_condition[node]) {
            auto paths = dfs(graph, nodes_condition, path, node, target_node);

            all_paths.merge(paths);
        }
    }

    return all_paths;
}


adj_list_t generate_graph(uint32_t size) {
    adj_list_t adjacency_list;

    std::random_device rd;
    std::mt19937 mersenne(rd());

    for (uint32_t i = 0; i < size; i++) {
        std::vector<uint32_t> nodes;

        for (uint32_t j = 0; j < size; j++) {
            uint32_t node_id = mersenne() % size;

            if (node_id != i and std::count(nodes.rbegin(), nodes.rend(), node_id) == 0) {
                nodes.push_back(node_id);
            }
        }

        adjacency_list.push_back(nodes);
    }

    return adjacency_list;
}


void print_graph(const adj_list_t &graph) {
    uint32_t max_indent = 6;

    uint32_t max_index_indent = std::to_string(graph.size() - 1).size() + 1;

    if (max_index_indent > max_indent) {
        max_indent = max_index_indent;
    }

    std::cout << "Node" << std::string(max_indent - 4, ' ') << " " << "Nodes" << std::endl;

    for (uint32_t i = 0; i < graph.size(); i++) {
        std::cout << i << std::string(max_indent - std::to_string(i).size(), ' ') <<" ";

        for (const auto &node : graph[i]) {
            std::cout << node << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}


void print_paths(const path_list_t &paths) {
    if (paths.empty()) {
        std::cout << "Paths is empty :(" << std::endl << std::endl;
        return;
    }

    auto it = paths.rbegin();
    uint32_t begin_node = it->at(0);
    uint32_t end_node = it->at(it->size() - 1);

    std::cout << "All path: " << begin_node << " -> " << end_node << std::endl;
    std::cout << "Number of paths: " << paths.size() << std::endl;

    for (const auto& path : paths) {
        for (const auto &node : path) {
            if (node != end_node) {
                std::cout << node << " -> ";
            } else {
                std::cout << node;
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}


#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include <mutex>
#include <vector>
#include <list>

#include <boost/process.hpp>

namespace bp = boost::process;

void ThreadCompileProcess();
void ThreadRunProcessDFS(
        std::list<std::vector<uint32_t>> &container,
        const std::vector<std::vector<uint32_t>> &graph,
        std::mutex &mutex,
        uint32_t begin_node,
        uint32_t curr_node,
        uint32_t end_node
);

std::vector<std::vector<uint32_t>> GenGraph(size_t size);
void PrintGraph(const std::vector<std::vector<uint32_t>> &graph);
void PrintPaths(const std::list<std::vector<uint32_t>> &paths);


int main() {
    std::thread(ThreadCompileProcess).join();

    std::list<std::vector<uint32_t>> paths;

    std::vector<std::thread> thread_ids;
    std::mutex mutex;

    uint32_t begin_node = 2;
    uint32_t end_node = 5;
    uint32_t num_of_nodes = 8;

    auto graph = GenGraph(num_of_nodes);

    for (const auto &node : graph[begin_node]) {
        thread_ids.emplace_back(
                ThreadRunProcessDFS,
                std::ref(paths),
                std::cref(graph),
                std::ref(mutex),
                begin_node,
                node,
                end_node
        );
    }

    for (auto &thread_id : thread_ids) {
        thread_id.join();
    }

    PrintGraph(graph);
    PrintPaths(paths);

    return 0;
}


void ThreadCompileProcess() {
    bp::child (
        bp::search_path("clang++"),
        std::vector<std::string>{"modules/dfs.cpp", "-omodules/dfs", "-std=c++20"},
        bp::std_out > stdout, bp::std_err > stderr
    ).wait();
}


void ThreadRunProcessDFS(
        std::list<std::vector<uint32_t>> &container,
        const std::vector<std::vector<uint32_t>> &graph,
        std::mutex &mutex,
        uint32_t begin_node,
        uint32_t curr_node,
        uint32_t end_node
) {
    bp::ipstream istream;
    bp::opstream ostream;

    bp::child proc_dfs("modules/dfs", bp::std_out > istream, bp::std_in < ostream, bp::std_err > stderr);

    // sent nodes
    ostream << begin_node << " " << curr_node << " " << end_node << std::endl;

    // sent capacity
    ostream << graph.size() << std::endl;

    // sent graph
    for (const auto &nodes : graph) {
        for (size_t i = 0; i < nodes.size() - 1; i++) {
            ostream << nodes[i] << " ";
        }
        ostream << nodes[nodes.size() - 1] << std::endl;
    }

    std::list<std::vector<uint32_t>> temp_paths;
    std::vector<uint32_t> path;
    path.reserve(graph.size());

    std::stringstream stream;
    std::string line;

    stream.exceptions(std::ios::failbit);

    while (std::getline(istream, line)) {
        // clear stream
        stream.str("");
        stream.clear();

        // clear path
        path.clear();

        stream << line;
        try {
            uint32_t node;

            while (!stream.eof()) {
                stream >> node;
                path.push_back(node);
            }

        } catch (std::exception &e) {
            std::cout << "Conversion error" << std::endl;
            proc_dfs.terminate();
            return;
        }

        temp_paths.push_back(path);
    }

    proc_dfs.wait();

    {
        std::lock_guard<std::mutex> lg(mutex);
        container.merge(temp_paths);
    }
}


std::vector<std::vector<uint32_t>> GenGraph(size_t size) {
    std::vector<std::vector<uint32_t>> adjacency_list;
    adjacency_list.reserve(size);

    std::random_device rd;
    std::mt19937 mersenne(rd());

    std::vector<uint32_t> nodes;

    for (uint32_t i = 0; i < size; i++) {
        nodes.clear();

        for (uint32_t j = 0; j < size; j++) {
            uint32_t node_id = mersenne() % size;

            if (node_id != i and std::find(nodes.cbegin(), nodes.cend(), node_id) == nodes.cend()) {
                nodes.push_back(node_id);
            }
        }

        adjacency_list.push_back(nodes);
    }

    return adjacency_list;
}


void PrintGraph(const std::vector<std::vector<uint32_t>> &graph) {
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


void PrintPaths(const std::list<std::vector<uint32_t>> &paths) {
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
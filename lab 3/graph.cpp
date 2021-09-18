#include "graph.h"
#include "graph_algs.h"

#include <random>
#include <sstream>


namespace l4 {

    Graph::Graph(uint32_t _num_of_nodes) {
        this->num_of_nodes = _num_of_nodes;
    }


    void Graph::generate_graph() {
        std::random_device rd;
        std::mt19937 mersenne(rd());

        for (uint32_t i = 0; i < this->num_of_nodes; i++) {
            std::vector<uint32_t> nodes;

            for (uint32_t j = 0; j < this->num_of_nodes; j++) {
                uint32_t node_id = mersenne() % this->num_of_nodes;

                if (node_id != i and std::count(nodes.rbegin(), nodes.rend(), node_id) == 0) {
                    nodes.push_back(node_id);
                }
            }

            this->graph.push_back(nodes);
        }
    }


    void Graph::find_all_paths(uint32_t begin_node, uint32_t end_node) {
        std::vector<std::thread> threads;

        for (const auto &node: this->graph[begin_node]) {
            threads.emplace_back(fthread_find_paths,
                                 this->shared_from_this(),
                                 begin_node,
                                 node,
                                 end_node);
        }

        for (auto & thread : threads) {
            thread.join();
        }
    }


    void Graph::fthread_find_paths(const std::shared_ptr<Graph>& graph, uint32_t begin_node, uint32_t curr_node, uint32_t end_node) {
        std::vector<bool> nodes_condition(graph->num_of_nodes, false);
        std::vector<uint32_t> path;

        nodes_condition[begin_node] = true;
        path.push_back(begin_node);

        auto paths = dfs(graph->graph, nodes_condition, path, curr_node, end_node);

        graph->merge_paths(paths);
    }


    void Graph::merge_paths(path_list_t &paths) {
        std::lock_guard<std::mutex> lg(this->mutex);
        this->all_paths.merge(paths);
    }


    std::string Graph::print_paths() const {
        std::ostringstream out;

        uint32_t max_indent = 6;

        uint32_t max_index_indent = std::to_string(this->num_of_nodes - 1).size() + 1;

        if (max_index_indent > max_indent) {
            max_indent = max_index_indent;
        }

        out << "Node" << std::string(max_indent - 4, ' ') << " " << "Nodes" << std::endl;

        for (uint32_t i = 0; i < this->num_of_nodes; i++) {
            out << i << std::string(max_indent - std::to_string(i).size(), ' ') <<" ";

            for (const auto &node : this->graph[i]) {
                out << node << " ";
            }

            out << std::endl;
        }
        return out.str();
    }


    std::string Graph::print_graph() const {
        std::ostringstream out;

        if (this->all_paths.empty()) {
            out << "Paths is empty :(" << std::endl;
            return out.str();
        }

        auto it = this->all_paths.cbegin();
        uint32_t begin_node = it->at(0);
        uint32_t end_node = it->at(it->size() - 1);

        out << "All path: " << begin_node << " -> " << end_node << std::endl;
        out << "Number of paths: " << this->all_paths.size() << std::endl;

        for (const auto& path : this->all_paths) {
            for (const auto &node : path) {
                if (node != end_node) {
                    out << node << " -> ";
                } else {
                    out << node;
                }
            }
            out << std::endl;
        }

        return out.str();
    }


    std::ostream &operator<<(std::ostream &out, const Graph &graph) {
        out << graph.print_graph() << std::endl;
        out << graph.print_paths() << std::endl;
        return out;
    }


} // namespace l4
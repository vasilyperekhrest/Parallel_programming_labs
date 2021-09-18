#ifndef LAB_3_GRAPH_H
#define LAB_3_GRAPH_H

#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <memory>

#include "graph_types.h"


namespace l4 {

class Graph : public std::enable_shared_from_this<Graph>{
    private:
        std::mutex mutex;

        path_list_t all_paths;
        adj_list_t graph;
        uint32_t num_of_nodes;


        static void fthread_find_paths(const std::shared_ptr<Graph>& graph, uint32_t begin_node, uint32_t curr_node, uint32_t end_node);

    public:
        explicit Graph(uint32_t _num_of_nodes);

        void generate_graph();
        void find_all_paths(uint32_t begin_node, uint32_t end_node);
        void merge_paths(path_list_t &paths);

        std::string print_paths() const;
        std::string print_graph() const;
        friend std::ostream& operator<< (std::ostream &out, const Graph &graph);

    };

} // namespace l4


#endif //LAB_3_GRAPH_H

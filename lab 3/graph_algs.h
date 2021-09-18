#ifndef LAB_3_GRAPH_ALGS_H
#define LAB_3_GRAPH_ALGS_H

#include "graph.h"


namespace l4 {

    path_list_t dfs(const adj_list_t &graph,
                    std::vector<bool> nodes_condition,
                    std::vector<uint32_t> path,
                    uint32_t curr_node,
                    uint32_t target_node);

} // namespace l4

#endif //LAB_3_GRAPH_ALGS_H

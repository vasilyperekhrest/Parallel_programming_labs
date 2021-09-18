#include "graph_algs.h"


namespace l4 {

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


} // namespace l4
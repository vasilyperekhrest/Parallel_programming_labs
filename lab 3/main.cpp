#include <iostream>
#include <memory>

#include "graph.h"


int main() {
    uint32_t num_of_nodes = 5;
    std::shared_ptr<l4::Graph> graph(new l4::Graph(num_of_nodes));

    graph->generate_graph();
    std::cout << *graph << std::endl;

    graph->find_all_paths(1, 4);
    std::cout << *graph;

    return 0;
}

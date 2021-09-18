#ifndef LAB_3_GRAPH_TYPES_H
#define LAB_3_GRAPH_TYPES_H

#include <vector>
#include <list>


namespace l4 {

    using adj_list_t = std::vector<std::vector<uint32_t>>;
    using path_list_t = std::list<std::vector<uint32_t>>;

}

#endif //LAB_3_GRAPH_TYPES_H

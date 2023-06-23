//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_CONFIG_H
#define DATABASE_BLOCKS_CONFIG_H

#include <cstdint>

namespace database_blocks {
    struct configs {
        static configs default_config(){
            return {.mem_tree_size = 4096};
        }
        configs()= default;
    public:
        int64_t mem_tree_size = 4096;
    };

}

#endif //DATABASE_BLOCKS_CONFIG_H

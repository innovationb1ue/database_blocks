//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_DB_H
#define DATABASE_BLOCKS_DB_H

#include "config.h"
#include "mem_tree_impl.h"
#include <vector>

namespace database_blocks {
    class db {
    public:
        db(configs config);

        ~db() = default;

    private:
        // config struct
        configs config;
        // more mem_tree means more possible memory occupation and better performance.
        std::vector<mem_tree> mem_trees;
    };
}


#endif //DATABASE_BLOCKS_DB_H

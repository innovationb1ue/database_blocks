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
        explicit db(configs config);

        explicit db();

        ~db() = default;

        void put(std::string& key, std::string & val);

        void new_memtree();


    private:
        // config struct
        configs config;
        // current storages.
        std::vector<mem_tree> trees;
        // immutable trees that are ready to get flushed to file.
        std::vector<mem_tree> immutable_trees;
    };
}


#endif //DATABASE_BLOCKS_DB_H

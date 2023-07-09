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

        template<typename T>
        void put(T &&key, T &&val) {
            // generate an uuid and make it operation id.
            // ensure only one tree receive the put request here.
            for (auto &tree: trees) {
                if (tree.is_immutable()) {
                    continue;
                }
                auto res = tree.put(std::forward<T>(key), std::forward<T>(val));
                if (res) {
                    return;
                }
            }
            // no tree is available ?
            // construct a new one.
            std::scoped_lock<std::mutex> l(tree_lock);
            auto new_tree = trees.emplace_back();
            new_tree.put(std::forward<T>(key), std::forward<T>(val));
        }

        void new_memtree();

        // current storages.
        std::vector<database_blocks::mem_tree> trees;
    private:
        // config struct (config should be light so do not worry copying it around. )
        configs config;
        // immutable trees that are ready to get flushed to file.
        std::vector<mem_tree> immutable_trees;
        // trees lock
        std::mutex tree_lock;
    };
}

template void database_blocks::db::put(std::string &&, std::string &&);

template void database_blocks::db::put(std::string &, std::string &);


#endif //DATABASE_BLOCKS_DB_H

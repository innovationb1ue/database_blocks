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

        // this gets called concurrently.
        template<typename T>
        requires std::is_same_v<std::string &, T &>
        void put(T &&key, T &&val) {
            // generate an uuid and make it operation id.
            // ensure only one tree receive the put request here.
            if (tree.is_immutable()) {

            }
            auto res = tree.put(std::forward<T>(key), std::forward<T>(val));
            if (res) {
                return;
            }
            // no tree is available ?
            // construct a new one.
            std::scoped_lock<std::mutex> l(tree_lock);
        }

        // swap mem tree with the new one.
        // block until swap succeed.
        void swap();

        std::string get(std::string key, std::string val) {
            return {"default val"};
        }

        // current storages.
        mem_tree tree;
        // trees waiting to be flushed.
        std::vector<mem_tree> immut_trees;
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

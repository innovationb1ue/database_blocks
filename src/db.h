//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_DB_H
#define DATABASE_BLOCKS_DB_H

#include "config.h"
#include "mem_tree_impl.h"
#include <vector>
#include <thread>

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
            // fire and forget the swap task.
            // Check swap thread
            if (tree->is_immutable() && !swap_thread.joinable()) {
                swap_thread = std::thread(&db::swap_mem_tree, this);
                swap_thread.join();
                //todo: optimize this to run concurrently.
            }
            // put data
            auto res = tree->put(std::forward<T>(key), std::forward<T>(val));
            if (res) {
                return;
            }
        }

        // swap_mem_tree mem tree with the new one.
        // block until swap_mem_tree succeed.
        void swap_mem_tree();

        std::string get(const std::string &key) {
            return {"default val"};
        }

        // current storages.
        std::shared_ptr<mem_tree> tree;
        // trees waiting to be flushed.
        std::vector<std::shared_ptr<mem_tree>> immutable_trees;
    private:
        // config struct (config should be light so do not worry copying it around. )
        configs config;
        // trees lock
        std::mutex tree_lock;
        std::thread swap_thread;
    };
}

template void database_blocks::db::put(std::string &&, std::string &&);

template void database_blocks::db::put(std::string &, std::string &);


#endif //DATABASE_BLOCKS_DB_H

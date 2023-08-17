//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_DB_H
#define DATABASE_BLOCKS_DB_H

#include "config.h"
#include "mem_tree_impl.h"
#include <ranges>
#include <vector>
#include <thread>
#include "unordered_set"

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

        std::optional<std::string> get(const std::string &key) {
            // Search in the current tree first
            auto currentTreeValue = tree->get(key);
            if (currentTreeValue.has_value()) {
                return currentTreeValue.value();
            }

            // If not found in the current tree, search in the immutable trees
            for (auto &immutable_tree: std::ranges::reverse_view(immutable_trees)) {
                auto immutableTreeValue = immutable_tree->get(key);
                if (immutableTreeValue.has_value()) {
                    return immutableTreeValue.value();
                }
            }

            // If the key is not found in both the current tree and immutable trees, try loading from a specific path
            std::string loadedValue;

            auto path = config.db_store_path;

            if (!path.empty() && std::filesystem::is_directory(path)) {
                // Use an unordered_set as a simple Bloom filter
                std::unordered_set < std::string > bloomFilter;
                bloomFilter.insert(key);

                for (const auto &entry: std::filesystem::directory_iterator(path)) {
                    if (!entry.is_regular_file()) {
                        continue;
                    }

                    std::string filePath = entry.path().string();
                    std::string fileName = entry.path().filename().string();

                    // Check if the file name matches the bloom filter
                    if (bloomFilter.find(fileName) == bloomFilter.end()) {
                        continue;
                    }

                    // Load the data from the file and create a new mem_tree instance
                    std::ifstream file(filePath);
                    if (file) {
                        std::stringstream buffer;
                        buffer << file.rdbuf();
                        std::string fileData = buffer.str();

                        mem_tree loadedTree;
                        loadedTree.deserialize(fileData, fileData.size());

                        // Search for the key in the loadedTree
                        auto loadedTreeValue = loadedTree.get(key);
                        if (loadedTreeValue.has_value()) {
                            loadedValue = loadedTreeValue.value();
                            break; // Found the value, no need to check other files
                        }
                    }
                }
            }

            // If the value is found in the loaded tree, return it
            if (!loadedValue.empty()) {
                return loadedValue;
            }

            //todo: if not in current and immutable trees. search persistence file.
            return std::nullopt;
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

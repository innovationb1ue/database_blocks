//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_CONFIG_H
#define DATABASE_BLOCKS_CONFIG_H

#include <cstdint>
#include <string>
#include <filesystem>
#include <utility>

namespace database_blocks {
    struct configs {
        configs(int64_t i, std::filesystem::path p) : mem_tree_size(i), db_store_path(std::move(p)) {

        };

        configs(){
            this->mem_tree_size = 4096;
            this->db_store_path = std::filesystem::current_path();
        }

        static configs default_config() {
            return {4096, std::filesystem::current_path()};
        }

        configs(configs&& other) noexcept {
            this->db_store_path = std::move(other.db_store_path);
            this->mem_tree_size = other.mem_tree_size;
        }

        configs(configs const &other){
            this->mem_tree_size = other.mem_tree_size;
            this->db_store_path = other.db_store_path;
        }

        configs& operator= (configs const& other)= default;

    public:
        // mem tree kv_size_in_bytes in memory.
        int64_t mem_tree_size;
        // path to store db files. this should point to a directory where we can freely use.
        std::filesystem::path db_store_path;
    };

}

#endif //DATABASE_BLOCKS_CONFIG_H
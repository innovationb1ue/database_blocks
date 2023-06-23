//
// Created by 钟卓凡 on 23/06/2023.
//

#ifndef DATABASE_BLOCKS_CONFIG_H
#define DATABASE_BLOCKS_CONFIG_H

#include <cstdint>
#include <string>
#include <filesystem>

namespace database_blocks {
    struct configs {
        static configs default_config() {
            return {
                    .mem_tree_size = 4096,
                    .db_store_path = std::filesystem::current_path()
            };
        }

        configs() = default;

    public:
        // mem tree size in memory.
        int64_t mem_tree_size;
        // path to store db files. this should point to a directory where we can freely use.
        const std::filesystem::path db_store_path;
    };

}

#endif //DATABASE_BLOCKS_CONFIG_H

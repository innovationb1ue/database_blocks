//
// Created by 钟卓凡 on 23/06/2023.
//

#include "db.h"
#include "mem_tree_impl.h"

#include <utility>

#include "config.h"
#include <vector>
#include "spdlog/spdlog.h"

database_blocks::db::db(database_blocks::configs config): config(std::move(config)) {
}

database_blocks::db::db() {
    this->trees = std::vector<mem_tree>();
}

void database_blocks::db::put(std::string& key, std::string & val) {
    // generate an uuid and make it operation id.
    // ensure only one tree receive the put request here.
    spdlog::info("hello");
    for (auto &tree: trees) {
        if (tree.is_immutable()) {
            continue;
        }
        auto res = tree.put(key, val);
        if (res) {
            return;
        }
    }
}
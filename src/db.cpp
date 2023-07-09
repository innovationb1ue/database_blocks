//
// Created by 钟卓凡 on 23/06/2023.
//

#include "db.h"
#include "mem_tree_impl.h"
//#include "mem_tree_impl.cpp"

#include <utility>

#include "config.h"
#include <vector>
#include "spdlog/spdlog.h"

database_blocks::db::db(database_blocks::configs config) : config(config) {
    for (int i = 0; i < config.mem_tree_num; i++) {
        trees.emplace_back(config);
    }
}

database_blocks::db::db() {
    this->trees = std::vector<mem_tree>();
    // todo: make this initialize process
    trees.emplace_back(this->config);
}
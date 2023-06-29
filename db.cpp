//
// Created by 钟卓凡 on 23/06/2023.
//

#include "db.h"

#include <utility>

#include "config.h"

database_blocks::db::db(database_blocks::configs config): config(std::move(config)) {

}

database_blocks::db::db() {

}

void database_blocks::db::put(std::string& key, std::string & val) {
    // generate an uuid and make it operation id.
    for (auto & tree : trees){
        auto res = tree.put(key, val);
        if (res){
            return;
        }
    }
}


//
// Created by 钟卓凡 on 15/06/2023.
//
#include "library.h"
#include "string"

int main() {
    auto db = database_blocks::mem_tree();
    auto v1 = std::string("789");
    auto k1 = std::string("456");
    db.put(k1, v1);
    db.flush();
}

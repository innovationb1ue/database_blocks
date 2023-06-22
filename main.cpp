
//
// Created by 钟卓凡 on 15/06/2023.
//
#include "library.h"
#include "string"
#include "iostream"

int main() {
    auto db = database_blocks::mem_tree();
    int k = 0;
    int v = 0;
    while (!db.is_immutable()){
        auto k1 = std::to_string(k);
        auto v1 = std::to_string(v);
        db.put(k1, v1);
        k++;
        v++;
        std::cout << "put" << k1 << v1 << std::endl;
        std::cout << "size = " << db.size() << std::endl;
    }
    std::cout<<"is immutable" << std::endl;
    db.flush();
}

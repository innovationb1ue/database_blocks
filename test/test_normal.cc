//
// Created by 钟卓凡 on 22/06/2023.
//
#include <gtest/gtest.h>
#include "../mem_tree_impl.h"
#include "string"

TEST(BasicTest, BasicAssertions){
    auto db = database_blocks::mem_tree();
    int k = 0;
    int v = 0;
    while (!db.is_immutable()){
        auto k1 = std::to_string(k);
        auto v1 = std::to_string(v);
        db.put(k1, v1);
        k++;
        v++;
    }
    db.flush();
    ASSERT_EQ(db.is_immutable(), true);
}

TEST(BasicTest, DeleteElementAssertion){
    auto db = database_blocks::mem_tree();
    std::string k = "123456";
    std::string v = "456";
    db.put(k, v);
    db.remove(k);
    ASSERT_EQ(db.get(k).has_value(), false);
}


TEST(BasicLoadTest, LoadAssertion){
    auto db = database_blocks::mem_tree();
    int k = 0;
    int v = 0;
    while (!db.is_immutable()){
        auto k1 = std::to_string(k);
        auto v1 = std::to_string(v);
        db.put(k1, v1);
        k++;
        v++;
    }
    db.flush();

    auto db2 = database_blocks::mem_tree();
    db2.load();
    std::string k2 = "5";
    ASSERT_EQ(db2.get(k2), "5");

    std::string k3 = "8";
    ASSERT_EQ(db.get(k3), "8");
}
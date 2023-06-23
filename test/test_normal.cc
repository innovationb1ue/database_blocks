//
// Created by 钟卓凡 on 22/06/2023.
//
#include <gtest/gtest.h>
#include "mem_tree_impl.h"
#include <string>
#include <sstream>
#include <iostream>

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
    while (!db.is_immutable() && k < 1000){
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

TEST(BasicLoadTest, Int64Assertion){
    auto db = database_blocks::mem_tree();
    int k = 0;
    auto k1 = std::to_string(k);

    std::vector<int64_t> nums = {123456789, 987654321};
    std::ostringstream oss;
    for (auto num : nums) {
        oss << num << " ";
    }
    std::string str = oss.str();
    db.put(k1, str);
    db.flush();
    std::string new_key = "0";
    auto val = db.get(new_key);
    assert(val.has_value());
    std::istringstream iss(val.value());
    std::vector<int64_t> nums2;
    int64_t num2;
    while (iss >> num2) {
        nums2.push_back(num2);
    }
    for (auto num : nums2) {
        std::cout << "Deserialized number: " << num << std::endl;
    }
}

TEST(BasicLoadTest, WriteIntIntoValAssertion) {
    auto db = database_blocks::mem_tree();
    int k = 0;
    auto k1 = std::to_string(k);
    auto v = 64;
    auto v2 = 996;
    std::string v1;
    v1.reserve(sizeof v * 2);
    memcpy(v1.data(), reinterpret_cast<const char *>(&v), sizeof v);
    memcpy(v1.data() + sizeof v, reinterpret_cast<const char *>(&v2), sizeof v);
    db.put(k1, v1);
    auto res = db.get(k1);
    auto ptr = reinterpret_cast<int*>(res->data());
    ASSERT_EQ(64, *ptr);
    ptr ++;
    ASSERT_EQ(996, *ptr);
}
//
// Created by 钟卓凡 on 22/06/2023.
//
#include <gtest/gtest.h>
#include "mem_tree_impl.h"
#include <string>
#include <sstream>
#include <iostream>
#include "config.h"

class BasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto default_config = database_blocks::configs::default_config();
    }

    database_blocks::mem_tree tree{database_blocks::configs::default_config()};
};


TEST_F(BasicTest, BasicAssertions) {
    auto config = database_blocks::configs::default_config();
    int k = 0;
    int v = 0;
    while (!tree.is_immutable()) {
        auto k1 = std::to_string(k);
        auto v1 = std::to_string(v);
        tree.put(k1, v1);
        k++;
        v++;
    }
    tree.flush(config.db_store_path / "db.txt");
    ASSERT_EQ(tree.is_immutable(), true);
}

TEST_F(BasicTest, DeleteElementAssertion) {
    auto db = database_blocks::mem_tree();
    std::string k = "123456";
    std::string v = "456";
    db.put(k, v);
    db.remove(k);
    ASSERT_EQ(db.get(k).has_value(), false);
}


TEST_F(BasicTest, LoadAssertion) {
    int k = 0;
    int v = 0;
    while (!tree.is_immutable() && k < 1000) {
        auto k1 = std::to_string(k);
        auto v1 = std::to_string(v);
        tree.put(k1, v1);
        k++;
        v++;
    }
    tree.flush(tree.config.db_store_path / "db.txt");

    auto db2 = database_blocks::mem_tree();
    db2.load();
    std::string k2 = "5";
    ASSERT_EQ(db2.get(k2), "5");

    std::string k3 = "8";
    ASSERT_EQ(tree.get(k3), "8");
}

TEST_F(BasicTest, Int64Assertion) {
    int k = 0;
    auto k1 = std::to_string(k);

    std::vector<int64_t> nums = {123456789, 987654321};
    std::ostringstream oss;
    for (auto num: nums) {
        oss << num << " ";
    }
    std::string str = oss.str();
    tree.put(k1, str);
    tree.flush(tree.config.db_store_path / "db.txt");
    std::string new_key = "0";
    auto val = tree.get(new_key);
    assert(val.has_value());
    std::istringstream iss(val.value());
    std::vector<int64_t> nums2;
    int64_t num2;
    while (iss >> num2) {
        nums2.push_back(num2);
    }
    for (auto num: nums2) {
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
    auto ptr = reinterpret_cast<int *>(res->data());
    ASSERT_EQ(64, *ptr);
    ptr++;
    ASSERT_EQ(996, *ptr);
}


TEST(BasicLoadTest, CompareBytes) {
    auto db = database_blocks::mem_tree();

    std::string k1;
    k1.resize(4);
    int k = 123;
    memcpy(k1.data(), reinterpret_cast<const char *>(&k), sizeof k);

    auto v = 64;
    std::string v1;
    v1.reserve(sizeof v * 2);
    memcpy(v1.data(), reinterpret_cast<const char *>(&v), sizeof v);

    auto v2_i = 456;
    std::string v2;
    v2.reserve(sizeof v2_i * 2);
    memcpy(v2.data(), reinterpret_cast<const char *>(&v2_i), sizeof v2_i);

    db.put(k1, v1);
    db.put(k1, v2);
    auto res = db.get(k1);
    auto ptr = reinterpret_cast<int *>(res->data());
    ASSERT_EQ(v2_i, *ptr);

}

//
// Created by 钟卓凡 on 22/06/2023.
//
#include <gtest/gtest.h>
#include "src/mem_tree_impl.h"
#include <string>
#include <sstream>
#include <iostream>
#include "src/config.h"

class BasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        k1 = "123";
        v1 = "456789";
        k2 = "456789";
        v2 = "454444";
        k3 = "456";
        v3 = "888888";
        k4 = "567";
        v4 = "678";
        tree.put(k1, v1);
        tree.put(k2, v2);
        tree.put(k3, v3);
        tree.put(std::move(k4), std::move(v4));
    }

    void TearDown() override {
        tree.clear_wal();
    }

    std::string k1, v1, k2, v2, k3, v3, k4, v4;
    database_blocks::mem_tree tree;
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
    std::remove((config.db_store_path / "db.txt").c_str());
}

TEST_F(BasicTest, DeleteElementAssertion) {
    std::string k = "123456";
    std::string v = "456";
    tree.put(k, v);
    tree.remove(k);
    ASSERT_EQ(tree.get(k).has_value(), false);
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

    auto tree2 = database_blocks::mem_tree();
    tree2.load();
    std::string k2 = "5";
    ASSERT_EQ(tree2.get(k2), "5");

    std::string k3 = "8";
    ASSERT_EQ(tree.get(k3), "8");
    tree2.clear_wal();
    std::remove((tree.config.db_store_path / "db.txt").c_str());
}

TEST_F(BasicTest, LoadAssertion2) {
    std::string k1 = "123";
    std::string v1 = "456789";
    std::string k2 = "456789";
    std::string v2 = "454444";
    tree.put(k1, v1);
    tree.put(k2, v2);

    auto res = tree.get(k1);
    ASSERT_TRUE(res.has_value());
    ASSERT_TRUE(res == "456789");
    tree.flush();

    auto tree2 = database_blocks::mem_tree();
    tree2.load();
    ASSERT_EQ(tree2.get(k1), "456789");
    ASSERT_EQ(tree2.get(k2), "454444");
    tree2.clear_wal();

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

TEST_F(BasicTest, WriteIntAsValAssertion) {
    int k = 0;
    auto k1 = std::to_string(k);
    auto v = 64;
    auto v2 = 996;
    std::string v1;
    v1.resize(sizeof v + sizeof v2);
    memcpy(v1.data(), reinterpret_cast<const char *>(&v), sizeof v);
    memcpy(v1.data() + sizeof v, reinterpret_cast<const char *>(&v2), sizeof v2);
    tree.put(k1, v1);
    auto res = tree.get(k1);
    auto ptr = reinterpret_cast<int *>(res->data());
    ASSERT_EQ(64, *ptr);
    ptr++;
    ASSERT_EQ(996, *ptr);
}


TEST_F(BasicTest, CompareBytes) {
    std::string k1;
    k1.resize(4);
    int k = 123;
    memcpy(k1.data(), reinterpret_cast<const char *>(&k), sizeof k);

    auto v = 64;
    std::string v1;
    v1.resize(sizeof v * 2);
    memcpy(v1.data(), reinterpret_cast<const char *>(&v), sizeof v);

    auto v2_i = 456;
    std::string v2;
    v2.resize(sizeof v2_i * 2);
    memcpy(v2.data(), reinterpret_cast<const char *>(&v2_i), sizeof v2_i);
    tree.put(k1, v1);
    tree.put(k1, v2);
    auto res = tree.get(k1);
    auto ptr = reinterpret_cast<int *>(res->data());
    ASSERT_EQ(v2_i, *ptr);

}

TEST_F(BasicTest, TestSerialize) {
    std::string k1 = "123";
    std::string v1 = "456789";
    std::string k2 = "456789";
    std::string v2 = "454444";
    tree.put(k1, v1);
    tree.put(k2, v2);

    auto buf = tree.serialize();
    auto res = tree.get(k1);

    char *p = buf.data();
    // key kv_size_in_bytes
    auto ptr = reinterpret_cast<size_t *>(p);
    ASSERT_EQ(k1.size(), *ptr);
    p += sizeof(size_t);
    // val kv_size_in_bytes
    ptr = reinterpret_cast<size_t *>(p);
    ASSERT_EQ(k2.size(), *ptr);
    p += sizeof(size_t);
    // key 1
    std::string key1;
    key1.resize(k1.size());
    memcpy(key1.data(), p, k1.size());
    ASSERT_EQ(k1, key1);
}

TEST_F(BasicTest, TestDeserialize) {
    auto buf = tree.serialize();
    auto tree2 = database_blocks::mem_tree();
    tree2.deserialize(buf, buf.size());
    auto res = tree2.get(k1);
    tree2.clear_wal();
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res, v1);
}

// Define a fixture for testing
class LoadTest : public BasicTest {
};

// Test case for load function
TEST_F(LoadTest, LoadFromFile) {
    // Flush the tree to a file
    std::filesystem::path filePath = "test.db";
    tree.flush(filePath);

    // Clear the existing data in the tree
    tree.clear();

    // Load the data from the file
    tree.load(filePath);

    // Verify the loaded data
    EXPECT_EQ(tree.get(k1), v1);
    EXPECT_EQ(tree.get(k2), v2);
    EXPECT_EQ(tree.get(k3), v3);

    // Clean up the file
    std::filesystem::remove(filePath);
}

// Test case for load function with different keys
TEST_F(LoadTest, LoadFromFile_DifferentKeys) {
    // Insert additional key-value pairs
    std::string k4 = "987654";
    std::string v4 = "abcd";
    std::string k5 = "xyz";
    std::string v5 = "123";

    tree.put(k4, v4);
    tree.put(k5, v5);

    // Flush the tree to a file
    std::filesystem::path filePath = "test.db";
    tree.flush(filePath);

    // Clear the existing data in the tree
    tree.clear();

    // Load the data from the file
    tree.load(filePath);

    // Verify the loaded data
    EXPECT_EQ(tree.get(k1), v1);
    EXPECT_EQ(tree.get(k2), v2);
    EXPECT_EQ(tree.get(k3), v3);
    EXPECT_EQ(tree.get(k4), v4);
    EXPECT_EQ(tree.get(k5), v5);

    // Clean up the file
    std::filesystem::remove(filePath);
}


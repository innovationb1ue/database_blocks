//
// Created by 钟卓凡 on 23/06/2023.
//

#include <gtest/gtest.h>
#include "src/db.h"
#include "src/config.h"
#include "src/wal.h"

class DbTests : public ::testing::Test {
protected:
    void SetUp() override {
        db = std::make_shared<database_blocks::db>(database_blocks::configs::default_config());
        k1 = "123";
        v1 = "456789";
        k2 = "456789";
        v2 = "454444";
        k3 = "456";
        v3 = "888888";
        db->put(k1, v1);
        db->put(k2, v2);
        db->put(k3, v3);
    }

    void TearDown() override {
        db->tree->clear_wal();
    }

    std::string k1, v1, k2, v2, k3, v3;
    std::shared_ptr<database_blocks::db> db;
};

TEST_F(DbTests, EmptyBuildTest) {
    db->put(k1, v1);
}

TEST_F(DbTests, WALTEST) {
    auto test_wal = database_blocks::wal("./test_wal.txt");
    test_wal.write_to_wal("PUT", "123", "123456");
    auto m = test_wal.read_from_wal();
    auto it = m.begin();
    while (it != m.end()) {
        std::cout << "key = " << it->first << " " << "val = " << it->second << std::endl;
        it++;
    }
    ASSERT_EQ(m.find("123")->second, "123456");
    test_wal.remove();
}

TEST_F(DbTests, DbSwapTest) {
    db->swap_mem_tree();
}

TEST_F(DbTests, PutAndSwapTest) {
    // Perform massive put operations

    const int numPutOperations = 1000; // You can adjust this number as needed
    for (int i = 0; i < numPutOperations; ++i) {
        std::string key = "key_" + std::to_string(i);
        std::string value = "value_" + std::to_string(i);
        db->put(key, value);
    }
    // 999 remain in the database
    ASSERT_EQ(db->get("key_999"), "value_999");
    // key_1 should be in immutable trees and possibly get flushed into disk.
    ASSERT_EQ(db->get("key_1"), "value_1");
    // Swap the mem_tree
    db->swap_mem_tree();

    // Verify that the new mem_tree is now mutable one
    ASSERT_FALSE(db->tree->is_immutable());

    // Check that the newly created mem_tree is empty (assuming "default val" means empty)
    ASSERT_EQ(db->get("some_random_key"), std::nullopt);
}








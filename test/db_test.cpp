//
// Created by 钟卓凡 on 23/06/2023.
//

#include <gtest/gtest.h>
#include "src/db.h"
#include "src/config.h"
#include "src/mem_tree_impl.h"
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

    std::string k1, v1, k2, v2, k3, v3;
    std::shared_ptr<database_blocks::db> db;
};

TEST_F(DbTests, EmptyBuildTest) {
    db->put(k1, v1);
}

TEST_F(DbTests, WALTEST) {
    auto mgr = database_blocks::wal("./db1.txt");
    mgr.write_to_wal("123456");
    mgr.read_from_wal("./db1.txt");
}

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
    }

    std::shared_ptr<database_blocks::db> db;
};

TEST_F(DbTests, WalTest) {
    db->trees.emplace_back();
    db->trees.emplace_back();
}

TEST_F(DbTests, WALTEST){
    auto mgr = database_blocks::wal("./db1.txt");
    mgr.write_to_wal("123456");
    mgr.read_from_wal("./db1.txt");
}

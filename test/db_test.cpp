//
// Created by 钟卓凡 on 23/06/2023.
//

#include <gtest/gtest.h>
#include "db.h"
#include "config.h"

class DbTests : public  ::testing::Test {
protected:
    void SetUp() override{
        db = std::make_shared<database_blocks::db>(database_blocks::configs::default_config());
    }

    std::shared_ptr<database_blocks::db> db;
};

TEST_F(DbTests, SimpleDb) {
}
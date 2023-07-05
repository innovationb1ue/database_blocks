//
// Created by jeff zhong on 2023/7/5.
//
#include "gtest/gtest.h"
#include "fstream"
#include "cstdio"
#include "src/wal.h"


// Define a fixture for testing
class LogFileTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    database_blocks::wal db_wal = database_blocks::wal("wal_test.txt");
};


// Test case for writeToLogFile and deserializeFromLogFile
TEST_F(LogFileTest, SerializationDeserializationTest) {
    // Test data
    std::string operation = "write";
    std::string key = "data";
    std::string val = "123";

    // Serialize the data
    db_wal.write_to_wal(operation, key, val);

    // Deserialize the data from the log file
    auto deserializedData = db_wal.read_from_wal();

    // Verify the deserialized data
    EXPECT_EQ(deserializedData.size(), 1);
    EXPECT_EQ(deserializedData[key], val);

}


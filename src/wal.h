//
// Created by 钟卓凡 on 25/06/2023.
//

#ifndef DATABASE_BLOCKS_WAL_H
#define DATABASE_BLOCKS_WAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>

namespace database_blocks {

    /*
     * WAL should be appended only file and each time we do an update on memory we write into WAL first.
     * 1. WAL should get flushed every time we write into it.
     * 2. No timestamp. Each mem_tree will have exactly one WAL related to it.
     * 3. WAL should be light since we never sort the entries, and we do not random access it.
     * 4. WAL is used as disaster backup when write to mem_tree does not flushed to disk.
     * 5. provide clean method that we can use to remove unnecessary WAL file (be in wal manager).
     *
     * */
    class wal {
    public:
        explicit wal(const std::filesystem::path &log_file_path);

        ~wal();

        wal(const wal &other) {
            this->log_file_path_ = other.log_file_path_;
            this->log_file_ = std::fstream(this->log_file_path_, std::ios::in | std::ios::app | std::ios::binary);
        }
//
//        wal(const wal &&other) {
//            this->log_file_path_ = other.log_file_path_;
//            this->log_file_ = std::fstream(this->log_file_path_, std::ios::in | std::ios::app | std::ios::binary);
//        }

        void write_to_wal(const std::string &operation, const std::string &key, const std::string &val);

        // read full WAL
        std::map<std::string, std::string> read_from_wal();

        // WAL path.
        std::filesystem::path log_file_path_;
    private:
        // hold wal file handle
        std::fstream log_file_;
    };

}


#endif //DATABASE_BLOCKS_WAL_H

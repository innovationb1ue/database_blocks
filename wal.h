//
// Created by 钟卓凡 on 25/06/2023.
//

#ifndef DATABASE_BLOCKS_WAL_H
#define DATABASE_BLOCKS_WAL_H
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
namespace database_blocks {

    namespace fs = std::filesystem;

    class wal_manager {
    public:
        explicit wal_manager(const fs::path &log_file_path);

        ~wal_manager();

        void write_to_wal(const fs::path &log_file_path, const std::string &data);

        void read_from_wal(const fs::path &log_file_path);

    private:
        fs::path log_file_path_;
        std::ofstream log_file_;
    };

}


#endif //DATABASE_BLOCKS_WAL_H

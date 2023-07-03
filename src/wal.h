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

    /*
     * WAL should be appended only file and each time we do an update on memory we write into WAL first.
     * 1. WAL should get persisted every time we write into it.
     * 2. It should contain a timestamp, so we can decide which operation decide the final value of the target key.
     * 3. WAL should be light since we never sort the entries, and we do not random access it.
     * 4. provide clean method that we can use to remove unnecessary WAL file.
     *
     * */
    class wal {
    public:
        explicit wal(const fs::path &log_file_path);

        ~wal();

        void write_to_wal(const std::string &data);

        static std::string &read_from_wal(const fs::path &log_file_path);

    private:
        fs::path log_file_path_;
        std::fstream log_file_;
    };

}


#endif //DATABASE_BLOCKS_WAL_H

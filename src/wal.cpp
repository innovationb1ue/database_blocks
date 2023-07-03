
#include "wal.h"
#include "iostream"
#include "uuid.h"
#include "fstream"

namespace database_blocks {
    wal::wal(const fs::path &log_file_path)
            : log_file_path_(log_file_path), log_file_(log_file_path, std::ios::in | std::ios::app | std::ios::binary) {
        if (!log_file_) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
        }
    }

    wal::~wal() {
        log_file_.close();
    }

    std::string &wal::read_from_wal(const std::filesystem::path &log_file_path) {
        std::ifstream read_file(log_file_path, std::ios::in | std::ios::binary);
        if (!read_file) {
            std::cerr << "Failed to open log file for reading: " << log_file_path << std::endl;
            // Handle error
        }

        std::size_t length = 6;
        while (!read_file.eof()) {
            std::string data(length, '\0');
            read_file.read(&data[0], length);

            // Process the read data (e.g., print to console)
            std::cout << "Read data: " << data << std::endl;
        }
        if (!read_file.eof()) {
            std::cerr << "Error occurred while reading from log file: " << log_file_path << std::endl;
            // Handle error
        }
        read_file.close();
        return (std::string &) "123";
    }

    void wal::write_to_wal(const std::string &data) {
        // Write the length of the data as a 4-byte integer
        // wal file format:
        // int_64_t: 8 bytes key length.
        // int_64_t: 8 bytes val length.
        // key
        //
        const size_t length = data.length();
        log_file_.write(reinterpret_cast<const char *>(&length), sizeof(size_t));

        // Write the actual data
        log_file_.write(data.c_str(), length);
        log_file_.flush();

        if (!log_file_) {
            std::cerr << "Failed to write to log file: " << log_file_path_ << std::endl;
            // Handle error
        }

        log_file_.close();
    }
}
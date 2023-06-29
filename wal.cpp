
#include "wal.h"
#include "iostream"
#include "uuid.h"

namespace database_blocks {
    wal_manager::wal_manager(const fs::path &log_file_path)
            : log_file_path_(log_file_path), log_file_(log_file_path, std::ios::app | std::ios::binary) {
        if (!log_file_) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
            // Handle error
        }
    }

    wal_manager::~wal_manager() {
        log_file_.close();
    }

    void wal_manager::read_from_wal(const std::filesystem::path& log_file_path) {
        std::ifstream read_file(log_file_path, std::ios::in | std::ios::binary);
        if (!read_file) {
            std::cerr << "Failed to open log file for reading: " << log_file_path << std::endl;
            // Handle error
        }

        std::size_t length = 0;
        while (read_file.read(reinterpret_cast<char*>(&length), sizeof(std::size_t))) {
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
    }

    void wal_manager::write_to_wal(const std::filesystem::path& log_file_path, const std::string& data) {
        std::ofstream log_file(log_file_path, std::ios::app | std::ios::binary);
        if (!log_file) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
            // Handle error
        }

        // Write the length of the data as a 4-byte integer
        const std::size_t length = data.length();
        log_file.write(reinterpret_cast<const char*>(&length), sizeof(std::size_t));

        // Write the actual data
        log_file.write(data.c_str(), length);

        if (!log_file) {
            std::cerr << "Failed to write to log file: " << log_file_path << std::endl;
            // Handle error
        }

        log_file.close();
    }

}

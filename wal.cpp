
#include "wal.h"
#include "iostream"

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

}

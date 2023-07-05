
#include "wal.h"

namespace database_blocks {
    wal::wal(const std::filesystem::path &log_file_path)
            : log_file_path_(log_file_path), log_file_(log_file_path, std::ios::in | std::ios::app | std::ios::binary) {
        if (!log_file_) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
        }
    }

    wal::~wal() {
        log_file_.close();
    }

    std::map<std::string, std::string> wal::read_from_wal() {
        std::map<std::string, std::string> dataMap;

        log_file_.seekp(std::ios::beg);

        // Read the file and deserialize the data
        while (true) {
            // Read the sizes from the log file
            size_t operationSize;
            size_t keySize;
            size_t valSize;

            log_file_.read(reinterpret_cast<char *>(&operationSize), sizeof(size_t));
            log_file_.read(reinterpret_cast<char *>(&keySize), sizeof(size_t));
            log_file_.read(reinterpret_cast<char *>(&valSize), sizeof(size_t));

            // Check for end-of-file
            if (log_file_.eof()) {
                break;
            }// Read the values from the log file
            std::string operation(operationSize, '\0');
            std::string key(keySize, '\0');
            std::string val(valSize, '\0');

            log_file_.read(&operation[0], operationSize);
            log_file_.read(&key[0], keySize);
            log_file_.read(&val[0], valSize);

            // Add the key-value pair to the map
            dataMap[key] = val;
        }

        return dataMap;
    }

    void wal::write_to_wal(const std::string &operation, const std::string &key, const std::string &val) {
        // Calculate the sizes of the strings
        auto operationSize = static_cast<size_t>(operation.size());
        auto keySize = static_cast<size_t>(key.size());
        auto valSize = static_cast<size_t>(val.size());

        // Write the sizes to the log file
        log_file_.write(reinterpret_cast<const char *>(&operationSize), sizeof(size_t));
        log_file_.write(reinterpret_cast<const char *>(&keySize), sizeof(size_t));
        log_file_.write(reinterpret_cast<const char *>(&valSize), sizeof(size_t));

        // Write the values to the log file
        log_file_.write(operation.data(), operationSize);
        log_file_.write(key.data(), keySize);
        log_file_.write(val.data(), valSize);

        // Flush immediately
        log_file_.flush();
    }
}
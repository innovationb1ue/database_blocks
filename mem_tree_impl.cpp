#include "mem_tree_impl.h"

#include <iostream>
#include <fstream>
#include "config.h"

#define separator ";"
namespace database_blocks {

    database_blocks::mem_tree::mem_tree() {
        configs default_config = configs::default_config();
        this->_store = std::make_shared<std::map<std::string, std::string>>();
        this->immutable = false;
    }

    database_blocks::mem_tree::mem_tree(configs& config): config(config){
        this->_store = std::make_shared<std::map<std::string, std::string>>();
        this->immutable = false;
    }

    database_blocks::mem_tree::mem_tree(configs&& config): config(std::move(config)){
        this->_store = std::make_shared<std::map<std::string, std::string>>();
        this->immutable = false;
    }

    void database_blocks::mem_tree::flush(std::filesystem::path dst) {
        if (!immutable) {
            set_immutable();
        }
        std::ofstream file;
        file.open(dst, std::ios::binary);
        auto it = _store->begin();
        while (it != _store->end()) {
            auto key_size = it->first.size();
            auto val_size = it->second.size();
            file.write(reinterpret_cast<const char *>(&key_size), sizeof(key_size));
            file.write(reinterpret_cast<const char *>(&val_size), sizeof(val_size));
            file.write(it->first.data(), it->first.size());
            file.write(it->second.data(), it->second.size());

            it++;
        }
        file.flush();
    }

// put arbitrary byte data into the map.
    bool database_blocks::mem_tree::put(std::string &key, std::string &val) {
        if (immutable) {
            return false;
        }
        auto res = _store->insert_or_assign(key, val);
        // set immutable if maximum size is reached
        size += (key.size() + val.size());
        if (size > config.mem_tree_size) {
            this->set_immutable();
        }
        return res.second;
    }

    void database_blocks::mem_tree::set_immutable() {
        immutable = true;
    }

    bool database_blocks::mem_tree::is_immutable() const {
        return immutable;
    }

    bool database_blocks::mem_tree::merge(const database_blocks::mem_tree &&other) {
        this->_store->merge(std::move(*other.get_store().get()));
        return true;
    }

    database_blocks::store_type database_blocks::mem_tree::get_store() const {
        return _store;
    }

    bool database_blocks::mem_tree::remove(std::string &key) {
        return this->_store->erase(key);
    }

    std::optional<std::string> database_blocks::mem_tree::get(std::string &key) {
        auto res = this->_store->find(key);
        return res == this->_store->end() ?
               std::nullopt :
               std::optional<std::reference_wrapper<std::string>>{res->second};
    }

    void database_blocks::mem_tree::load() {
        std::ifstream myfile;
        // todo: make the file path a parameter.
        myfile.open("db.txt", std::ios::binary);
        myfile.seekg(0, std::ios::beg);
        const auto start = myfile.tellg();
        myfile.seekg(0, std::ios::end);
        const auto end = myfile.tellg();
        const auto file_size = end - start;

        myfile.seekg(0, std::ios::beg);
        while (!myfile.eof()) {
            size_t key_size;
            myfile.read(reinterpret_cast<char *>(&key_size), sizeof(key_size));
            size_t value_size;
            myfile.read(reinterpret_cast<char *>(&value_size), sizeof(value_size));
            std::string key, val;
            key.resize(key_size);
            myfile.read(key.data(), key_size);
            val.resize(value_size);
            myfile.read(val.data(), value_size);
            this->_store->emplace(std::move(key), std::move(val));
        }
    }

    void mem_tree::clear() {
        this->_store->clear();
    }
}
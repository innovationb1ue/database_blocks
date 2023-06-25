#include "mem_tree_impl.h"

#include <iostream>
#include <fstream>
#include "config.h"

namespace database_blocks {
    database_blocks::mem_tree::mem_tree(configs &config) : config(config) {
        this->_store = std::map<std::string, std::string>();
        this->immutable = false;
        this->path = config.db_store_path / "db.txt";
    }

    database_blocks::mem_tree::mem_tree(configs &&config){
        this->_store = std::map<std::string, std::string>();
        this->immutable = false;
        this->path = config.db_store_path / "db.txt";
        this->config = std::move(config);
    }

    database_blocks::mem_tree::mem_tree() : mem_tree(configs::default_config()){

    };

    // flush data to target file.
    // if the file already exist. we merge them emplace.
    void database_blocks::mem_tree::flush(const std::filesystem::path& dst) {
        std::scoped_lock lock(latch);
        if (!immutable) {
            set_immutable();
        }
        std::ofstream file;
        // merge if we need
        if (std::filesystem::exists(dst)){
            auto file_tree = mem_tree();
            file_tree.load(dst);
            this->merge(std::move(file_tree));

        }
        file.open(dst, std::ios::binary);
        auto it = _store.begin();
        while (it != _store.end()) {
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
        std::scoped_lock lock(latch);
        if (immutable) {
            return false;
        }
        auto res = _store.insert_or_assign(key, val);
        // set immutable if maximum size is reached
        size += (key.size() + val.size());
        if (size > config.mem_tree_size) {
            this->set_immutable();
        }
        return res.second;
    }

    void database_blocks::mem_tree::set_immutable() {
        std::scoped_lock l(latch);
        immutable = true;
    }

    bool database_blocks::mem_tree::is_immutable() {
        std::scoped_lock lock(latch);
        return immutable;
    }

    bool database_blocks::mem_tree::merge(const database_blocks::mem_tree &&other) {
        this->_store.merge(std::move(other.get_store()));
        return true;
    }

    database_blocks::store_type database_blocks::mem_tree::get_store() const {
        return _store;
    }

    bool database_blocks::mem_tree::remove(std::string &key) {
        return this->_store.erase(key);
    }

    std::optional<std::string> database_blocks::mem_tree::get(std::string &key) {
        auto res = this->_store.find(key);
        return res == this->_store.end() ?
               std::nullopt :
               std::optional<std::reference_wrapper<std::string>>{res->second};
    }

    void database_blocks::mem_tree::load() {
        return load(std::filesystem::current_path() / "db.txt");
    }

    void mem_tree::clear() {
        std::scoped_lock lock(latch);
        this->_store.clear();
    }

    void mem_tree::load(const std::filesystem::path& src) {
        std::scoped_lock lock(latch);
        if (!std::filesystem::exists(path)){
            return;
        }

        std::ifstream src_file;
        src_file.open(src, std::ios::binary);
        src_file.seekg(0, std::ios::beg);
        while (!src_file.eof()) {
            size_t key_size;
            src_file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size));
            size_t value_size;
            src_file.read(reinterpret_cast<char *>(&value_size), sizeof(value_size));
            std::string key, val;
            key.resize(key_size);
            src_file.read(key.data(), key_size);
            val.resize(value_size);
            src_file.read(val.data(), value_size);
            this->_store.emplace(std::move(key), std::move(val));
        }

    }
}
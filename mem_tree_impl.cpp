#include "mem_tree_impl.h"

#include <iostream>
#include <fstream>
#include "config.h"
#include "cstdint"


namespace database_blocks {
    database_blocks::mem_tree::mem_tree(configs &config) : config(config) {
        this->_store = std::map<std::string, std::string>();
        this->immutable = false;
        this->path = config.db_store_path / "db.txt";
    }

    database_blocks::mem_tree::mem_tree(configs &&config) {
        this->_store = std::map<std::string, std::string>();
        this->immutable = false;
        this->path = config.db_store_path / "db.txt";
        this->config = config;
    }

    std::string database_blocks::mem_tree::serialize() {
        std::string buf;
        // total element(key, val) size and two length int64 for each.
        // do we need a timestamp here?
        size_t encode_size = this->size + this->_store.size() * sizeof(int64_t) * 2;
        buf.resize(encode_size);
        char *p = buf.data();
        for (auto &it: _store) {
            auto key_size = it.first.size();
            auto val_size = it.second.size();
            // write key size
            memcpy(p, reinterpret_cast<const char *>(&key_size), sizeof(key_size));
            p += sizeof(key_size);
            // write val size
            memcpy(p, reinterpret_cast<const char *>(&val_size), sizeof(val_size));
            p += sizeof(val_size);
            // write key
            memcpy(p, it.first.data(), it.first.size());
            p += key_size;
            // write val
            memcpy(p, it.second.data(), it.second.size());
            p += val_size;
        }
        return buf;
    }

    database_blocks::mem_tree::mem_tree() : mem_tree(configs::default_config()) {}

    // flush data to target file.
    // if the file already exist. we merge them emplace.
    void database_blocks::mem_tree::flush(const std::filesystem::path &dst) {
        if (!immutable) {
            set_immutable();
        }
        std::ofstream file;
        file.open(dst, std::ios::binary | std::ios::trunc);
        for (auto &it: _store) {
            auto key_size = it.first.size();
            auto val_size = it.second.size();
            file.write(reinterpret_cast<const char *>(&key_size), sizeof(key_size));
            file.write(reinterpret_cast<const char *>(&val_size), sizeof(val_size));
            file.write(it.first.data(), it.first.size());
            file.write(it.second.data(), it.second.size());
        }
        file.flush();
    }

    void database_blocks::mem_tree::flush() {
        return flush(path);
    }

    // put arbitrary byte data into the map.
    template<class T>
    requires std::is_same<T &, std::string &>::value
    bool database_blocks::mem_tree::put(T &&key, T &&val) {
        if (immutable) {
            return false;
        }
        auto pre_element = _store.find(key);
        // add new key and val size.
        size += (key.size() + val.size());
        size_t pre_val_size = 0;
        // has same key element, just replace the value.
        if (pre_element != _store.end()) {
            // remove the old value from total size.
            pre_val_size = pre_element->second.size();
            size -= pre_val_size;
            // forwarding the value to
            pre_element->second = std::forward<T>(val);
        } else {
            _store.emplace(std::forward<T>(key), std::forward<T>(val));
        }

        if (size > config.mem_tree_size) {
            this->set_immutable();
        }
        return pre_element == _store.end();
    }

    template bool database_blocks::mem_tree::put<std::string &&>(std::string &&, std::string &&);

    template bool database_blocks::mem_tree::put<std::string &>(std::string &, std::string &);

    void database_blocks::mem_tree::set_immutable() {
        immutable = true;
    }

    bool database_blocks::mem_tree::is_immutable() const {
        return immutable;
    }

    bool database_blocks::mem_tree::merge(const database_blocks::mem_tree &&other) {
        this->_store.merge(std::move(other.get_store()));
        return true;
    }

    bool database_blocks::mem_tree::merge(const std::filesystem::path &file) {
        // merge if the file exist
        if (std::filesystem::exists(file)) {
            auto file_tree = mem_tree();
            file_tree.load(file);
            this->merge(std::move(file_tree));
            return true;
        }
        return false;
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
        this->_store.clear();
    }

    void mem_tree::load(const std::filesystem::path &src) {
        if (!std::filesystem::exists(path)) {
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
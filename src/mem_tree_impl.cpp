#include "mem_tree_impl.h"

#include <iostream>
#include <fstream>
#include "config.h"
#include "cstring"


namespace database_blocks {
    mem_tree::mem_tree(configs &config) : mem_tree(std::move(config)) {}

    mem_tree::mem_tree(configs &&config) :
            id(uuid_util::random_uuid()), config(config),
            tree_wal(to_string(this->id) + ".txt"),
            immutable(false), path(config.db_store_path / "db.txt") {}

    std::string database_blocks::mem_tree::serialize() {
        std::string buf;
        // total element(key, val) kv_size_in_bytes and two length int64 for each.
        // do we need a timestamp here?
        size_t encode_size = this->kv_size_in_bytes + this->_store.size() * sizeof(int64_t) * 2;
        buf.resize(encode_size);
        char *p = buf.data();
        for (auto &it: _store) {
            auto key_size = it.first.size();
            auto val_size = it.second.size();
            // write key kv_size_in_bytes
            memcpy(p, reinterpret_cast<const char *>(&key_size), sizeof(key_size));
            p += sizeof(key_size);
            // write val kv_size_in_bytes
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

    // deserialize string into mem_tree
    std::string mem_tree::deserialize(std::string val, size_t size) {
        if (size == 0) {
            return {};
        }
        auto pos = val.data();
        size_t deserialized_size = 0;
        size_t *k_size;
        size_t *v_size;
        while (deserialized_size < size) {
            // get sizes
            k_size = reinterpret_cast<size_t *>(pos);
            pos += sizeof(size_t);
            v_size = reinterpret_cast<size_t *>(pos);
            pos += sizeof(size_t);
            // construct map element.
            _store.emplace(std::string(pos, *k_size), std::string(pos + *k_size, *v_size));
            pos += *k_size + *v_size;
            deserialized_size += 2 * sizeof(size_t) + *k_size + *v_size;
        }
        return {};
    }

    mem_tree::mem_tree() : mem_tree(configs::default_config()) {}

    // flush data to target file. truncate file first.
    void mem_tree::flush(const std::filesystem::path &dst) {
        if (!immutable) {
            set_immutable();
        }
        std::ofstream file;
        // will truncate the file if it exists.
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
    
    void mem_tree::set_immutable() {
        immutable = true;
    }

    bool mem_tree::is_immutable() const {
        return immutable;
    }

    bool mem_tree::merge(const database_blocks::mem_tree &&other) {
        this->_store.merge(std::move(other.get_store()));
        return true;
    }

    bool mem_tree::merge(const std::filesystem::path &file) {
        // merge if the file exist
        if (std::filesystem::exists(file)) {
            auto file_tree = mem_tree();
            file_tree.load(file);
            this->merge(std::move(file_tree));
            return true;
        }
        return false;
    }

    store_type mem_tree::get_store() const {
        return _store;
    }

    bool mem_tree::remove(std::string &key) {
        return this->_store.erase(key);
    }

    std::optional<std::string> mem_tree::get(const std::string &key) {
        auto res = this->_store.find(key);
        return res == this->_store.end() ?
               std::nullopt :
               std::optional<std::reference_wrapper<std::string>>{res->second};
    }

    void mem_tree::load() {
        return load(std::filesystem::current_path() / "db.txt");
    }

    void mem_tree::clear() {
        this->_store.clear();
    }

    void mem_tree::clear_wal() {
        this->tree_wal.remove();
    }

    void mem_tree::load(const std::filesystem::path &src) {
        clear(); // Clear the existing data before loading

        std::ifstream file;
        file.open(src, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for loading: " + src.string());
        }

        while (!file.eof()) {
            // Read the key size
            std::size_t key_size;
            file.read(reinterpret_cast<char *>(&key_size), sizeof(key_size));

            // Read the value size
            std::size_t val_size;
            file.read(reinterpret_cast<char *>(&val_size), sizeof(val_size));

            if (file.eof()) {
                break;
            }

            // Read the key
            std::string key(key_size, '\0');
            file.read(&key[0], key_size);

            // Read the value
            std::string value(val_size, '\0');
            file.read(&value[0], val_size);

            // Insert the key-value pair into the mem_tree
            _store[key] = value;
        }

        file.close();
    }


}
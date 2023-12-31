#ifndef DATABASE_BLOCKS_MEM_TREE_IMPL_H
#define DATABASE_BLOCKS_MEM_TREE_IMPL_H

#include <map>

#include "config.h"
#include "wal.h"
#include "uuid.h"
#include "uuid_util.h"
#include <mutex>
#include <vector>
#include <optional>
#include <concepts>

namespace database_blocks {
    typedef std::map<std::string, std::string> store_type;

    // this class is not synchronized internally.
    // so make sure it is used in concurrent safe environment.
    class mem_tree {
    public:
        mem_tree();

        explicit mem_tree(database_blocks::configs &config);

        explicit mem_tree(database_blocks::configs &&config);

        // default copy
        mem_tree(const mem_tree &other) = default;

        // default move
        mem_tree(mem_tree &&other) noexcept: tree_wal(other.tree_wal.log_file_path_) {
            this->_store = std::move(other._store);
            this->kv_size_in_bytes = other.kv_size_in_bytes;
            this->immutable = other.immutable;
            this->path = std::move(other.path);
        };

        // flush to disk.
        void flush(const std::filesystem::path &);

        // flush to default path
        void flush();

        // put a k-v pair into the tree
        template<class T>
        bool put(T &&key, T &&val) {
            if (immutable) {
                return false;
            }
            // write to WAL
            this->tree_wal.write_to_wal("PUT", std::forward<T>(key), std::forward<T>(val));

            auto pre_element = _store.find(key);
            // add new key and val kv_size_in_bytes.
            kv_size_in_bytes += (key.size() + val.size());
            size_t pre_val_size = 0;
            // has same key element, just replace the value.
            if (pre_element != _store.end()) {
                // remove the old value from total kv_size_in_bytes.
                pre_val_size = pre_element->second.size();
                kv_size_in_bytes -= pre_val_size;
                // forwarding the value to
                pre_element->second = std::forward<T>(val);
            } else {
                _store.emplace(std::forward<T>(key), std::forward<T>(val));
            }

            if (kv_size_in_bytes > config.mem_tree_size) {
                this->set_immutable();
            }
            return pre_element == _store.end();
        }

        // clear tree
        void clear();

        // clear wal
        void clear_wal();

        bool remove(std::string &key);

        // serialize the tree data into bytes contained in the std::string.
        // intended for storage purpose.
        std::string serialize();

        // deserialize the tree from byte stream;
        // will insert the data decoded into _store.
        std::string deserialize(std::string, size_t size);


        // merge two mem_tree into one.
        bool merge(const mem_tree &&other);

        // merge by file path.
        bool merge(const std::filesystem::path &);

        // set this tree in memory to immutable state and ready to be flush into disk.
        void set_immutable();

        bool is_immutable() const;

        [[nodiscard]] store_type get_store() const;

        std::optional<std::string> get(const std::string &key);

        // default load. intentionally for tests.
        void load();

        // load from file.
        void load(const std::filesystem::path &);

    public:
        // global config.
        configs config;

    private:
        // tree ID. this has to be on top of tree_wal initialization.
        uuids::uuid id = uuid_util::random_uuid();
        // storage
        store_type _store;
        // storage kv_size_in_bytes in bytes with key and value kv_size_in_bytes added together.
        size_t kv_size_in_bytes = 0;
        // maximum storage kv_size_in_bytes in byte.
        bool immutable = false;
        // path for record file.
        std::string path;
        // WAL
        wal tree_wal;
    };
}


#endif //DATABASE_BLOCKS_MEM_TREE_IMPL_H

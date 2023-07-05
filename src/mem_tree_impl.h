#ifndef DATABASE_BLOCKS_MEM_TREE_IMPL_H
#define DATABASE_BLOCKS_MEM_TREE_IMPL_H

#include <map>

#include "config.h"
#include "wal.h"
#include "uuid.h"
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
        requires std::is_same<T &, std::string &>::value
        bool put(T &&key, T &&val);

        void clear();

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

        std::optional<std::string> get(std::string &key);

        // default load. intentionally for tests.
        void load();

        // load from file.
        void load(const std::filesystem::path &);

    public:
        // global config.
        configs config;

    private:
        // storage
        store_type _store;
        // storage kv_size_in_bytes in bytes with key and value kv_size_in_bytes added together.
        size_t kv_size_in_bytes = 0;
        // maximum storage kv_size_in_bytes in byte.
        bool immutable;
        // path for record file.
        std::string path;
        // WAL
        wal tree_wal;
        // tree ID
        uuids::uuid id;
    };
}

#endif //DATABASE_BLOCKS_MEM_TREE_IMPL_H

#ifndef DATABASE_BLOCKS_MEM_TREE_IMPL_H
#define DATABASE_BLOCKS_MEM_TREE_IMPL_H

#include <map>

#include "config.h"
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

        mem_tree(const mem_tree &other) = default;

        mem_tree(mem_tree &&other) = default;

        // flush to disk.
        void flush(const std::filesystem::path &);

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
        std::string deserialize();

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
        // storage size in bytes with key and value size added together.
        size_t size = 0;
        // maximum storage size in byte.
        bool immutable;
        // path for record file.
        std::string path;
    };
}

#endif //DATABASE_BLOCKS_MEM_TREE_IMPL_H

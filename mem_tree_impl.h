#ifndef DATABASE_BLOCKS_MEM_TREE_IMPL_H
#define DATABASE_BLOCKS_MEM_TREE_IMPL_H

#include<map>
#include "config.h"

namespace database_blocks {
    typedef std::shared_ptr<std::map<std::string, std::string>> store_type;

    class mem_tree {
    public:
        mem_tree();

        mem_tree(database_blocks::configs &config);

        mem_tree(database_blocks::configs &&config);

        // flush to disk.
        void flush(std::filesystem::path);

        // put a k-v pair into the tree
        bool put(std::string &key, std::string &val);

        void clear();

        bool remove(std::string &key);

        // merge two mem_tree into one.
        bool merge(const mem_tree &&other);

        // set this tree in memory to immutable state and ready to be flush into disk.
        void set_immutable();

        [[nodiscard]] bool is_immutable() const;

        [[nodiscard]] store_type get_store() const;

        std::optional<std::string> get(std::string &key);

        void load();

    public:
        // global config.
        configs config;

    private:
        // storage
        store_type _store;
        // storage size in bytes
        size_t size = 0;
        // maximum storage size in byte.
        bool immutable;
        // path for record file.
        std::string path;
    };
}

#endif //DATABASE_BLOCKS_MEM_TREE_IMPL_H

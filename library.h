#ifndef DATABASE_BLOCKS_LIBRARY_H
#define DATABASE_BLOCKS_LIBRARY_H

#include<map>
void hello();

typedef std::shared_ptr<std::map<std::string, std::string>> store_type;

namespace database_blocks{
    class mem_tree{
    public:
        mem_tree();
        void flush();
        bool put(std::string & key, std::string & val);
        bool remove(std::string & key);
        // merge two mem_tree into one.
        bool merge(const mem_tree && other);
        // set this tree in memory to immutable state and ready to be flush into disk.
        void set_immutable();
        [[nodiscard]] bool is_immutable() const;
        [[nodiscard]] store_type get_store() const;
        size_t size(){
            return _store->size();
        }
        std::optional<std::string> get(std::string & key);
        void load();

    private:
        store_type _store;
        size_t max_size;
        bool immutable;
    };
}

#endif //DATABASE_BLOCKS_LIBRARY_H

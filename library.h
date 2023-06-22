#ifndef DATABASE_BLOCKS_LIBRARY_H
#define DATABASE_BLOCKS_LIBRARY_H

#include<map>
void hello();

namespace database_blocks{
    class mem_tree{
    public:
        mem_tree();
        void flush();
        bool put(std::string & key, std::string & val);
        bool remove();
        bool merge();
        // set this tree in memory to immutable state and ready to be flush into disk.
        void set_immutable();

        size_t size(){
            return _store->size();
        }

    private:
        std::shared_ptr<std::map<std::string, std::string>> _store;
        size_t max_size;
        bool is_immutable;
    };
}

#endif //DATABASE_BLOCKS_LIBRARY_H

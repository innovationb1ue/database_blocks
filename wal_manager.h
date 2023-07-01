//
// Created by jeff zhong on 2023/7/1.
//

#ifndef DATABASE_BLOCKS_WAL_MANAGER_H
#define DATABASE_BLOCKS_WAL_MANAGER_H

#include "string"
#include "vector"
#include "array"
#include "wal.h"
#include "list"

namespace database_blocks {
    class wal_manager {
        wal_manager() = default;

        // write data into wal.
        // we should provide time stamp to action.
        void write(std::string &data);

    public:

    private:
        std::list<wal> wal_list;
    };

}


#endif //DATABASE_BLOCKS_WAL_MANAGER_H

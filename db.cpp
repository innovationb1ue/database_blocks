//
// Created by 钟卓凡 on 23/06/2023.
//

#include "db.h"

#include <utility>

database_blocks::db::db(database_blocks::configs config): config(std::move(config)) {

}
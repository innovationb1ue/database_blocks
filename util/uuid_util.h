//
// Created by jeff zhong on 2023/7/1.
//


#ifndef DATABASE_BLOCKS_UUID_UTIL_H
#define DATABASE_BLOCKS_UUID_UTIL_H


namespace database_blocks {
    class uuid_util {
    public:
        static uuids::uuid random_uuid() noexcept {
            std::random_device rd;
            auto seed_data = std::array<int, std::mt19937::state_size>{};
            std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
            std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
            std::mt19937 generator(seq);
            uuids::uuid_random_generator gen{generator};
            auto id = gen();
            return id;
        }
    };

}


#endif //DATABASE_BLOCKS_UUID_UTIL_H

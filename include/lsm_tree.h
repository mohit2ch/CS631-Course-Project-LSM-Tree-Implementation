#ifndef LSM_TREE_H
#define LSM_TREE_H

#include "memtable.h"
#include "sstable.h"

#include <vector>
#include <string>
#include <optional>
#include <cstddef>

class LSMTree {
public:
    explicit LSMTree(size_t memtable_capacity);

    LSMTree(size_t memtable_capacity, const std::string& db_path);

    void put(
        const std::string& key,
        const std::string& value
    );

    std::optional<std::string> get(
        const std::string& key
    ) const;

    void flush();

private:
    Memtable memtable;

    std::vector<std::string> sstables;

    size_t memtable_capacity;
    size_t next_sstable_id = 0;
};

#endif
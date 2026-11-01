
#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <map>
#include <string>
#include <optional>
#include <cstddef>
#include <vector>
#include <utility>

class Memtable {
public:
    void put(const std::string& key, const std::string& value);

    std::optional<std::string> get(
        const std::string& key
    ) const;

    size_t size() const;

    bool empty() const;
    std::vector<std::pair<std::string, std::string>> get_all() const;

private:
    std::map<std::string, std::string> data;
    
};

#endif

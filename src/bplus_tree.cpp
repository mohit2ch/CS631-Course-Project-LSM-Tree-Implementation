
#include "bplus_tree.h"

#include <algorithm>
#include <stdexcept>
#include <functional>

BPlusTree::BPlusTree(size_t max_keys)
    : max_keys(max_keys) {

    if (max_keys < 2) {
        throw std::invalid_argument(
            "B+ tree max_keys must be at least 2"
        );
    }

    root = std::make_unique<Node>(true);
}

BPlusTree::Node* BPlusTree::find_leaf(
    const std::string& key
) const {
    Node* current = root.get();

    while (!current->is_leaf) {
        auto it = std::upper_bound(
            current->keys.begin(),
            current->keys.end(),
            key
        );

        size_t index = it - current->keys.begin();

        current = current->children[index].get();
    }

    return current;
}

BPlusTree::InsertResult BPlusTree::insert_recursive(
    Node* node,
    const std::string& key,
    const std::string& value
) {
    // Case 1: Leaf node
    if (node->is_leaf) {
        auto it = std::lower_bound(
            node->keys.begin(),
            node->keys.end(),
            key
        );

        size_t index = it - node->keys.begin();

        // Update existing key.
        if (it != node->keys.end() && *it == key) {
            node->values[index] = value;
            return {};
        }

        node->keys.insert(it, key);

        node->values.insert(
            node->values.begin() + index,
            value
        );

        // No overflow.
        if (node->keys.size() <= max_keys) {
            return {};
        }

        // Split leaf.
        auto right = std::make_unique<Node>(true);

        size_t mid = node->keys.size() / 2;

        right->keys.assign(
            node->keys.begin() + mid,
            node->keys.end()
        );

        right->values.assign(
            node->values.begin() + mid,
            node->values.end()
        );

        node->keys.resize(mid);
        node->values.resize(mid);

        // Maintain linked leaves.
        right->next = node->next;
        node->next = right.get();

        // First key in right leaf becomes separator.
        return {
            right->keys.front(),
            std::move(right)
        };
    }

    // Case 2: Internal node.
    auto it = std::upper_bound(
        node->keys.begin(),
        node->keys.end(),
        key
    );

    size_t index = it - node->keys.begin();

    InsertResult result = insert_recursive(
        node->children[index].get(),
        key,
        value
    );

    if (!result.right_node) {
        return {};
    }

    // Insert separator and new child.
    node->keys.insert(
        node->keys.begin() + index,
        result.separator
    );

    node->children.insert(
        node->children.begin() + index + 1,
        std::move(result.right_node)
    );

    // No overflow.
    if (node->keys.size() <= max_keys) {
        return {};
    }

    // Split internal node.
    size_t mid = node->keys.size() / 2;

    std::string separator = node->keys[mid];

    auto right = std::make_unique<Node>(false);

    right->keys.assign(
        node->keys.begin() + mid + 1,
        node->keys.end()
    );

    right->children.reserve(
        node->children.size() - (mid + 1)
    );

    for (size_t i = mid + 1;
         i < node->children.size();
         ++i) {

        right->children.push_back(
            std::move(node->children[i])
        );
    }

    node->keys.resize(mid);
    node->children.resize(mid + 1);

    return {
        separator,
        std::move(right)
    };
}

void BPlusTree::insert(
    const std::string& key,
    const std::string& value
) {
    Node* leaf = find_leaf(key);

    auto it = std::lower_bound(
        leaf->keys.begin(),
        leaf->keys.end(),
        key
    );

    bool exists = (
        it != leaf->keys.end() && *it == key
    );

    InsertResult result = insert_recursive(
        root.get(),
        key,
        value
    );

    if (!exists) {
        ++entry_count;
    }

    // Root overflowed: create a new root.
    if (result.right_node) {
        auto new_root = std::make_unique<Node>(false);

        new_root->keys.push_back(result.separator);

        new_root->children.push_back(std::move(root));
        new_root->children.push_back(
            std::move(result.right_node)
        );

        root = std::move(new_root);
    }
}

std::optional<std::string> BPlusTree::get(
    const std::string& key
) const {
    Node* leaf = find_leaf(key);

    auto it = std::lower_bound(
        leaf->keys.begin(),
        leaf->keys.end(),
        key
    );

    if (it == leaf->keys.end() || *it != key) {
        return std::nullopt;
    }

    size_t index = it - leaf->keys.begin();

    return leaf->values[index];
}

std::vector<std::pair<std::string, std::string>>
BPlusTree::range_scan(
    const std::string& start,
    const std::string& end
) const {
    std::vector<std::pair<std::string, std::string>> result;

    if (end < start) {
        return result;
    }

    Node* leaf = find_leaf(start);

    while (leaf != nullptr) {
        for (size_t i = 0; i < leaf->keys.size(); ++i) {
            if (leaf->keys[i] > end) {
                return result;
            }

            if (leaf->keys[i] >= start) {
                result.emplace_back(
                    leaf->keys[i],
                    leaf->values[i]
                );
            }
        }

        leaf = leaf->next;
    }

    return result;
}

size_t BPlusTree::size() const {
    return entry_count;
}

bool BPlusTree::validate() const {
    if (!root) {
        return false;
    }

    struct ValidationInfo {
        bool valid = true;
        bool has_keys = false;

        std::string min_key;
        std::string max_key;

        size_t leaf_depth = 0;
        size_t entries = 0;
    };

    std::vector<const Node*> leaves;

    std::function<ValidationInfo(const Node*, size_t, bool)> check_node;

    check_node = [&](const Node* node,
                     size_t depth,
                     bool is_root) -> ValidationInfo {

        ValidationInfo result;

        if (!node) {
            result.valid = false;
            return result;
        }

        // Every node must respect the maximum key capacity.
        if (node->keys.size() > max_keys) {
            result.valid = false;
            return result;
        }

        // Check that keys are strictly increasing.
        for (size_t i = 1; i < node->keys.size(); i++) {
            if (node->keys[i - 1] >= node->keys[i]) {
                result.valid = false;
                return result;
            }
        }

        if (node->is_leaf) {
            // Leaves must not have children.
            if (!node->children.empty()) {
                result.valid = false;
                return result;
            }

            // Every leaf key must have a corresponding value.
            if (node->keys.size() != node->values.size()) {
                result.valid = false;
                return result;
            }

            // A non-root leaf should not be empty.
            if (!is_root && node->keys.empty()) {
                result.valid = false;
                return result;
            }

            leaves.push_back(node);

            result.has_keys = !node->keys.empty();
            result.entries = node->keys.size();
            result.leaf_depth = depth;

            if (result.has_keys) {
                result.min_key = node->keys.front();
                result.max_key = node->keys.back();
            }

            return result;
        }

        // Internal nodes must have one more child than separator keys.
        if (node->children.size() != node->keys.size() + 1) {
            result.valid = false;
            return result;
        }

        // Internal nodes should not store values.
        if (!node->values.empty()) {
            result.valid = false;
            return result;
        }

        // An internal root must have at least one separator.
        if (is_root && node->keys.empty()) {
            result.valid = false;
            return result;
        }

        std::vector<ValidationInfo> child_info;

        for (const auto& child : node->children) {
            ValidationInfo info =
                check_node(child.get(), depth + 1, false);

            if (!info.valid || !info.has_keys) {
                result.valid = false;
                return result;
            }

            child_info.push_back(info);
            result.entries += info.entries;
        }

        // All leaves below this internal node must have equal depth.
        for (size_t i = 1; i < child_info.size(); i++) {
            if (child_info[i].leaf_depth !=
                child_info[0].leaf_depth) {
                result.valid = false;
                return result;
            }
        }

        // Verify separator semantics:
        // keys[i] == minimum key in children[i + 1].
        for (size_t i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] != child_info[i + 1].min_key) {
                result.valid = false;
                return result;
            }

            // Keys in adjacent subtrees must not overlap.
            if (child_info[i].max_key >=
                child_info[i + 1].min_key) {
                result.valid = false;
                return result;
            }
        }

        result.has_keys = true;
        result.min_key = child_info.front().min_key;
        result.max_key = child_info.back().max_key;
        result.leaf_depth = child_info.front().leaf_depth;

        return result;
    };

    ValidationInfo info = check_node(root.get(), 0, true);

    if (!info.valid || info.entries != entry_count) {
        return false;
    }

    // Verify that the linked list of leaves matches DFS leaf order.
    for (size_t i = 0; i < leaves.size(); i++) {
        const Node* expected_next =
            (i + 1 < leaves.size()) ? leaves[i + 1] : nullptr;

        if (leaves[i]->next != expected_next) {
            return false;
        }
    }

    return true;
}
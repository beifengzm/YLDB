#pragma once

#include "base/common.h"

namespace qsdb::base {

struct Node {
    Offset offset;
    int32_t next;
    int32_t down;
};

class NodePool {
    struct Header {
        int32_t block_num;
        int32_t first_free;
        int64_t file_size;
    };

public:
    explicit NodePool(std::string_view filename);

    std::pair<BlockId, Node*> Alloc();
    void Dealloc(BlockId block_id);

    Node* GetNode(BlockId block_id);

private:
    void Expand();
    char* GetAddress(BlockId block_id) const {
        return data_ + sizeof(Header) + sizeof(Node) * block_id;
    }

    int fd_;

    Header* header_;
    char* data_;

    NodePool(const NodePool&) = delete;
    NodePool& operator=(const NodePool&) = delete;
};

} // namespace qsdb::base

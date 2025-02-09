#include <sys/mman.h>
#include <cassert>
#include <string_view>

#include "base/node_pool.h"
#include "base/file_util.h"

namespace qsdb::base {

NodePool::NodePool(std::string_view filename) {
    bool need_init = false;
    if (!FileUtil::Exists(filename)) {
        need_init = true;
        FileUtil::CreateFile(filename);
    }

    fd_ = open(filename.data(), O_RDWR);
    CHECK(fd_ != -1);

    fallocate(fd_, 0, 0, sizeof(Header));
    data_ = (char*)mmap(nullptr, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    CHECK(data_ != MAP_FAILED);

    header_ = reinterpret_cast<Header*>(data_);
    if (need_init) {
        header_->block_num = 0;
        header_->first_free = Null;
        header_->file_size = sizeof(Header);
    }
}

std::pair<BlockId, Node*> NodePool::Alloc() {
    if (header_->first_free == Null) Expand();

    auto block_id = header_->first_free;
    auto next_id = *reinterpret_cast<const int32_t*>(GetAddress(block_id));
    header_->first_free = next_id;
    return {block_id, GetNode(block_id)};
}

void NodePool::Dealloc(BlockId block_id) {
    *reinterpret_cast<int32_t*>(GetAddress(block_id)) = header_->first_free;
    header_->first_free = block_id;
}

Node* NodePool::GetNode(BlockId block_id) {
    if (block_id >= header_->block_num) return nullptr;
    return reinterpret_cast<Node*>(GetAddress(block_id));
}

void NodePool::Expand() {
    fallocate(fd_, 0, header_->file_size, sizeof(Node));
    header_->file_size += sizeof(Node);
    header_->first_free = header_->block_num;
    *reinterpret_cast<int32_t*>(GetAddress(header_->block_num)) = -1;
    header_->block_num += 1;
}

} // namespace qsdb::base

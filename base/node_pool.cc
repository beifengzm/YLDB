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
        /*** [TODO]实现功能: 初始化 header_ 的各个成员，参考 base/data_pool.cc
         * 提示：1. header_->node_num 表示 node 的数目
         *      2. header_->first_free 表示当前空闲的 node id, 初始为 Null(定义在 base/common.h)
         *      3. header_->file_size 为该 mmap 文件的大小，初始只存储了一个 Header
        ***/
    }
}

std::pair<NodeId, Node*> NodePool::Alloc() {
    /*** [TODO]实现功能: 分配一个空闲 node
     * 提示：1. 先判断 header_->first_free 是否为 Null，如果是，则需要先扩展一个新 Node(调用 Expand)
     *      2. 分配空闲链表上的第一个 node，并将 header_->first 置为链表上的下一个元素
     *      3. 返回 node_id 和 Node 指针(需要用到 GetNode 函数)
     *   @return 返回分配 node 的 node_id(node 对应的下标) 和 Node 指针
    ***/
}

void NodePool::Dealloc(NodeId node_id) {
    /*** [TODO]实现功能: 回收一个 node
     * 提示：1. 先将该 node 的前 4 个字节置为 header_->first_free
     *      2. 再将 header_->first_free 置为该 node_id
     *   @param node_id 待回收的 node_id
    ***/
    *reinterpret_cast<int32_t*>(GetAddress(node_id)) = header_->first_free;
    header_->first_free = node_id;
}

Node* NodePool::GetNode(NodeId node_id) {
    if (node_id >= header_->node_num) return nullptr;
    return reinterpret_cast<Node*>(GetAddress(node_id));
}

void NodePool::Expand() {
    fallocate(fd_, 0, header_->file_size, sizeof(Node));
    header_->file_size += sizeof(Node);
    header_->first_free = header_->node_num;
    *reinterpret_cast<int32_t*>(GetAddress(header_->node_num)) = -1;
    header_->node_num += 1;
}

} // namespace qsdb::base

#include <sys/mman.h>
#include <cassert>

#include "base/skiplist.h"
#include "base/file_util.h"

namespace qsdb::base {

SkipList::SkipList(std::string_view filename,
                   std::string_view node_pool_path,
                   std::string_view data_pool_path) {
    bool need_init = false;
    if (!FileUtil::Exists(filename)) {
        need_init = true;
        FileUtil::CreateFile(filename);
    }

    fd_ = open(filename.data(), O_RDWR);
    CHECK(fd_ != -1);

    fallocate(fd_, 0, 0, sizeof(Header));
    header_ = (Header*)mmap(nullptr, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    CHECK(header_ != MAP_FAILED);

    node_pool_ = std::make_unique<NodePool>(node_pool_path);
    data_pool_ = std::make_unique<DataPool>(data_pool_path);

    if (need_init) {
        int32_t last_id = Null;
        for (int i = 0; i < HEIGHT; ++i) {
            auto [head_id, head_node] = node_pool_->Alloc();
            if (i == 0) header_->first_head = head_id;
            head_node->offset = Null;
            head_node->next = Null;
            head_node->down = last_id;
            last_id = head_id;
        }
        header_->head = last_id;
        header_->entry_num = 0;
    }
}

void SkipList::Insert(std::string_view key, std::string_view value) {
    /*** [TODO]实现功能: 向跳表中插入一个 key/value pair
     * 提示：1. 调用 Alloc 为 key/value 分配空间，返回 key/value 在 data_pool 中的偏移【已实现】
     *      2. 从上到下，从左到右遍历，找到每一层需要插入新 key/value 的前一个 node，存储在 path 中
     *      3. 反向遍历 path, 依次插入新结点(调用 Random 来判断是否终止)
     *      4. 具体实现细节需要额外查询相关资料, 阅读一下 Remove 函数，能获得一些思路
     *   @param key 键
     *   @param value 值
    ***/

    auto offset = Alloc(key, value);
    auto node_id = header_->head;
    std::vector<Node*> path;

    // write your code
}

Offset SkipList::Remove(std::string_view key) {
    auto node_id = header_->head;
    Offset offset = Null;
    while (node_id != Null) {
        auto node = GetNode(node_id);
        if (node->next == Null) {
            node_id = node->down;
        } else {
            auto next_node = GetNode(node->next);
            auto next_entry = GetEntry(next_node->offset);
            auto cmp_ret = next_entry.Compare(key);
            if (cmp_ret < 0) {
                node_id = node->next;
                continue;
            }
            if (cmp_ret == 0) {
                node_pool_->Dealloc(node->next);
                node->next = next_node->next;
                offset = next_node->offset;
            }
            node_id = node->down;
        }
    }
    if (offset != Null) header_->entry_num--;
    return offset;
}

Entry SkipList::Find(std::string_view key) {
    /*** [TODO]实现功能: 从跳表中查找一个 key
     * 提示：1. 实现了 Insert，Find 就不难了
     *   @param key 待查找的键
     *   @return 返回查找到的 entry
    ***/
}

Offset SkipList::Alloc(std::string_view key, std::string_view value) {
    int size = 8 + key.length() + value.length();
    auto [offset, ptr] = data_pool_->Alloc(size);
    *reinterpret_cast<int32_t*>(ptr) = key.length();
    *reinterpret_cast<int32_t*>(ptr + 4) = value.length();
    memcpy(ptr + 8, key.data(), key.length());
    memcpy(ptr + 8 + key.length(), value.data(), value.length());
    return offset;
}

Entry SkipList::GetEntry(Offset offset) const {
    if (offset == Null) return Entry(offset, {}, {});
    auto raw_entry = data_pool_->GetData(offset);
    const char* ptr = raw_entry.data();
    int32_t key_len = *reinterpret_cast<const int32_t*>(ptr);
    int32_t value_len = *reinterpret_cast<const int32_t*>(ptr + 4);
    std::string_view key(ptr + 8, key_len);
    std::string_view value(ptr + 8 + key_len, value_len);
    return Entry(offset, key, value);
}

} // namespace qsdb::base

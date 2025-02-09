#include <sys/mman.h>
#include <string.h>
#include <cassert>

#include "base/data_pool.h"
#include "base/file_util.h"

namespace qsdb::base {

DataPool::DataPool(std::string_view filename) {
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
        header_->file_size = sizeof(Header);
        header_->current = sizeof(Header);
    }
}

std::pair<Offset, Pointer> DataPool::Alloc(int size) {
    /*** [TODO]实现功能: 从内存池分配一个指定大小的内存块
     * 提示：1. 调用 Expand 函数扩展需要的空间
     *      2. 在这块空间的头部存储这块空间的大小(int32_t, 4 个字节，在 Expand 时需要考虑这 4 个字节)
     *      3. 返回该空间的起始偏移和地址(这里偏移包含头部 4 个字节， 地址则不要包含， 需要额外加上 4)
     *   @param size 要分配的内存大小
     *   @return 分配的内存位于内存池的偏移和地址
    ***/
}

void DataPool::Dealloc(Offset offset) {}

std::string_view DataPool::GetData(Offset offset) const {
    /*** [TODO]实现功能: 根据偏移获取存储在该空间的数据
     * 提示：1. 需要先判断 offset 是否为 Null(定义在 base/common.h)，如果是，返回一个空数据
     *      2. 根据偏移找到数据的地址以及数据的长度(数据长度存储在该空间起始的 4 个字节中)
     *      3. 返回类型是 std::string_view, 是为了避免拷贝, 可以了解一下这个类的用法
     *   @param offset 要读取的数据在内存池中的偏移
     *   @return 位于该偏移处的数据
    ***/
}

void DataPool::Expand(int32_t size) {
    fallocate(fd_, 0, header_->file_size, size);
    header_->file_size += size;
}

} // namespace qsdb::base

#include "base/skiplist.h"
#include "core/db.h"

namespace qsdb {

DB::DB(const DBOptions& optionts)
    : options_(optionts) {
    std::string db_prefix_path = options_.db_dir + "/" + options_.db_name;
    std::string db_path = db_prefix_path + "/db";
    std::string node_path = db_prefix_path + "/node_pool";
    std::string data_path = db_prefix_path + "/data_pool";
    skiplist_ = new base::SkipList(db_path, node_path, data_path);
}

DB::~DB() {
    if (skiplist_) delete skiplist_;
}

void DB::Get(const GetRequest& request, GetResponse* response) {
    /*** [TODO]实现功能: 从存储系统中获取数据
     * 提示：1. 从 request 中获取 key(db_name,key,value均存放在request.body，通过各自的 len 获取)，
     *         这里需要了解一下柔性数组的用法
     *      2. 从跳表中 Find 查询的 key, 检查返回的 entry 是否有效(entry.Valid())
     *      3. 如果无效，将 response->state 置为 State::kInvKey(通过 static_cast 将其转换为 int8_t)
     *      4. 如果有效，将 response->state 置为 State::kOK，并从 entry 中得到对应的key/value，
     *         并赋值给 response 对应的字段
     *   @param request Get 请求信息(定义见 core/message.h)
     *   @param response Get 响应信息(定义见 core/message.h)
    ***/
}

void DB::Put(const PutRequest& request, PutResponse* response) {
    const char* ptr = &request.body[0] + request.db_len;
    std::string_view key(ptr, request.key_len);
    std::string_view value(ptr + request.key_len, request.value_len);
    skiplist_->Remove(key);
    auto put_type = static_cast<PutType>(request.put_type);
    if (put_type != PutType::Delete) skiplist_->Insert(key, value);
    response->state = static_cast<int8_t>(State::kOK);
}

}   // namespace qsdb

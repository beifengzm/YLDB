#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

#include "core/db.h"
#include "core/db_server.h"
#include "core/socket.h"

namespace qsdb {

DBServer::DBServer(const DBServerOptions& options)
    : options_(options), thread_pool_(options.thread_num) {
    // load all dbs
    for (const auto& entry : fs::directory_iterator(options.db_dir)) {
        if (!entry.is_directory()) continue;
        const auto& db_name = entry.path().filename().string();
        DBOptions db_options;
        db_options.db_dir = options.db_dir;
        db_options.db_name = db_name;
        DB* db = new DB(db_options);
        name2db_[db_name] = db;
    }
}

DBServer::~DBServer() {
    thread_pool_.JoinAll();

    for (auto [name, db] : name2db_) {
        if (db) delete db;
    }

    if (sockfd_ != Null) {
        close(sockfd_);
    }

    if (client_fd_ != Null) {
        close(client_fd_);
    }
}

IOBuf DBServer::ReadData(int client_fd, int length) {
    IOBuf buffer(length);
    char* ptr = buffer.data();
    int current = 0;
    while (current < length) {
        int nread = read(client_fd, ptr + current, length - current);
        current += nread;
    }
    return buffer;
}

void DBServer::Start() {
    sockfd_ = NetSocket::Socket();
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(options_.port);

    NetSocket::Bind(sockfd_, (struct sockaddr*)&address, sizeof(address));
    NetSocket::Listen(sockfd_, options_.backlog);

    struct sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(client_addr);
    client_fd_ = NetSocket::Accept(sockfd_, (struct sockaddr*)&client_addr, &addrlen);

    while (true) {
        auto raw_head = ReadData(client_fd_, sizeof(MessageHead));
        auto& head = *reinterpret_cast<const MessageHead*>(raw_head.data());
        int32_t length = head.length;
        auto type = static_cast<Method>(head.method);

        auto raw_buffer = ReadData(client_fd_, length);

        if (type == Method::PUT) {
            auto func = [this, raw_buffer = std::move(raw_buffer)] {
                auto& request = *reinterpret_cast<const PutRequest*>(raw_buffer.data());
                HandlePutRequest(request);
            };
            thread_pool_.AddTask(new base::Task(func));
        } else if (type == Method::GET) {
            auto func = [this, raw_buffer = std::move(raw_buffer)] {
                auto& request = *reinterpret_cast<const GetRequest*>(raw_buffer.data());
                HandleGetRequest(request);
            };
            thread_pool_.AddTask(new base::Task(func));
        }
    }
}

void DBServer::HandleGetRequest(const GetRequest& request) {
    /*** [TODO]实现功能: 处理 Get 请求
     * 提示：1. 从 request 中获取 db_name
     *      2. 检查 name2db_ 是否有对应的 db_name, 如果没有，将 response.state 置为 State::kInvDB
     *      3. 如果有，则调用对应 db 的 Get 读取数据
     *      4. 将 response 的发送到 client_fd_，注意，这里需考虑 response.key 和 response.value 如何发送
     *   @param request Get 请求信息(定义见 core/message.h)
    ***/
}

void DBServer::HandlePutRequest(const PutRequest& request) {
    const char* data = &request.body[0];
    std::string db_name(data, request.db_len);
    PutResponse response;
    auto iter = name2db_.find(db_name);
    if (static_cast<PutType>(request.put_type) == PutType::Create) {
        if (iter == name2db_.end()) {
            DBOptions options;
            options.db_name = db_name;
            auto db = new DB(options);
            name2db_.insert({db_name, db});
            response.state = static_cast<int8_t>(State::kOK);
        } else {
            response.state = static_cast<int8_t>(State::kInvDB);
        }
    } else {
        if (iter == name2db_.end()) {
            response.state = static_cast<int8_t>(State::kInvDB);
        } else {
            iter->second->Put(request, &response);
        }
    }
    write(client_fd_, (const void*)&response, sizeof(response));
}

}   // namespace qsdb

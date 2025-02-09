#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

#include "base/common.h"
#include "core/db_client.h"
#include "core/socket.h"
#include "core/message.h"

namespace qsdb {

DBClient::DBClient(const std::string& ip, int port) {
    sockfd_ = NetSocket::Socket();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    NetSocket::InetPton(AF_INET, ip.data(), &addr.sin_addr);

    NetSocket::Connect(sockfd_, (struct sockaddr*)&addr, sizeof(addr));
}

void DBClient::Get(const GetRequest& request, GetResponse* response) {
    /*** [TODO]实现功能: 发送 Get 请求
     * 提示：1. 设置 head，并发送 head 和 request
     *      2. 发送结束等待服务端响应，读取 response
     *   @param request Get 请求信息(定义见 core/message.h)
     *   @param response Get 请求信息(定义见 core/message.h)
    ***/
    MessageHead head;
    head.length = sizeof(int32_t) * 2 + request.db_len + request.key_len;
    head.method = static_cast<int8_t>(Method::GET);
}

void DBClient::Put(const PutRequest& request, PutResponse* response) {
    /*** [TODO]实现功能: 发送 Get 请求
     * 提示：1. 设置 head，并发送 head 和 request
     *      2. 发送结束等待服务端响应，读取 response
     *   @param request Put 请求信息(定义见 core/message.h)
     *   @param response Put 请求信息(定义见 core/message.h)
    ***/
    MessageHead head;
    head.length = sizeof(int32_t) * 3 +
            sizeof(int8_t) + request.db_len + request.key_len + request.value_len;
    head.method = static_cast<int8_t>(Method::PUT);
}

DBClient::~DBClient() {
    if (sockfd_ != Null) close(sockfd_);
}

}   // namespace qsdb

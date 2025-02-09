#pragma once

#include <vector>

enum struct Method : int8_t {
    GET,
    PUT
};

enum struct PutType : int8_t {
    Create,    // create new db
    Delete,    // delete key
    Update     // update key/value pair
};

enum struct State : int8_t {
    kOK,
    kInvDB,
    kInvKey
};

using IOBuf = std::vector<char>;

#pragma pack(1)

struct MessageHead {
    int32_t length;      // message length
    int8_t method;       // GET or PUT
};

struct GetRequest {
    int32_t db_len = 0;
    int32_t key_len = 0;
    char body[];
};

struct GetResponse {
    int8_t state;
    int32_t key_len = 0;
    int32_t value_len = 0;
    const char* key;
    const char* value;
};

struct PutRequest {
    int8_t put_type;    // for PutType
    int32_t db_len = 0;
    int32_t key_len = 0;
    int32_t value_len = 0;
    char body[];
};

struct PutResponse {
    int8_t state;
};

#pragma pack()

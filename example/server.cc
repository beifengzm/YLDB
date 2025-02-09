#include "core/db_server.h"

int main(int argc, char** argv) {
    qsdb::DBServerOptions options;
    options.port = 9999;
    options.backlog = 500;
    qsdb::DBServer db_server(options);

    db_server.Start();

    return 0;
}

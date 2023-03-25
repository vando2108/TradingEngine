// Copyright [2023] <Kdimo>
#include "../include/util/util.h"
#include "../include/http/http-server.h"

int main(int argc, char* argv[]) {
  Http::HttpServer server("127.0.0.1", 3000);
  if (server.start(1024) < 0) {
    errsys("failed to start server\n");
    return -1;
  }

  trace("server is running\n");

  for (;;) {
    char buff[1024];
    char* cmd = fgets(buff, sizeof(buff), stdin);

    if (cmd == nullptr) {
      break;
    }
  }

  trace("terminate...\n");

  return 0;
}


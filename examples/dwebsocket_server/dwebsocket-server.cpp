#include <dwebsocket_server>
#include <dwebsocket_client>
#include <iostream>
//#include "dlog.h"
#include <dmpacket>

int main()
{
    dws::DWebSocketClient client("ws://127.0.0.1:8888");
    dws::DWebSocketServer server("0.0.0.0",8888);
    server.run();
}
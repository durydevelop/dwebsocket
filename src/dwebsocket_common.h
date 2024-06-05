#ifndef DWebSocketCommon_H
#define DWebSocketCommon_H

#include <functional>
#include <cstdint>
#include <map>
#include <string>

namespace dws
{
    enum DWebSocketEventType { EVENT_OPEN, EVENT_CLOSE, EVENT_ERROR, EVENT_DATA_STR, EVENT_DATA_BIN, EVENT_PING, EVENT_PONG, EVENT_FRAGMENT};
    enum DWebSocketReadyState { STATE_CONNECTING, STATE_OPEN, STATE_CLOSING, STATE_CLOSED, STATE_TIMEOUT };

    //using DWebSocketHttpHeaders = std::map<std::string, std::string>; //, CaseInsensitiveLess>;
    typedef std::map<std::string, std::string> DWebSocketHttpHeaders;

    struct DConnectionBinder {
        std::string ID;
        std::string IP;
        int Port;
        DWebSocketHttpHeaders httpHeaders;
    };
}

#endif
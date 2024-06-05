#ifndef DWEBSOCKET_EM_H
#define DWEBSOCKET_EM_H

#ifdef EMSCRIPTEN
//#include <functional>
#include <string>
#include <emscripten/websocket.h>
#include "dwebsocket_base.h"

namespace dws
{
    //typedef void (*OnEmMessageCallback)(DWebSocketEventType EventType, uint8_t *data, unsigned int len);

    class DWebSocket : public DWebSocketBase {
        public:
            DWebSocket(std::string webSocketUrl);
            ~DWebSocket() override;
            
            bool close(void) override;
            bool stop(void) override;
            ReadyState getReadyState(void) override;
            void setUrl(std::string webSocketUrl) override;
            void setOnMessageCallback(OnWebSocketMessageCallback callbackFunc) override;
            // TODO void setAutoReconnect(bool enabled) override;

            bool start() override;
            bool isConnected(void) override;

            bool send(void) override;
            bool send(const std::string& data) override;
            bool send(const std::vector<uint8_t>& data) override;
            bool send(const char *data, size_t len) override;
            bool send(const uint8_t *data, size_t len) override;

            std::string getLastError(void) override;
            // TODO void setHttpHeaders(DWebSocketHttpHeaders httpExtraHeaders) override;
/*            
            typedef std::function<void (DWebSocketEventType EventType, uint8_t *data, unsigned int len)> OnWebSocketMessageCallback;

            // https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/readyState
            enum ReadyState { Connecting = 0, Open = 1, Closing = 2, Closed = 3 };
            
            DWebSocket(std::string webSocketUrl);
            ~DWebSocket();
            
            bool close(void);
            bool stop(void);
            ReadyState getReadyState(void);
            void setUrl(std::string webSocketUrl);
            void setOnMessageCallback(OnWebSocketMessageCallback callbackFunc);
            bool start();
            bool send(const std::string& data);
            bool sendBinary(const uint8_t data[], size_t len);

            std::string getLastError(void);
            OnWebSocketMessageCallback eventCallback;
*/
            OnWebSocketMessageCallback eventCallback; // Hides eventCallback in base class so, can be used in external This pointer (see .cpp)

        private:
            EmscriptenWebSocketCreateAttributes attr;
            EMSCRIPTEN_WEBSOCKET_T socket;
            EMSCRIPTEN_RESULT lastResult;
    };
}

#endif
#endif
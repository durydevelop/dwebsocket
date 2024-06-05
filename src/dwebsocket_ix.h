#ifndef DWEBSOCKET_IX_H
#define DWEBSOCKET_IX_H

#include <string>
#include "ixwebsocket/IXWebSocket.h"
#include "dwebsocket_base.h"

namespace dws
{
    class DWebSocket : public DWebSocketBase {
        public:
            DWebSocket(std::shared_ptr<ix::WebSocket> ixWebSocket);
            DWebSocket(std::string webSocketUrl);
            ~DWebSocket() override;
            
            bool close(void) override;
            bool stop(void) override;
            ReadyState getReadyState(void) override;
            void setUrl(std::string webSocketUrl) override;
            void setOnMessageCallback(OnWebSocketMessageCallback callbackFunc) override;
            void setAutoReconnect(bool enabled);

            bool start() override;
            bool isConnected(void) override;

            bool send(void) override;
            bool send(const std::string& data) override;
            bool send(const std::vector<uint8_t>& data) override;
            bool send(const char *data, size_t len) override;
            bool send(const uint8_t *data, size_t len) override;

            std::string getLastError(void) override;
            void setHttpHeaders(DWebSocketHttpHeaders httpExtraHeaders);

        private:
            void OnMessage(const ix::WebSocketMessagePtr& msg);
            std::shared_ptr<ix::WebSocket> ixWebsocket;
    };
}

#endif
#ifndef DWEBSOCKET_H
#define DWEBSOCKET_H

#include <string>
#include "dwebsocket_common.h"
#include "dmpacket"

namespace dws
{
    class DWebSocketBase {
        public:
            //typedef std::function<void (DWebSocketEventType EventType, uint8_t *data, unsigned int len)> OnWebSocketMessageCallback;
            typedef std::function<void (DWebSocketEventType EventType, DMPacket& packetBuffer)> OnWebSocketMessageCallback;

            // https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/readyState
            enum ReadyState { Connecting = 0, Open = 1, Closing = 2, Closed = 3 };
            
            //DWebSocket(std::string webSocketUrl);
            virtual ~DWebSocketBase() = default;
            
            virtual bool close(void) = 0;
            virtual bool stop(void) = 0;
            virtual ReadyState getReadyState(void) = 0;
            virtual void setUrl(std::string webSocketUrl) = 0;
            virtual void setOnMessageCallback(OnWebSocketMessageCallback callbackFunc) = 0;
            // TODO? virtual void setAutoReconnect(bool enabled) = 0;
            virtual bool start() = 0;
            virtual bool isConnected(void) = 0;

            virtual bool send(void) = 0;
            virtual bool send(const std::string& data) = 0;
            virtual bool send(const std::vector<uint8_t>& data) = 0;
            virtual bool send(const char *data, size_t len) = 0;
            virtual bool send(const uint8_t *data, size_t len) = 0;
            //TODO sendUtf8String()

            virtual std::string getLastError(void) = 0;
            // TODO? virtual void setHttpHeaders(DWebSocketHttpHeaders httpExtraHeaders) = 0;

            DMPacket inputBuffer;
            DMPacket outputBuffer;

        protected:
            OnWebSocketMessageCallback eventCallback;
            virtual void setError(const std::string& error) = 0;
            
            std::string url;
            std::string lastError;
    };
}

#endif
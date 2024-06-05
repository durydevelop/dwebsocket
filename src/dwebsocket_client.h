#ifndef WEBSOCKET_CLIENT_EM_H
#define WEBSOCKET_CLIENT_EM_H

#include <dwebsocket_endpoint>
#include <dwebsocket>

namespace dws
{
    class DWebSocketClient {
        public:
            //typedef std::function<void (DWebSocketEventType EventType, uint8_t *data, unsigned int len)> OnClientEventCallback;
            typedef std::function<void (DWebSocketEventType EventType, DMPacket& dataPacket)> OnClientEventCallback;
            
            DWebSocketClient(std::string webSocketUrl);
            ~DWebSocketClient();

            bool close(void);
            void setUrl(std::string webSocketUrl);
            void setOnEventCallback(OnClientEventCallback callbackEventFunc);
            // TODO? void setAutoReconnect(bool enabled);

            bool connect();
            bool connect(std::string webSocketUrl);

            bool send(void);
            //bool send(const std::string& data);
            //bool send(const char *data, size_t len);

            bool isConnected(void);
            bool isClosed(void);
            DWebSocket::ReadyState getStatus(void);
            std::string getLastError(void);

            std::string getVersionInfo(void);

            DMPacket& getInputBuffer(void);
            DMPacket& getOutputBuffer(void);
            std::weak_ptr<DWebSocket> getWebSocket(void);

            OnClientEventCallback eventCallback;

        private:
            void onWebSocketMessage(DWebSocketEventType EventType, DMPacket& packetBuffer);

            std::string url;
            //std::shared_ptr<DEndPoint> endPoint;
            std::shared_ptr<DWebSocket> webSocket;
    };
}

#endif
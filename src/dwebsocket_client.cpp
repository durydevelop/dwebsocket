#include "dwebsocket_client.h"
#include <dwebsocket_log>

namespace dws
{
    const char TAG[7]="Client";

    DWebSocketClient::DWebSocketClient(std::string webSocketUrl) {
        Log::debug(TAG,"DWebSocketClient()");
        if (!webSocketUrl.empty()) {
            url=webSocketUrl;
        }
        webSocket=std::shared_ptr<DWebSocket>(new DWebSocket(url));
        //inputBuffer=std::shared_ptr<DMPacket>(new DMPacket());
        //outputBuffer=std::shared_ptr<DMPacket>(new DMPacket());
    }

    /**
     * @brief Destroy the Web Socket object.
     * 
     */
    DWebSocketClient::~DWebSocketClient() {
        Log::debug(TAG,"~DWebSocketClient()");
        close();
        webSocket.reset();
    }

    /**
     * @brief Close current websocket (blocking operation).
     * 
     * @return true on close success.
     * @return false if closing timeout reached (TODO).
     */
    bool DWebSocketClient::close(void) {
        if (webSocket->getReadyState() == DWebSocket::ReadyState::Open) {
            Log::debug(TAG,"Stopping socket...");
            webSocket->stop();
            Log::debug(TAG,"...stopped");
            // TODO: implement timeout
        }
        return true;
    }

    /**
     * @brief Set entire websocket url string.
     * N.B. This function only set url string, does not start any connection.
     * 
     * @param webSocketUrl  ->  Url string that must be in ws://host:port form, eg.:"ws://localhost:8088/".
     */
    void DWebSocketClient::setUrl(std::string webSocketUrl) {
        url=webSocketUrl;
    }

    /**
     * @brief Set callback for any web socket event.
     * 
     * @param callbackMessageFunc   ->  Callback function like this:
     * void onWebSocketEvent((dws::DWebSocketEventType type, uint8_t *data, size_t len);
     */
    void DWebSocketClient::setOnEventCallback(OnClientEventCallback callbackEventFunc) {
        eventCallback=callbackEventFunc;
    }

    //void DWebSocketClient::setAutoReconnect(bool enabled) {
    //    webSocket->setAutoReconnect(enabled);
    //}

    /**
     * @brief Connect to websocket url stored in #url.
     * 
     * @return:
     * false on errors.
     * true if start() is called, so, does not mean connection success but only connection started.
     */
    bool DWebSocketClient::connect() {
        if (url.empty()) {
            Log::error(TAG,"Cannot connect, url is not set");
            return false;
        }
        Log::debug(TAG,"Try to connecting to %s",url.c_str());

        if (webSocket->getReadyState() == DWebSocket::ReadyState::Open) {
            Log::debug(TAG,"Socket is open, closing...");
            if (webSocket->stop()) {
                Log::debug(TAG,"...closed");
            }
            else {
                Log::error("...not closed: ",webSocket->getLastError().c_str());
            }
        }

        // Update url
        webSocket->setUrl(url);
        
        // Bind callback
        auto callback=std::bind(&DWebSocketClient::onWebSocketMessage, this, std::placeholders::_1, std::placeholders::_2);
        webSocket->setOnMessageCallback(callback);

        // Start web socket connection
        webSocket->start();

        return true;
    }

    std::string DWebSocketClient::getVersionInfo(void) {
        return("DWebSocket Client ver. TODO"); /// @todo DWEBSOCKET_VERSION);
    }

    void DWebSocketClient::onWebSocketMessage(DWebSocketEventType EventType, DMPacket& packetBuffer) {
        if (eventCallback) {
                eventCallback(EventType,packetBuffer);
            }
    }

    /**
     * @brief Connect to a specific url.
     * 
     * @param webSocketUrl  ->  Url string that must be in ws://host:port form, eg.:"ws://localhost:8088/".
     * @return true on sucessfully connect otherwise false.
     */
    bool DWebSocketClient::connect(std::string webSocketUrl) {
        setUrl(webSocketUrl);
        return connect();
    }

    /**
     * @brief Send output buffer content.
     * 
     * @return true 
     * @return false 
     */
    bool DWebSocketClient::send(void) {
        return webSocket->send();
    }

    /**
     * @brief Send a string message.
     * 
     * @param data  ->  string to send.
     * 
     * @return true on success otherwise false.
     */
//    bool DWebSocketClient::send(const std::string& data) {
//        webSocket->send(data);
//        return true;
//    }

    /**
     * @brief Send a data buffer.
     * 
     * @param data  ->  buffer containing bytes of data.
     * @param len   ->  buffer lenght.
     * @return true on success otherwise false.
     */
//    bool DWebSocketClient::send(const char *data, size_t len) {
//        return (webSocket->send(data,len));
//    }

    bool DWebSocketClient::isConnected(void) {
        return webSocket->getReadyState() == DWebSocket::ReadyState::Open;
    }

    bool DWebSocketClient::isClosed(void) {
        return webSocket->getReadyState() == DWebSocket::ReadyState::Closed;
    }

    DWebSocket::ReadyState DWebSocketClient::getStatus(void) {
        return webSocket->getReadyState();
    }

    std::string DWebSocketClient::getLastError(void) {
        return webSocket->getLastError();
    }

    DMPacket& DWebSocketClient::getInputBuffer(void) {
        return webSocket->inputBuffer;
    }

    DMPacket& DWebSocketClient::getOutputBuffer(void) {
        return webSocket->outputBuffer;
    }

    std::weak_ptr<DWebSocket> DWebSocketClient::getWebSocket(void) {
        return webSocket;
    }
}
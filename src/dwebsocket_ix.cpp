#include "dwebsocket_ix.h"
#include <dwebsocket_log>

namespace dws
{
    const char TAG[10]="WebSocket";
    
    //OnMessageCallback eventCallback;

    /**
     * @brief Construct a new Web Socket object.
     * 
     */
    DWebSocket::DWebSocket(std::string webSocketUrl) {
        Log::debug(TAG,"DWebSocket()");
        if (!webSocketUrl.empty()) {
            url=webSocketUrl;
        }
    }

    DWebSocket::DWebSocket(std::shared_ptr<ix::WebSocket> newIxWebSocket) {
        Log::debug(TAG,"DWebSocket()");
        ixWebsocket=newIxWebSocket;
    }

    /**
     * @brief Destroy the Web Socket object.
     * 
     */
    DWebSocket::~DWebSocket() {
        Log::debug(TAG,"~DWebSocket()");
        stop();
    }

    bool DWebSocket::isConnected(void) {
        return getReadyState() == DWebSocket::ReadyState::Open;
    }

    /**
     * @brief Asyncronous
     * 
     * @return true 
     * @return false 
     */
    bool DWebSocket::close(void) {
        Log::debug(TAG,"close()");
        ixWebsocket->close();
        return true;
    }

    // Syncronous
    bool DWebSocket::stop(void) {
        if (ixWebsocket) {
            if (ixWebsocket->getReadyState() == ix::ReadyState::Open) {
                Log::debug(TAG,"Stopping...");
                ixWebsocket->stop();
                Log::debug(TAG,"...stopped");
                // TODO: implement timeout
            }
        }
        return true;
    }

    DWebSocket::ReadyState DWebSocket::getReadyState(void) {
        if (ixWebsocket) {
            return (ReadyState) ixWebsocket->getReadyState();
        }
        return ReadyState::Closed;
    }

    /**
     * @brief Set websocket url.
     * N.B. This function only set url string, does not start any connection.
     * 
     * @param webSocketUrl  ->  Url string that must be in ws://host:port form, eg.:"ws://localhost:8088/".
     */
    void DWebSocket::setUrl(std::string webSocketUrl) {
        url=webSocketUrl;
    }

    /**
     * @brief Set callback for any web socket event.
     * 
     * @param callbackMessageFunc   ->  Callback function like this:
     * void onWebSocketEvent(dws::DWebSocketEventType type, uint8_t *data, size_t len);
     */
    void DWebSocket::setOnMessageCallback(OnWebSocketMessageCallback callbackFunc) {
        eventCallback=callbackFunc;
    }

    bool DWebSocket::start() {
        if (url.empty()) {
            lastError="Cannot connect, url is not set";
            Log::error(TAG,lastError.c_str());
            return false;
        }
        Log::debug(TAG,"Starting websocket for %s",url.c_str());

        if (ixWebsocket) {
            if (ixWebsocket->getReadyState() == ix::ReadyState::Open) {
                Log::debug(TAG,"Socket is open, closing...");
                ixWebsocket->stop();
                Log::debug(TAG,"...closed");
            }
            ixWebsocket.reset();
        }

        //ixWebsocket=std::shared_ptr<ix::WebSocket>(new ix::WebSocket());
        ixWebsocket=std::make_shared<ix::WebSocket>();
        // Optional heart beat, sent every 45 seconds when there is not any traffic to make sure that load balancers do not kill an idle connection.
        ixWebsocket->setPingInterval(45);
        // Per message deflate connection is enabled by default. You can tweak its parameters or disable it
        ixWebsocket->disablePerMessageDeflate();
        // Disable automatic reconnection by default
        ixWebsocket->disableAutomaticReconnection();

        // Update url
        ixWebsocket->setUrl(url);
        
        // Bind callback
        ixWebsocket->setOnMessageCallback(std::bind(&DWebSocket::OnMessage, this, std::placeholders::_1));

        // Create new socket and connect
        ixWebsocket->close();
        ixWebsocket->start();

        return true;
    }

    /**
     * @brief Send data from output buffer.
     * N.B.
     * outputBuffer is cleared on success.
     * 
     * @return true 
     * @return false 
     */
    bool DWebSocket::send(void) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }
        auto info=ixWebsocket->sendBinary(ix::IXWebSocketSendData(outputBuffer.buffer()));
        if (!info.success) {
            lastError="Error sending output buffer";
        }
        else {
            outputBuffer.clear();
        }
        return info.success;
    }

    /**
     * @brief Send a string message.
     * 
     * @param data  ->  string to send.
     * 
     * @return true on success otherwise false.
     */
    bool DWebSocket::send(const std::string& data) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }
        auto info=ixWebsocket->send(data);
        if (!info.success) {
            lastError="Error sending output buffer";
        }
        return info.success;
    }

    bool DWebSocket::send(const std::vector<uint8_t>& data) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }
        auto info=ixWebsocket->sendBinary(ix::IXWebSocketSendData(data));
        if (!info.success) {
            lastError="Error sending output buffer";
        }
        return info.success;
    }

    /**
     * @brief Send a char data buffer.
     * 
     * @param data  ->  buffer containing bytes of data.
     * @param len   ->  buffer lenght.
     * @return true on success otherwise false.
     */
    bool DWebSocket::send(const char *data, size_t len) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }
        auto info=ixWebsocket->sendBinary(ix::IXWebSocketSendData(data,len));
        if (!info.success) {
            lastError="Error sending output buffer";
        }
        return info.success;
    }

    /**
     * @brief Send a bytes data buffer.
     * 
     * @param data  ->  buffer containing bytes of data.
     * @param len   ->  buffer lenght.
     * @return true on success otherwise false.
     */
    bool DWebSocket::send(const uint8_t *data, size_t len) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }
        auto d=std::vector<uint8_t>(&data[0],&data[len-1]);
        auto info=ixWebsocket->sendBinary(ix::IXWebSocketSendData(d));
        if (!info.success) {
            lastError="Error sending output buffer";
        }
        return info.success;
    }

    std::string DWebSocket::getLastError(void) {
        return lastError;
    }

    void DWebSocket::setHttpHeaders(DWebSocketHttpHeaders httpExtraHeaders) {
        ix::WebSocketHttpHeaders headers;
        for (auto header : httpExtraHeaders) {
            headers.insert(header);
        }
        ixWebsocket->setExtraHeaders(headers);
    }

    void DWebSocket::setAutoReconnect(bool enabled) {
        if (enabled) {
            ixWebsocket->enableAutomaticReconnection();
        }
        else {
            ixWebsocket->disableAutomaticReconnection();
        }
    }

    void DWebSocket::OnMessage(const ix::WebSocketMessagePtr& msg) {
        inputBuffer.setBuffer((uint8_t *)msg->str.data(),msg->str.size());
        if (msg->type == ix::WebSocketMessageType::Open) {
            Log::debug(TAG,"OPEN");
            if (eventCallback) {
                //eventCallback(DWebSocketEventType::EVENT_OPEN,nullptr,0);
                eventCallback(DWebSocketEventType::EVENT_OPEN,inputBuffer);
            }
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
            Log::debug(TAG,"CLOSE");
            if (eventCallback) {
                //eventCallback(DWebSocketEventType::EVENT_CLOSE,(uint8_t*)msg->closeInfo.reason.data(),msg->closeInfo.reason.size());
                eventCallback(DWebSocketEventType::EVENT_CLOSE,inputBuffer);
            }
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            Log::debug(TAG,"ERROR");
            lastError=msg->errorInfo.reason;
            if (eventCallback) {
                //eventCallback(DWebSocketEventType::EVENT_ERROR,(uint8_t*)msg->errorInfo.reason.data(),msg->errorInfo.reason.size());
                eventCallback(DWebSocketEventType::EVENT_ERROR,inputBuffer);
            }
        }
        else if (msg->type == ix::WebSocketMessageType::Message) {
            Log::debug(TAG,"MESSAGE");
            if (eventCallback) {
                if (msg->binary) {
                    //eventCallback(DWebSocketEventType::EVENT_DATA_BIN,(uint8_t*)msg->str.data(),msg->str.size());    
                    eventCallback(DWebSocketEventType::EVENT_DATA_BIN,inputBuffer);    
                }
                else {
                    //eventCallback(DWebSocketEventType::EVENT_DATA_STR,(uint8_t*)msg->str.data(),msg->str.size());
                    eventCallback(DWebSocketEventType::EVENT_DATA_STR,inputBuffer);
                }
            }
        }
    }
}

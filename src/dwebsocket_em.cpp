#include "dwebsocket_em.h"

#ifdef EMSCRIPTEN
#include "dwebsocket_log.h"
#include <map>

namespace dws
{
    std::map<EMSCRIPTEN_RESULT,std::string> EMErrors = {
        { EMSCRIPTEN_RESULT_SUCCESS,    "EMSCRIPTEN_RESULT_SUCCESS" },
        { EMSCRIPTEN_RESULT_DEFERRED,   "EMSCRIPTEN_RESULT_DEFERRED" },
        { EMSCRIPTEN_RESULT_NOT_SUPPORTED,  "EMSCRIPTEN_RESULT_NOT_SUPPORTED" },
        { EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED, "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED" },
        { EMSCRIPTEN_RESULT_INVALID_TARGET, "EMSCRIPTEN_RESULT_INVALID_TARGET" },
        { EMSCRIPTEN_RESULT_UNKNOWN_TARGET, "EMSCRIPTEN_RESULT_UNKNOWN_TARGET" },
        { EMSCRIPTEN_RESULT_INVALID_PARAM, "EMSCRIPTEN_RESULT_INVALID_PARAM" },
        { EMSCRIPTEN_RESULT_FAILED, "EMSCRIPTEN_RESULT_FAILED" },
        { EMSCRIPTEN_RESULT_NO_DATA, "EMSCRIPTEN_RESULT_NO_DATA" },
        { EMSCRIPTEN_RESULT_TIMED_OUT, "EMSCRIPTEN_RESULT_TIMED_OUT" }
    };
    
    const char TAG[10]="WebSocket";
    
    // Callback functions definitions
    EM_BOOL WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData);
    EM_BOOL WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData);
    EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData);
    EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData);

    // Pointer to this for accessing member callback
    DWebSocket *This=nullptr;

    /**
     * @brief Construct a new Web Socket object.
     * 
     */
    DWebSocket::DWebSocket(std::string webSocketUrl) {
        if (!emscripten_websocket_is_supported()) {
            Log::debug(TAG,"WebSockets are not supported, cannot continue!");
            exit(1);
        }

        This=this;
        socket=0;
        // Setup web socket attributes
	    emscripten_websocket_init_create_attributes(&attr);
        //attr.protocols = "binary,base64";
        attr.createOnMainThread=true;

        if (!webSocketUrl.empty()) {
            setUrl(webSocketUrl);
        }
    }

    /**
     * @brief Destroy the Web Socket object.
     * 
     */
    DWebSocket::~DWebSocket() {
        stop();
    }

    // For compatibility with IXWebSocket (but syncronous)
    bool DWebSocket::close(void) {
        return stop();
    }
	
	// Syncronous
    bool DWebSocket::stop(void) {
        lastResult=emscripten_websocket_close(socket, 0, 0);
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            //Log::error(TAG,"Cannot close websocket. Error:%i",lastResult);
            return false;
        }

        lastResult=emscripten_websocket_delete(socket);
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            //Log::error(TAG,"Cannot delete websocket. Error:%i",lastResult);
            return false;
        }

        socket=0;
        return true;
    }

    bool DWebSocket::isConnected(void) {
        return getReadyState() == DWebSocket::ReadyState::Open;
    }

    DWebSocket::ReadyState DWebSocket::getReadyState(void) {
        if (!socket) {
            return ReadyState::Closed;
        }
        unsigned short state;
        lastResult=emscripten_websocket_get_ready_state(socket,&state);
        //Log::debug(TAG,"state=%d",state);
        return ((ReadyState) state);
    }
/*
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
*/
    /**
     * @brief Set websocket url.
     * N.B. This function only set url string, does not start any connection.
     * 
     * @param webSocketUrl  ->  Url string that must be in ws://host:port form, eg.:"ws://localhost:8088/".
     */
    void DWebSocket::setUrl(std::string webSocketUrl) {
        url=webSocketUrl;
        attr.url=url.c_str();
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
        Log::debug(TAG,"Starting...");
        Log::debug(TAG,"socket=%d",socket);
        if (url.empty()) {
            Log::error(TAG,"Url not set");
            return false;
        }

        if (socket) {
            // Try to free current socket
            Log::debug(TAG,"Socket is open...closing...");
            if (!stop()) {
                return false;
            }
        }

        // Create new socket and connect
        socket = emscripten_websocket_new(&attr);
        Log::debug(TAG,"socket=%d",socket);
        if (socket <= 0) {
            Log::error(TAG,"WebSocket creation failed, error code %d!\n", (EMSCRIPTEN_RESULT)socket);
            return false;
        }

        // bind emscripten callbacks to this socket
        emscripten_websocket_set_onopen_callback(socket, (void*)42, WebSocketOpen);
        emscripten_websocket_set_onclose_callback(socket, (void*)43, WebSocketClose);
        emscripten_websocket_set_onerror_callback(socket, (void*)44, WebSocketError);
        emscripten_websocket_set_onmessage_callback(socket, (void*)45, WebSocketMessage);

        Log::debug(TAG,"...started");
            
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

        lastResult=emscripten_websocket_send_binary(socket,outputBuffer.rawBuffer(), outputBuffer.size());
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            lastError="Error sending output buffer";
            return false;
        }

        outputBuffer.clear();
        return true;
    }

    bool DWebSocket::send(const std::string& data) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }

        lastResult=emscripten_websocket_send_binary(socket,(void *)data.data(), data.size());
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            lastError="Error sending string data";
            return false;
        }

        return true;
    }

    bool DWebSocket::send(const std::vector<uint8_t>& data) {
        if (!isConnected() ) {
            lastError="Web socket is not opened";
            Log::error(TAG,lastError.c_str());
        }

        lastResult=emscripten_websocket_send_binary(socket,(void *)data.data(), data.size());
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            lastError="Error sending vector data";
            return false;
        }
        
        return true;
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

        lastResult=emscripten_websocket_send_binary(socket,(void *)data, len);
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            lastError="Error sending char data";
            return false;
        }

        return true;
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

        lastResult=emscripten_websocket_send_binary(socket,(void *)data, len);
        if (lastResult != EMSCRIPTEN_RESULT_SUCCESS) {
            lastError="Error sending char data";
            return false;
        }

        return true;
    }

    std::string DWebSocket::getLastError(void) {
        return EMErrors[lastResult];
    }

//***************************************************************************************************************
// Non class member
//***************************************************************************************************************

    EM_BOOL WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData) {
        Log::debug(TAG,"WebSockeConnect: eventType=%d, socket=%d, userData=%ld", eventType, e->socket, (long)userData);
        if (This->eventCallback) {
            This->inputBuffer.clear();
            This->eventCallback(DWebSocketEventType::EVENT_OPEN, This->inputBuffer);
        }
        return 0;
    }

    EM_BOOL WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData) {
        Log::debug(TAG,"WebSockeClose: eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%ld", eventType, e->wasClean, e->code, e->reason, (long)userData);
        if (This->eventCallback) {
            This->inputBuffer.setBuffer(e->reason,strlen(e->reason));
            This->eventCallback(DWebSocketEventType::EVENT_CLOSE, This->inputBuffer); //(uint8_t *)e->reason, strlen(e->reason));
        }
        return 0;
    }

    EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData) {
        Log::debug(TAG,"WebSocketError: eventType=%d, userData=%ld, socket=%d", eventType, (long)userData, e->socket);
        if (This->eventCallback) {
            This->inputBuffer.clear();
            This->eventCallback(DWebSocketEventType::EVENT_ERROR, This->inputBuffer);
        }
        return 0;
    }

    EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData) {
        Log::debug(TAG,"WebSocketMessage: eventType=%d, userData=%ld, data=%p, numBytes=%d, isText=%d", eventType, (long)userData, e->data, e->numBytes, e->isText);
        if (This->eventCallback) {
            This->inputBuffer.setBuffer(e->data,e->numBytes);
            if (e->isText) {
                This->eventCallback(DWebSocketEventType::EVENT_DATA_STR,This->inputBuffer);
            }
            else {
                This->eventCallback(DWebSocketEventType::EVENT_DATA_BIN,This->inputBuffer);
            }
        }
        return 0;
    }
}
#endif
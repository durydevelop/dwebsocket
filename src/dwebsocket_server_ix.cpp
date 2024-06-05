#include "dwebsocket_server_ix.h"
#include <dwebsocket_log.h>

#define TAG "Server"

namespace dws
{
    /**
     * @brief Create the web server (does not run it).
     * 
     */
    DWebSocketServer::DWebSocketServer(const std::string& hostName, size_t listenPort) {
        host=hostName;
        port=listenPort;
        server=std::make_shared<ix::WebSocketServer>(port,host);
    }

    /**
     * @brief Stop server and destroy object.
     * 
     */
    DWebSocketServer::~DWebSocketServer() {
        stop();
    }

    /**
     * @brief Set callback for any web server event.
     * 
     * @param callbackEventFunc   ->  Callback function with signature void (void (DWebSocketEventType, std::shared_ptr<DEndPoint>)
     */
    void DWebSocketServer::setOnEventCallback(OnServerEventCallback callbackEventFunc) {
        eventCallback=callbackEventFunc;
    }

    bool DWebSocketServer::start(void) {
        server->setOnConnectionCallback([this](std::weak_ptr<ix::WebSocket> ixWebSocket, std::shared_ptr<ix::ConnectionState> connState) {
            auto ws=ixWebSocket.lock();
            ws->setOnMessageCallback(std::bind(&DWebSocketServer::OnMessage, this, ixWebSocket, connState, std::placeholders::_1));
        });
        Log::debug(TAG,"Start listening on port %d",server->getPort());
        bool ret=server->listenAndStart();
        if (ret) {
            Log::debug(TAG,"Started");
        }
        else {
            Log::error(TAG,"Cannot start web-socket server");
        }
        return ret;
    }

    void DWebSocketServer::OnMessage(std::weak_ptr<ix::WebSocket> ixWebSocket, std::shared_ptr<ix::ConnectionState> connState, const ix::WebSocketMessagePtr &msg) {
        std::string connID=connState->getId();
        if (msg->type == ix::WebSocketMessageType::Open) {
                Log::debug(TAG,"NEW connection from ip: %s ",connState->getRemoteIp().c_str());
                Log::debug(TAG,"Conn id: %s",connID.c_str());
                Log::debug(TAG,"Conn used Uri: %s",msg->openInfo.uri.c_str());
                Log::debug(TAG,"Conn headers:");
                for (auto it : msg->openInfo.headers) {
                    Log::debug(TAG,"\t %s: %s",it.first.c_str(),it.second.c_str());
                }

                // Create a DConnectionBinder
                DConnectionBinder connBinder;
                connBinder.ID=connState->getId();
                connBinder.IP=connState->getRemoteIp();
                connBinder.Port=connState->getRemotePort();
                for (auto it : msg->openInfo.headers) {
                    connBinder.httpHeaders.insert(it);
                }

                // Wrap websocket
                auto dWebSocket=std::shared_ptr<DWebSocket>(new DWebSocket(ixWebSocket.lock()));

                // Create endpoint on websocket
                auto newEndpoint=std::shared_ptr<DEndPoint>(new DEndPoint(std::move(dWebSocket)));

                // Add connection binder
                newEndpoint->setConnectionBinder(std::move(connBinder));

                // Add to endpoints list
                std::lock_guard<std::mutex> lock(epMutex);
                endpoints.emplace(std::make_pair(newEndpoint->getID(),std::move(newEndpoint)));
                Log::debug(TAG,"Now active endpoints: %d",endpoints.size());

                if (eventCallback) {
                    // Do callback
                    eventCallback(EVENT_OPEN,endpoints.at(connID));
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Close) {
                if (endpoints.contains(connID)) {
                    Log::debug(TAG,"Conn id: %s CLOSE Reason: %s",connID.c_str(), msg->closeInfo.reason.c_str());
                    // Get end-point
                    auto endPoint=endpoints.at(connID);
                    endPoint->getInputBuffer().setBuffer(msg->closeInfo.reason.data(),msg->closeInfo.reason.size());
                    // First: do callback
                    eventCallback(EVENT_CLOSE,endPoint);
                    // Then: remove client from list
                    Log::debug(TAG,"Endpoint %s deleting",connID.c_str());
                    std::lock_guard<std::mutex> lock(epMutex);
                    endpoints.erase(connID);
                    Log::debug(TAG,"Endpoint %s deleted",connID.c_str());
                    Log::debug(TAG,"Now active endpoints: %d",endpoints.size());
                }
                else {
                    Log::debug(TAG,"Client: %s tried to send CLOSE but it is not in clients list",connID.c_str());
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Error) {
                Log::debug(TAG,"ERROR");
                if (endpoints.contains(connID)) {
                    Log::debug(TAG,"Conn id: %s ERROR Msg:",connID.c_str(),msg->errorInfo.reason.c_str());
                    // Get end-point
                    auto endPoint=endpoints.at(connID);
                    endPoint->getInputBuffer().setBuffer(msg->errorInfo.reason.data(),msg->errorInfo.reason.size());
                    eventCallback(EVENT_ERROR,endPoint);
                }
                else {
                    Log::debug(TAG,"Client %s sent error but it is not in clients list",connID.c_str());
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Ping) {
                if (endpoints.contains(connID)) {
                    Log::debug(TAG,"Conn id: %s PING Msg: %s",connID.c_str(),msg->str.c_str());
                    // Get end-point
                    auto endPoint=endpoints.at(connID);
                    endPoint->getInputBuffer().setBuffer(msg->str.data(),msg->str.size());
                    eventCallback(EVENT_PING,endPoint);
                }
                else {
                    Log::debug(TAG,"Client: %s tried to send PING but it is not in clients list",connID.c_str());
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Fragment) {
                if (endpoints.contains(connID)) {
                    Log::debug(TAG,"Conn id: %s FRAGMENT Msg: %s",connID.c_str(),msg->str.c_str());
                    // Get end-point
                    auto endPoint=endpoints[connID];
                    // Set data
                    endPoint->getInputBuffer().setBuffer(msg->str.data(),msg->str.size());
                    // Do callback
                    eventCallback(EVENT_FRAGMENT,endPoint);
                }
                else {
                    Log::debug(TAG,"Conn id: %s tried to send FRAGMENT but it is not in clients list",connID.c_str());
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Message) {
                if (endpoints.contains(connID)) {
                    // Get end-point
                    auto endPoint=endpoints.at(connID);
                    if (msg->binary) {
                        // Set data
                        endPoint->getInputBuffer().setBuffer(msg->str.data(),msg->str.size());
                        Log::debug(TAG,"Conn id: %s Binary Data: %s",connID.c_str(),endPoint->getInputBuffer().toHexString().c_str());
                        // Do callback
                        eventCallback(EVENT_DATA_BIN,endPoint);
                    }
                    else {
                        // Set data
                        endPoint->getInputBuffer().setBuffer(msg->str.data(),msg->str.size());
                        Log::debug(TAG,"Conn id: %s String Data: %s",connID.c_str(),msg->str.c_str());
                        // Do callback
                        eventCallback(EVENT_DATA_STR,endPoint);
                    }
                }
                else {
                    Log::debug(TAG,"Conn id: %s tried to send MESSAGE but it is not in clients list",connID.c_str());
                }
            }
    }

    bool DWebSocketServer::run(void) {
        if (!start()) {
            return false;
        }
        // TODO: timeout

        join();
        return true;
    }

    void DWebSocketServer::join(void) {
        Log::debug(TAG,"Now in blocking mode...waiting to exit");
        server->wait();
        Log::debug(TAG,"Wait loop finished...normal exit");
    }

    void DWebSocketServer::stop(void) {
        server->stop();
    }

    size_t DWebSocketServer::getClientsCount(void) {
        return server->getClients().size();
    }

    std::string DWebSocketServer::getVersionInfo(void) {
        return("DWebSocket Server ver. TODO"); /// @todo DWEBSOCKET_VERSION);
    }

    void DWebSocketServer::closeClient(std::string connID) {
        if (endpoints.contains(connID)) {
            endpoints.at(connID)->closeSocket();
        }
    }

    bool DWebSocketServer::hasClient(std::string connID) {
        return endpoints.contains(connID);
    }
}

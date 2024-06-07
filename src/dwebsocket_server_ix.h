#ifndef DWebSocketServer_IX_H
#define DWebSocketServer_IX_H

#ifndef EMSCRIPTEN

#include <ixwebsocket/IXWebSocketServer.h>
#include <dwebsocket_common.h>
#include <dmpacket>
#include <dwebsocket_endpoint>

namespace dws
{
    

    //! Callback function for web server events.
    typedef std::function<void (DWebSocketEventType, std::shared_ptr<DEndPoint>)> OnServerEventCallback;
    //! Callback function for data progress.
    typedef std::function<bool(int current, int total)> OnProgressCallback;

    /**
     * @brief Web socket server class that wraps WebSocketServer from IXWebSocket library https://github.com/machinezone/IXWebSocket/tree/master
     */
    class DWebSocketServer { //: public std::enable_shared_from_this<DWebSocketServer> {
        public:
            DWebSocketServer(const std::string& hostName, size_t listenPort);
            ~DWebSocketServer();

            void setOnEventCallback(OnServerEventCallback callbackEventFunc);

            bool start(void);
            bool run(void);
            void join();
            void stop(void);

            size_t getClientsCount(void);
            std::string getVersionInfo(void);

            void closeClient(std::string clientID);
            //void setTimeout(size_t DelayMills, std::string endpointID);
            bool hasClient(std::string clientID);
            //void abortTimeout(std::string clientID);

            std::map<std::string,std::shared_ptr<dws::DEndPoint>> endpoints;

        private:
            std::mutex epMutex;
            std::string host;
            size_t port;
            std::shared_ptr<ix::WebSocketServer> server;
            OnServerEventCallback eventCallback;

            void OnMessage(std::weak_ptr<ix::WebSocket> ixWebSocket, std::shared_ptr<ix::ConnectionState> connState, const ix::WebSocketMessagePtr &msg);
    };
}

#endif

#endif
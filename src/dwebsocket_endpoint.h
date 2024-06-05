#ifndef DEndPoint_H
#define DEndPoint_H

//#include <memory>
#include <dmpacket>
#include <dwebsocket>

namespace dws {
    /**
     * @brief Rappresent a client connection endpoint.
     * Contains:
     * - webSocket (as DWebSocket)          ->  provide websocket connection layer.
     * - connBinder (as DConnectionBinder)  ->  provide connection information (id, ip, headers, etc).
     * - inputPacket (as DMPacket)          ->  provide advanced buffer for store and extract data.
     */
    class DEndPoint {
        public:
            explicit DEndPoint(std::shared_ptr<DWebSocket> emWebSocket);
            ~DEndPoint();

            void setConnectionBinder(DConnectionBinder connectionBinder);
            bool send(void);
            void closeSocket();
            std::string getID(void);
            std::string getIP(void);
            int getPort(void);
            DWebSocketHttpHeaders getHttpHeaders(void);
            void getInfo(void);
            std::string getLastError(void);
            DMPacket& getInputBuffer(void);
            DMPacket& getOutputBuffer(void);
            std::weak_ptr<DWebSocket> getWebSocket(void);
            
            //std::shared_ptr<DMPacket> inputBuffer; // Input buffer
            //std::shared_ptr<DMPacket> outputBuffer; // Output buffer

        private:

            std::shared_ptr<DWebSocket> webSocket;
            DConnectionBinder connBinder; // used by DWebSocketServer
            
            
            friend class DWebSocketServer; // Only DWebSocketServer can access to inputPacket
    };
}

#endif
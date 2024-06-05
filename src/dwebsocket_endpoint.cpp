#include "dwebsocket_endpoint.h"
#include <dwebsocket_log>

namespace dws
{
    const char TAG[9]="EndPoint";

    DEndPoint::DEndPoint(std::shared_ptr<DWebSocket> emWebSocket) {
        Log::debug(TAG,"DEndPoint()");
        webSocket=emWebSocket;
        //clearConnectionBinder();
        //inputBuffer=std::shared_ptr<DMPacket>(new DMPacket());
        //outputBuffer=std::shared_ptr<DMPacket>(new DMPacket());
    }

    DEndPoint::~DEndPoint() {
        Log::debug(TAG,"~DEndPoint()");
    }

    void DEndPoint::setConnectionBinder(DConnectionBinder connectionBinder) {
        connBinder=connectionBinder;
    }

    /**
     * @brief syncronous close
     */
    void DEndPoint::closeSocket(void) {
        webSocket->close();
    }

    bool DEndPoint::send(void) {
        return webSocket->send();
    };

    std::string DEndPoint::getID(void) {
        return connBinder.ID;
    }

    std::string DEndPoint::getIP(void) {
        return connBinder.IP;
    }

    int DEndPoint::getPort(void) {
        return connBinder.Port;
    }

    DWebSocketHttpHeaders DEndPoint::getHttpHeaders(void) {
        return connBinder.httpHeaders;
    }

    std::string DEndPoint::getLastError(void) {
        return webSocket->getLastError();
    }

    void DEndPoint::getInfo(void) {
        Log::debug(TAG,"connBinder id: %s",connBinder.ID.c_str());
        Log::debug(TAG,"connBinder ip: %s ",connBinder.IP.c_str());
        Log::debug(TAG,"connBinder port: %d",connBinder.Port);
        Log::debug(TAG,"connBinder headers:");
        for (auto it : connBinder.httpHeaders) {
            Log::debug(TAG,"\t %s: %s",it.first.c_str(),it.second.c_str());
        }
    }

    DMPacket& DEndPoint::getInputBuffer(void) {
        return webSocket->inputBuffer;
    }

    DMPacket& DEndPoint::getOutputBuffer(void) {
        return webSocket->outputBuffer;
    }

    std::weak_ptr<DWebSocket> DEndPoint::getWebSocket(void) {
        return webSocket;
    }
}
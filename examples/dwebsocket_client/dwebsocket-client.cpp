#include <dwebsocket_client>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <limits>
#include <mutex>
#include <string>

int main(int argc, char* argv[])
{
    const std::string url = argc > 1 ? argv[1] : "ws://127.0.0.1:8080";
    std::mutex connectionMutex;
    std::condition_variable connectionChanged;
    bool connected = false;
    bool connectionFailed = false;
    dws::DWebSocketClient client(url);

    client.setOnEventCallback([&](dws::DWebSocketEventType event, DMPacket& packet) {
        switch (event) {
        case dws::EVENT_OPEN:
            std::cout << "Connected to " << url << '\n';
            {
                std::lock_guard<std::mutex> lock(connectionMutex);
                connected = true;
            }
            connectionChanged.notify_one();
            break;
        case dws::EVENT_DATA_STR:
        case dws::EVENT_DATA_BIN:
            std::cout << "Received: " << packet.readString() << '\n';
            break;
        case dws::EVENT_ERROR:
            std::cerr << "WebSocket error: " << packet.readString() << '\n';
            {
                std::lock_guard<std::mutex> lock(connectionMutex);
                connectionFailed = true;
            }
            connectionChanged.notify_one();
            break;
        case dws::EVENT_CLOSE:
            std::cout << "Disconnected\n";
            break;
        default:
            break;
        }
    });

    if (!client.connect()) {
        std::cerr << "Failed to start connection: " << client.getLastError() << '\n';
        return 1;
    }

    bool open;
    {
        std::unique_lock<std::mutex> lock(connectionMutex);
        open = connectionChanged.wait_for(
            lock, std::chrono::seconds(10), [&] { return connected || connectionFailed; })
            && connected;
    }
    if (!open) {
        std::cerr << "Connection failed or timed out\n";
        client.close();
        return 1;
    }

    std::cout << "Type messages to send (Ctrl+D to quit):\n";
    std::string message;
    while (std::getline(std::cin, message)) {
        if (message.size() > std::numeric_limits<std::uint16_t>::max()) {
            std::cerr << "Message is too long to send\n";
            continue;
        }
        client.getOutputBuffer().setBuffer(
            message.data(), static_cast<std::uint16_t>(message.size()));
        if (!client.send()) {
            std::cerr << "Failed to send message: " << client.getLastError() << '\n';
        }
    }

    client.close();
    return 0;
}
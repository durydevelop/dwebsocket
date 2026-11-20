#include <dwebsocket_server>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

int main()
{
    dws::DWebSocketServer server("0.0.0.0", 8080);

    server.setOnEventCallback([](dws::DWebSocketEventType event,
                                 std::shared_ptr<dws::DEndPoint> endpoint) {
        switch (event) {
        case dws::EVENT_OPEN:
            std::cout << "Client connected: " << endpoint->getID()
                      << " from " << endpoint->getIP() << '\n';
            break;
        case dws::EVENT_DATA_STR:
        case dws::EVENT_DATA_BIN: {
            const std::string message = endpoint->getInputBuffer().readString();
            std::cout << "Received: " << message << '\n';
            if (message.size() > std::numeric_limits<std::uint16_t>::max()) {
                std::cerr << "Message is too long to echo\n";
                break;
            }
            endpoint->getOutputBuffer().setBuffer(
                message.data(), static_cast<std::uint16_t>(message.size()));
            if (!endpoint->send()) {
                std::cerr << "Failed to send echo: "
                          << endpoint->getLastError() << '\n';
            }
            break;
        }
        case dws::EVENT_CLOSE:
            std::cout << "Client disconnected: " << endpoint->getID() << '\n';
            break;
        case dws::EVENT_ERROR:
            std::cerr << "WebSocket error: " << endpoint->getLastError() << '\n';
            break;
        default:
            break;
        }
    });

    if (!server.run()) {
        std::cerr << "Failed to start WebSocket server: "
                  << server.getLastError() << '\n';
        return 1;
    }
    return 0;
}

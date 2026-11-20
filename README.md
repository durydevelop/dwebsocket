# DWebSocket
DWebSocket is a C++ library built on top of [IXWebSocket](https://github.com/machinezone/IXWebSocket) with emscripten implementation for Web Assembly.

## Features
- Cross-platform: works on Windows, Linux, Linux Arm and Web Assembly.
- Ready to use **WebSocket Server** class.
- Ready to use **WebSocket Client** class.

## Dependencies
* CMake 3.13 or newer and a C++20 compiler.
* [Emscripten SDK](https://emscripten.org/) to build for WebAssembly.
* IXWebSocket and DMPacket are included in the repository by default.

## How to use

### WebAssembly (Emscripten)

Activate the Emscripten SDK environment, then configure and build the library:

```sh
source /path/to/emsdk/emsdk_env.sh
emcmake cmake -S . -B build-wasm \
  -DPLATFORM=Web \
  -Ddwebsocket_BUILD_EXAMPLES=OFF
cmake --build build-wasm
```

This builds the library; it does not produce a standalone web application.
Link it into an Emscripten application built with the same SDK. The CMake
configuration adds the Emscripten WebSocket support automatically. ZLIB
compression is enabled by default and can be disabled with `-DUSE_ZLIB=OFF`.

In the browser, DWebSocket provides a WebSocket client (not a server). For
example:

```cpp
#include <dwebsocket_client>

dws::DWebSocketClient client("wss://example.com/socket");
client.connect();
```

Keep the client alive while it is in use, and serve the page over HTTP or
HTTPS. Use a `ws://` or `wss://` URL reachable from the browser.


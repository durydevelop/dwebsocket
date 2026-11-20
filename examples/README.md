# Native examples

The server and client sources are in the separate `dwebsocket_server/` and
`dwebsocket_client/` directories.

Build both examples from the repository root:
d
```sh
cmake -S . -B build -Ddwebsocket_BUILD_EXAMPLES=ON
cmake --build build --target dwebsocket-server dwebsocket-client
```

Start the echo server in one terminal:

```sh
./build/bin/dwebsocket-server
```

Then connect the client from another terminal and type messages to send:

```sh
./build/bin/dwebsocket-client
```

The client defaults to `ws://127.0.0.1:8080`; a different WebSocket URL can be
passed as its first argument. The server echoes each received message. Press
Ctrl+D to exit the client and Ctrl+C to stop the server.
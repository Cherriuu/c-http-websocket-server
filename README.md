# C HTTP/WebSocket Server

An event-driven HTTP/1.1 and WebSocket server built from scratch in C.

The project explores low-level networking by implementing HTTP request parsing, static file serving, persistent connections, non-blocking I/O, and WebSocket communication directly on top of TCP sockets.

## Planned Features

- HTTP/1.1 request parsing
- Static file serving
- HTTP keep-alive
- Non-blocking sockets
- Linux `epoll` event loop
- Partial read and write handling
- Per-connection state management
- WebSocket upgrade and framing
- Real-time broadcast chat
- Request and connection resource limits

## Tech

- C
- POSIX sockets
- TCP/IP
- HTTP/1.1
- WebSockets
- Linux `epoll`

## Status

In development.
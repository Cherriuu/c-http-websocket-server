## Current Status

Implemented:
- TCP server setup and client acceptance
- HTTP/1.1 request-line parsing
- GET requests
- Static file serving from `public/`
- MIME type detection
- Basic status-code handling
- Path traversal protection
- Per-client connection state and buffers

In progress:
- Non-blocking I/O
- Event-driven connection handling
- Linux epoll integration
- Persistent HTTP connections
- WebSocket upgrade and framing
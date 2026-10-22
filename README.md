# SocketFlow: HTTP & WebSocket Server in C

A single-threaded, event-driven web server built from scratch in C using raw POSIX sockets. It speaks HTTP/1.1 and the WebSocket protocol with no external libraries, so every piece (connection handling, parsing, framing) is implemented by hand.

## Features

### Networking and concurrency
- Non-blocking POSIX sockets with `poll()` to handle many concurrent clients on a single thread
- Correct handling of partial reads and partial writes
- Idle connection timeouts
- Per-connection state machine (read, write, close)
- Fixed-size buffers that bound memory use per client

### HTTP/1.1
- Incremental request parsing, so requests can arrive in any number of chunks
- Persistent (keep-alive) connections
- Static file serving from `public/` with MIME type detection
- Request validation and basic status-code handling
- Path traversal protection

### WebSocket
- HTTP-to-WebSocket protocol upgrade handshake
- Frame parsing, including masked client frames
- Ping/pong control frames
- Graceful connection closure
- Persistent bidirectional messaging

## How it works

Every client connection has its own state and buffers. One loop uses `poll()` to wait for sockets that are ready, then advances each connection's state machine: read bytes, parse as much of the request as has arrived, write the response without blocking, and close when done or idle too long. A connection can start as plain HTTP and upgrade to WebSocket, after which the same loop handles framed messages in both directions.

## Project Structure

```
include/   # Header files
src/       # Server source
public/    # Static files served over HTTP
tests/     # Tests
Makefile
```

## Build and Run

```
make
./server
```

Then open `http://localhost:8080` in your browser.

## Status

Implemented: TCP server and client acceptance, non-blocking `poll()`-based connection handling, incremental HTTP/1.1 parsing, persistent connections, static file serving, path traversal protection, and the WebSocket upgrade, framing, and ping/pong flow.

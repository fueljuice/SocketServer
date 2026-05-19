# OnlineChat (Winsock TCP Chat Server + Client)

[![Language](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/20)
![Platform](https://img.shields.io/badge/platform-Windows-0078D6.svg)
![Transport](https://img.shields.io/badge/transport-TCP-informational.svg)

An educational **TCP chat system** written in **C++ (C++17)** on Windows using **Winsock**.

- **Server** accepts multiple clients, handles requests concurrently (thread-per-client), persists messages to a local text file, and broadcasts updates.
- **Client** connects to the server, registers a username, fetches chat history, sends public messages, and supports **direct messages (DMs)**.




## Features

- **TCP server** (Winsock) listening on a configurable port
- **Multiple clients** supported concurrently
- **Thread-per-client** worker model
- **Custom, fixed-size header protocol** for requests/responses
- **Public chat** stored in a server-side text file
- **Broadcast**: new public messages are sent to all registered clients
- **Direct messages (DMs)**: send a message to a specific registered username
- Client uses a **passive listener thread** (`select`) to print incoming server messages asynchronously

---

## How it works

### Server-side

1. `Server::launch()` starts listening.
2. `Server::acceptConnections()` blocks on `accept()` in a loop.
3. Each accepted socket is:
   - added to the `SessionManager` (connection metadata)
   - handled by a dedicated worker thread (`ClientConnectionWorker::run`)
4. The worker thread continuously:
   - reads a request header + payload (`RequestReader`)
   - parses the request (`ServerProtocol`)
   - routes to the correct handler (`RequestHandler`)
5. `RequestHandler`:
   - validates registration and request status
   - reads/writes to the database file (`DataBaseManager`)
   - sends responses and broadcasts via `NetworkIO`

### Client-side

1. `UserClient::startClient()` connects to the server and starts a passive listener.
2. You register a username, then fetch the chat.
3. Sending commands (`/msg`, `/dm`) constructs a request using `ClientProtocol`.
4. The passive listener thread waits for readable sockets with `select()` and prints responses.

---
# Workflows

### Client Workflow
<img width="500" height="800" alt="mermaid-diagram (1)" src="https://github.com/user-attachments/assets/8b0f46ae-121a-4006-94ec-59ac165c5d72" />

### Server Workflow

<img width="800" height="1000" alt="mermaid-diagram" src="https://github.com/user-attachments/assets/a58465bd-c2cd-4712-b979-ce731f976a12" />

---
# protocol
## Protocol: Request / Response 

### Request format (TCP)

```text
REQUEST (TCP)
┌─────────────────────────── HEADER (7 bytes) ───────────────────────────┐
│  LEN (4 ASCII digits)  │  TYPE (2 ASCII digits)  │  VER (1 ASCII digit) │
└────────────────────────────────────────────────────────────────────────┘
┌──────────────────────────── BODY (LEN bytes) ───────────────────────────┐
│ payload string (may include separators like ":" for DM)                  │
└────────────────────────────────────────────────────────────────────────┘

Example: SEND_MESSAGE ("hello")
Header: 0005 02 1
Body:   hello


RESPONSE (TCP)
┌────────────────────────── HEADER (6 bytes) ────────────────────────────┐
│  LEN (4 ASCII digits)  │  CODE (2 ASCII digits)                         │
└────────────────────────────────────────────────────────────────────────┘
┌──────────────────────────── BODY (LEN bytes) ───────────────────────────┐
│ payload string (chat history / status / broadcast)                       │
└────────────────────────────────────────────────────────────────────────┘
Example: OK ("welcome")
Header: 0007 00
Body:   welcome
```
```bat

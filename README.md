# simple-chat-socket-server

a simple over tcp chat server + client written in c++ using winsock. the objective it to allow a client to interact (read and write) to a text file on the server's machine

---

## overview

the project consists of:

- **server** (`sockets::server::Server`)
  - listens for incoming tcp connections
  - accepts multiple clients
  - receives requests in a custom format
  - stores chat messages in a text file
  - broadcasts new messages to all connected clients

- **client** (`Client::UserClient`)
  - connects to the server
  - sends requests (get chat, send message)
  - receives responses from the server
  - prints server responses to the console

the "database" is simply a text file on disk:

```cpp
#define dbFileDir "C:\\Users\\zohar\\Desktop\\dbFile.txt"
```

## protocol

the server and client use a simple custom protocol over tcp.

### header format

every request from client → server starts with an **8-byte ascii header**:

- `4 bytes`: message body length 
- `4 bytes`: request type   

for example:

```text
"0005""0002"

*This project has been created as part of the 42 curriculum by [Dfeve, Jpecquer, Jmalaval].*

# IRC

An Internet Relay Chat (IRC) server and client project developed as part of the 42 curriculum.

## Description

This project consists of implementing an IRC server that follows the IRC protocol, allowing multiple clients to connect, communicate, and interact through channels.

The main goal is to understand how network applications work under the hood, by handling:

- TCP connections
- Multiple clients simultaneously
- IRC commands and protocols
- Authentication and client registration
- Channels and channel operators
- Message broadcasting
- Client disconnections
- Error handling

The server is designed to be compatible with standard IRC clients (irssi, HexChat, WeeChat, etc.).

### Features

- TCP/IP socket communication
- Multiple simultaneous client connections
- Non-blocking I/O (via `poll()`)
- Client authentication
- Nickname and username management
- Private messages
- Channel creation and management
- Channel joining and leaving
- Channel operators and operator commands
- User modes and channel modes
- IRC protocol-compliant responses
- Graceful client disconnection

### Technologies

- **Language:** C++
- **Standard:** C++98
- **Networking:** TCP sockets
- **Multiplexing:** `poll()`
- **Build system:** Make

## Instructions

### Installation

Clone the repository:

```bash
git clone <repository-url>
cd <repository-name>
```

Compile the project:

```bash
make
```

Available Makefile rules:

```bash
make        # compiles the project
make clean  # removes compiled object files
make fclean # removes object files and the executable
make re     # recompiles everything from scratch
```

### Execution

Start the IRC server with a port and a password:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 password
```

The server then listens for incoming client connections on the specified port.

You can connect using hexchat


### Testing

1. Build the project with `make`.
2. Run the server: `./ircserv 6667 password`.
3. Connect several IRC clients and test:
   - Client registration
   - Nickname conflicts
   - Channel creation
   - Joining and leaving channels
   - Private messages and channel messages
   - Operator privileges
   - Channel modes
   - Client disconnections
   - Multiple clients communicating simultaneously

### Supported IRC commands

| Command | Description |
|---------|-------------|
| PASS | Authenticate with the server password |
| NICK | Set or change a nickname |
| USER | Register a user |
| JOIN | Join a channel |
| PART | Leave a channel |
| PRIVMSG | Send a private or channel message |
| NOTICE | Send a notice |
| KICK | Remove a user from a channel |
| INVITE | Invite a user to a channel |
| TOPIC | Get or change a channel topic |
| MODE | Change user or channel modes |
| QUIT | Disconnect from the server |

### Channel modes

- `i` — Invite-only channel
- `t` — Topic restricted to operators
- `k` — Channel password
- `o` — Channel operator privileges
- `l` — User limit

## Resources

### Classic references

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [RFC 2813 — Internet Relay Chat: Server Protocol](https://datatracker.ietf.org/doc/html/rfc2813)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- `man poll`, `man socket`, `man bind`, `man listen`, `man accept` — reference manuals used throughout development
- [Modern IRC documentation (modern.ircdocs.horse)](https://modern.ircdocs.horse/)

### AI usage

An AI assistant (Claude) was used solely for help writing and structuring this README.md file, specifically for:

- Organizing the content into the required sections (Description, Instructions, Resources).
- Improving clarity and wording of the explanations.

The project's design, implementation, and all source code were written and validated entirely by the team without AI assistance.

## What we learned

Through this project, we gained practical experience with:

- Network programming and TCP/IP communication
- Socket management and non-blocking I/O
- Event-driven programming
- Concurrent client handling
- Protocol parsing
- Object-oriented C++ (C++98)
- Error handling
- Collaborative software development

## Team

- [Dfeve]
- [Jpecquer]
- [Jmalaval]
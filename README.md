# IRC

An Internet Relay Chat (IRC) server and client project developed as part of the 42 curriculum.

## 📖 About

This project consists of implementing an IRC server that follows the IRC protocol, allowing multiple clients to connect, communicate, and interact through channels.

The main goal is to understand how network applications work by handling:

- TCP connections
- Multiple clients simultaneously
- IRC commands and protocols
- Authentication and client registration
- Channels and channel operators
- Message broadcasting
- Client disconnections
- Error handling

The server is designed to be compatible with standard IRC clients.

## 🚀 Features

- TCP/IP socket communication
- Multiple simultaneous client connections
- Non-blocking I/O
- Client authentication
- Nickname and username management
- Private messages
- Channel creation and management
- Channel joining and leaving
- Channel operators
- Operator commands
- User modes and channel modes
- IRC protocol-compliant responses
- Graceful client disconnection

## 🛠️ Technologies

- **Language:** C++
- **Standard:** C++98
- **Networking:** TCP sockets
- **Multiplexing:** poll()
- **Build system:** Make

## 📦 Installation

Clone the repository:

```bash
git clone <repository-url>
cd <repository-name>
```

Compile the project:

```bash
make
```

## ▶️ Usage

Start the IRC server with a port and a password:

```bash
./ircserv <port> <password>
```

For example:

```bash
./ircserv 6667 password
```

The server will then listen for incoming client connections on the specified port.

You can connect using an IRC client such as irssi, HexChat, or WeeChat.

Example with irssi:

```bash
irssi
```

Then connect to the server:

```
/connect localhost 6667 password
```

## 💬 Supported IRC Commands

The server supports the main commands required by the project, including:

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

## 🔐 Channel Modes

Depending on the implementation, the server supports channel modes such as:

- `i` — Invite-only channel
- `t` — Topic restricted to operators
- `k` — Channel password
- `o` — Channel operator privileges
- `l` — User limit

## 🧪 Testing

Build the project:

```bash
make
```

Run the server:

```bash
./ircserv 6667 password
```

Then connect several IRC clients and test:

- Client registration
- Nickname conflicts
- Channel creation
- Joining and leaving channels
- Private messages
- Channel messages
- Operator privileges
- Channel modes
- Client disconnections
- Multiple clients communicating simultaneously

## 🧹 Cleaning

Remove compiled object files:

```bash
make clean
```

Remove object files and the executable:

```bash
make fclean
```

Recompile everything from scratch:

```bash
make re
```

## 👥 Team

This project was developed as part of the 42 curriculum.

- [Dfeve]
- [Jpecquer]
- [Jmalaval]

## 📚 What We Learned

Through this project, we gained practical experience with:

- Network programming
- TCP/IP communication
- Socket management
- Event-driven programming
- Concurrent client handling
- Protocol parsing
- Object-oriented C++
- Error handling
- Collaborative software development

## 📜 License

This project was created for educational purposes as part of the 42 curriculum.
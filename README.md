# Cross-Platform Chat Application

A cross-platform chat application built with C++ that supports multiple clients connecting to a central server. The application uses TCP sockets for communication and works on both Windows and Unix-like systems.

## Features

- **Cross-platform**: Works on Windows, Linux, and macOS
- **Multi-client support**: Multiple clients can connect simultaneously
- **Real-time messaging**: Instant message delivery between clients
- **User management**: Track connected users and broadcast join/leave events
- **Command interface**: Built-in commands for user management
- **Protocol-based**: Custom binary protocol for efficient communication

## Project Structure

```
/chat-app
 ├── /server          # Server-side code
 │    ├── main.cpp     # Server entry point
 │    ├── Server.cpp/.h
 │    ├── ClientSession.cpp/.h
 │    ├── MessageRouter.cpp/.h
 │    └── Protocol.cpp/.h
 │
 ├── /client          # Client-side code
 │    ├── main.cpp     # Client entry point
 │    ├── Client.cpp/.h
 │    ├── Network.cpp/.h
 │    └── UI.cpp/.h    # CLI interface
 │
 ├── /shared          # Shared code
 │    ├── Message.h
 │    ├── Serializer.h
 │    └── Protocol.h
 │
 ├── /tests           # Test files (to be implemented)
 ├── CMakeLists.txt   # Build configuration
 └── README.md        # This file
```

## Building

### Prerequisites

- CMake 3.12 or higher
- C++17 compatible compiler:
  - Windows: MSVC 2017+ or MinGW
  - Linux: GCC 7+ or Clang 5+
  - macOS: Xcode 10+ or Clang 5+

### Build Instructions

1. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Generate build files:**
   ```bash
   cmake ..
   ```

3. **Build the project:**
   ```bash
   cmake --build .
   ```

   On Windows with Visual Studio:
   ```bash
   cmake --build . --config Release
   ```

4. **Executables will be in:**
   - `build/bin/chat-server` (or `chat-server.exe` on Windows)
   - `build/bin/chat-client` (or `chat-client.exe` on Windows)

## Usage

### Starting the Server

Run the server with an optional port number (default: 8080):

```bash
./bin/chat-server [port]
```

Example:
```bash
./bin/chat-server 8080
```

### Running Clients

Run a client with username and optional server address/port:

```bash
./bin/chat-client <username> [host] [port]
```

Examples:
```bash
# Connect to localhost:8080 as "Alice"
./bin/chat-client Alice

# Connect to a specific server
./bin/chat-client Bob 192.168.1.100 8080

# Connect to a different port
./bin/chat-client Charlie localhost 9000
```

### Client Commands

Once connected, you can use these commands:

- `/help` - Show available commands
- `/users` or `/list` - List all connected users
- `/quit` or `/exit` - Disconnect from the server

### Example Session

1. Start the server:
   ```bash
   ./bin/chat-server
   ```

2. In separate terminals, start clients:
   ```bash
   ./bin/chat-client Alice
   ./bin/chat-client Bob
   ```

3. Type messages in the client terminals - they will be broadcast to all connected clients.

## Protocol

The application uses a custom binary protocol:

- **Magic Number**: 0x43484154 ("CHAT")
- **Version**: 1
- **Message Types**: TEXT, JOIN, LEAVE, SYSTEM, USER_LIST, ERROR
- **Message Format**: Header (16 bytes) + Payload (variable length)

## Architecture

### Server Components

- **Server**: Main server class that accepts connections
- **ClientSession**: Manages individual client connections
- **MessageRouter**: Routes messages between clients
- **Protocol**: Protocol handling and validation

### Client Components

- **Client**: Main client class coordinating network and UI
- **Network**: Handles socket communication
- **UI**: Command-line interface for user interaction

### Shared Components

- **Message**: Message structure definition
- **Serializer**: Binary serialization/deserialization
- **Protocol**: Protocol constants and definitions

## Threading Model

- **Server**: One thread per client for receiving, one thread per client for sending
- **Client**: One thread for receiving messages, main thread for UI input
- All shared data structures are protected with mutexes

## Platform-Specific Notes

### Windows

- Uses Winsock2 for networking
- Automatically links `ws2_32.lib`
- Requires Windows 7 or later

### Linux/macOS

- Uses POSIX sockets
- Links pthread for threading
- Should work on any modern Linux distribution or macOS version

## Future Enhancements

- [ ] GUI client using Qt or similar framework
- [ ] Message encryption
- [ ] Private messaging between users
- [ ] File transfer support
- [ ] Message history persistence
- [ ] User authentication
- [ ] Room/channel support
- [ ] Unit tests
- [ ] Docker containerization

## License

This project is provided as-is for educational purposes.

## Contributing

Feel free to submit issues and enhancement requests!


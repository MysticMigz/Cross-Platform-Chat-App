# Quick Start Guide

## Prerequisites

1. **CMake** (3.12 or higher)
   - Download from: https://cmake.org/download/
   - Or install via: `winget install Kitware.CMake` or `choco install cmake`

2. **C++ Compiler**
   - **Visual Studio 2017+** (includes MSVC compiler)
   - Or **MinGW-w64** (GCC for Windows)
   - Or **Clang** for Windows

## Building the Application

### Option 1: Using CMake GUI (Easiest for Windows)

1. Open CMake GUI
2. Set "Where is the source code" to: `C:\Users\migue\Desktop\Cross-Platform Chat App`
3. Set "Where to build the binaries" to: `C:\Users\migue\Desktop\Cross-Platform Chat App\build`
4. Click "Configure"
5. Select your compiler (Visual Studio, MinGW, etc.)
6. Click "Generate"
7. Click "Open Project" (if using Visual Studio) or build manually

### Option 2: Using Command Line

Open PowerShell or Command Prompt in the project directory:

```powershell
# Create build directory
mkdir build
cd build

# Generate build files (use one of these):
cmake ..                          # For Visual Studio
cmake .. -G "MinGW Makefiles"     # For MinGW
cmake .. -G "Unix Makefiles"      # For MSYS2/Git Bash

# Build the project
cmake --build . --config Release

# On Windows with Visual Studio, you can also:
cmake --build . --config Debug    # For debug build
```

## Running the Application

### Step 1: Start the Server

Open a terminal and run:

```powershell
cd build\bin
.\chat-server.exe 8080
```

Or from the project root:
```powershell
.\build\bin\chat-server.exe 8080
```

The server will start on port 8080 (or the port you specify).

### Step 2: Start Clients

Open **separate terminal windows** for each client:

**Terminal 2:**
```powershell
cd build\bin
.\chat-client.exe Alice
```

**Terminal 3:**
```powershell
cd build\bin
.\chat-client.exe Bob
```

**Terminal 4:**
```powershell
cd build\bin
.\chat-client.exe Charlie
```

### Connecting to a Remote Server

If the server is on a different machine:

```powershell
.\chat-client.exe Username 192.168.1.100 8080
```

## Example Session

1. **Terminal 1 (Server):**
   ```
   > .\chat-server.exe 8080
   Server started on port 8080
   Chat server running on port 8080
   Press Ctrl+C to stop...
   Client Alice joined (ID: 1)
   Client Bob joined (ID: 2)
   ```

2. **Terminal 2 (Client - Alice):**
   ```
   > .\chat-client.exe Alice
   Connecting to 127.0.0.1:8080 as Alice...
   [SYSTEM]: Connected to server at 127.0.0.1:8080
   >>> Alice joined the chat
   > Hello everyone!
   ```

3. **Terminal 3 (Client - Bob):**
   ```
   > .\chat-client.exe Bob
   Connecting to 127.0.0.1:8080 as Bob...
   [SYSTEM]: Connected to server at 127.0.0.1:8080
   >>> Bob joined the chat
   [Alice]: Hello everyone!
   > Hi Alice!
   ```

## Client Commands

Once connected, you can use these commands:

- `/help` - Show available commands
- `/users` or `/list` - List all connected users
- `/quit` or `/exit` - Disconnect from server

## Troubleshooting

### "CMake not found"
- Install CMake and add it to your PATH
- Or use full path: `C:\Program Files\CMake\bin\cmake.exe`

### "No compiler found"
- Install Visual Studio Build Tools or MinGW
- Make sure compiler is in your PATH

### "Port already in use"
- Use a different port: `.\chat-server.exe 9000`
- Or close the application using that port

### "Connection refused"
- Make sure the server is running first
- Check firewall settings
- Verify the IP address and port are correct

### Build Errors
- Make sure you're using C++17 compatible compiler
- Try cleaning and rebuilding: Delete `build` folder and start over


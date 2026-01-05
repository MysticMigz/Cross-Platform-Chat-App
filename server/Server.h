#ifndef SERVER_H
#define SERVER_H

#include "ClientSession.h"
#include "MessageRouter.h"
#include <string>
#include <thread>
#include <atomic>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SocketHandle;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketHandle;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

class Server {
public:
    Server(uint16_t port = 8080);
    ~Server();
    
    bool start();
    void stop();
    bool isRunning() const { return running_; }
    
private:
    void acceptThread();
    bool initializeSocket();
    void cleanupSocket();
    void cleanupDisconnectedClients();
    
    uint16_t port_;
    SocketHandle listenSocket_;
    std::atomic<bool> running_;
    std::thread acceptThread_;
    
    MessageRouter router_;
    std::vector<ClientSession*> clients_;
    std::mutex clientsMutex_;
};

#endif // SERVER_H


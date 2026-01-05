#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <vector>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET SocketHandle;
    #define INVALID_SOCKET_VALUE INVALID_SOCKET
    #define SOCKET_ERROR_VALUE SOCKET_ERROR
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketHandle;
    #define INVALID_SOCKET_VALUE -1
    #define SOCKET_ERROR_VALUE -1
#endif

class MessageRouter;

class ClientSession {
public:
    ClientSession(SocketHandle socket, MessageRouter* router);
    ~ClientSession();
    
    bool start();
    void stop();
    void sendMessage(const std::vector<uint8_t>& data);
    std::string getUsername() const { return username_; }
    bool isConnected() const { return connected_; }
    uint32_t getClientId() const { return clientId_; }
    
private:
    void receiveThread();
    void sendThread();
    bool receiveData(std::vector<uint8_t>& buffer);
    bool sendData(const std::vector<uint8_t>& data);
    
    SocketHandle socket_;
    MessageRouter* router_;
    std::string username_;
    uint32_t clientId_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    
    std::thread receiveThread_;
    std::thread sendThread_;
    
    std::queue<std::vector<uint8_t>> sendQueue_;
    std::mutex sendQueueMutex_;
    
    static uint32_t nextClientId_;
};

#endif // CLIENTSESSION_H


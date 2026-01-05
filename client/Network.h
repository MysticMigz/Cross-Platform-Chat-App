#ifndef NETWORK_H
#define NETWORK_H

#include "../shared/Message.h"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
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

class Network {
public:
    using MessageCallback = std::function<void(const Message&)>;
    
    Network();
    ~Network();
    
    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const { return connected_; }
    
    void sendMessage(const Message& msg);
    void setMessageCallback(MessageCallback callback);
    
private:
    void receiveThread();
    bool receiveData(std::vector<uint8_t>& buffer);
    bool sendData(const std::vector<uint8_t>& data);
    
    SocketHandle socket_;
    std::atomic<bool> connected_;
    std::atomic<bool> running_;
    std::thread receiveThread_;
    
    MessageCallback messageCallback_;
    std::mutex callbackMutex_;
};

#endif // NETWORK_H


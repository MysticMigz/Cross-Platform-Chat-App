#include "Server.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

Server::Server(uint16_t port) : port_(port), listenSocket_(INVALID_SOCKET), running_(false) {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
        }
    #endif
}

Server::~Server() {
    stop();
    cleanupSocket();
    
    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool Server::initializeSocket() {
    listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket_ == INVALID_SOCKET_VALUE) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    // Set socket options
    int opt = 1;
    #ifdef _WIN32
        setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
    #else
        setsockopt(listenSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif
    
    // Bind socket
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);
    
    if (bind(listenSocket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR_VALUE) {
        std::cerr << "Failed to bind socket to port " << port_ << std::endl;
        cleanupSocket();
        return false;
    }
    
    // Listen
    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR_VALUE) {
        std::cerr << "Failed to listen on socket" << std::endl;
        cleanupSocket();
        return false;
    }
    
    return true;
}

void Server::cleanupSocket() {
    if (listenSocket_ != INVALID_SOCKET_VALUE) {
        #ifdef _WIN32
            closesocket(listenSocket_);
        #else
            close(listenSocket_);
        #endif
        listenSocket_ = INVALID_SOCKET_VALUE;
    }
}

bool Server::start() {
    if (running_) {
        return false;
    }
    
    if (!initializeSocket()) {
        return false;
    }
    
    running_ = true;
    acceptThread_ = std::thread(&Server::acceptThread, this);
    
    std::cout << "Server started on port " << port_ << std::endl;
    return true;
}

void Server::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // Close listen socket to unblock accept
    cleanupSocket();
    
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    
    // Stop all client sessions
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (ClientSession* client : clients_) {
            if (client) {
                client->stop();
                delete client;
            }
        }
        clients_.clear();
    }
    
    std::cout << "Server stopped" << std::endl;
}

void Server::cleanupDisconnectedClients() {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(
        std::remove_if(clients_.begin(), clients_.end(),
            [](ClientSession* client) {
                if (client && !client->isConnected()) {
                    client->stop();
                    delete client;
                    return true;
                }
                return false;
            }),
        clients_.end()
    );
}

void Server::acceptThread() {
    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        SocketHandle clientSocket = accept(listenSocket_, 
                                          reinterpret_cast<sockaddr*>(&clientAddr), 
                                          &clientAddrLen);
        
        if (clientSocket == INVALID_SOCKET_VALUE) {
            if (running_) {
                std::cerr << "Accept failed" << std::endl;
            }
            break;
        }
        
        // Create new client session
        ClientSession* client = new ClientSession(clientSocket, &router_);
        router_.addClient(client);
        
        {
            std::lock_guard<std::mutex> lock(clientsMutex_);
            clients_.push_back(client);
        }
        
        if (client->start()) {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
            std::cout << "New client connected from " << ipStr << ":" << ntohs(clientAddr.sin_port) << std::endl;
        } else {
            std::cerr << "Failed to start client session" << std::endl;
            delete client;
        }
        
        // Cleanup disconnected clients periodically
        cleanupDisconnectedClients();
    }
}


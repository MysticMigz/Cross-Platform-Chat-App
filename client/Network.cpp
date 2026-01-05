#include "Network.h"
#include "../shared/Serializer.h"
#include "../shared/Protocol.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

Network::Network() : socket_(INVALID_SOCKET_VALUE), connected_(false), running_(false) {
    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
}

Network::~Network() {
    disconnect();
    #ifdef _WIN32
        WSACleanup();
    #endif
}

bool Network::connect(const std::string& host, uint16_t port) {
    if (connected_) {
        return false;
    }
    
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == INVALID_SOCKET_VALUE) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }
    
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << host << std::endl;
        #ifdef _WIN32
            closesocket(socket_);
        #else
            close(socket_);
        #endif
        socket_ = INVALID_SOCKET_VALUE;
        return false;
    }
    
    if (::connect(socket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR_VALUE) {
        std::cerr << "Failed to connect to " << host << ":" << port << std::endl;
        #ifdef _WIN32
            closesocket(socket_);
        #else
            close(socket_);
        #endif
        socket_ = INVALID_SOCKET_VALUE;
        return false;
    }
    
    connected_ = true;
    running_ = true;
    receiveThread_ = std::thread(&Network::receiveThread, this);
    
    return true;
}

void Network::disconnect() {
    if (!connected_) {
        return;
    }
    
    running_ = false;
    connected_ = false;
    
    if (socket_ != INVALID_SOCKET_VALUE) {
        #ifdef _WIN32
            closesocket(socket_);
        #else
            close(socket_);
        #endif
        socket_ = INVALID_SOCKET_VALUE;
    }
    
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
}

bool Network::receiveData(std::vector<uint8_t>& buffer) {
    MessageHeader header;
    int bytesReceived = 0;
    
    // Receive header
    bytesReceived = recv(socket_, reinterpret_cast<char*>(&header), sizeof(MessageHeader), 0);
    
    if (bytesReceived <= 0) {
        return false;
    }
    
    if (bytesReceived != static_cast<int>(sizeof(MessageHeader))) {
        return false;
    }
    
    // Validate magic
    if (header.magic != PROTOCOL_MAGIC) {
        return false;
    }
    
    // Receive payload
    buffer.resize(sizeof(MessageHeader) + header.payloadSize);
    std::memcpy(buffer.data(), &header, sizeof(MessageHeader));
    
    if (header.payloadSize > 0) {
        bytesReceived = recv(socket_, reinterpret_cast<char*>(buffer.data() + sizeof(MessageHeader)), 
                           header.payloadSize, 0);
        if (bytesReceived != static_cast<int>(header.payloadSize)) {
            return false;
        }
    }
    
    return true;
}

bool Network::sendData(const std::vector<uint8_t>& data) {
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        int bytesSent = send(socket_, 
                                reinterpret_cast<const char*>(data.data() + totalSent),
                                static_cast<int>(data.size() - totalSent), 0);
        if (bytesSent <= 0) {
            return false;
        }
        totalSent += bytesSent;
    }
    return true;
}

void Network::sendMessage(const Message& msg) {
    if (!connected_) {
        return;
    }
    
    std::vector<uint8_t> data = Serializer::serialize(msg);
    if (!sendData(data)) {
        connected_ = false;
    }
}

void Network::setMessageCallback(MessageCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    messageCallback_ = callback;
}

void Network::receiveThread() {
    while (running_ && connected_) {
        std::vector<uint8_t> buffer;
        
        if (!receiveData(buffer)) {
            connected_ = false;
            break;
        }
        
        Message msg;
        if (Serializer::deserialize(buffer, msg)) {
            std::lock_guard<std::mutex> lock(callbackMutex_);
            if (messageCallback_) {
                messageCallback_(msg);
            }
        }
    }
}


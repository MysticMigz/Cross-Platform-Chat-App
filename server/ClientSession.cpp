#include "ClientSession.h"
#include "MessageRouter.h"
#include "../shared/Message.h"
#include "../shared/Serializer.h"
#include "../shared/Protocol.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
    #pragma comment(lib, "ws2_32.lib")
#endif

uint32_t ClientSession::nextClientId_ = 1;

ClientSession::ClientSession(SocketHandle socket, MessageRouter* router)
    : socket_(socket), router_(router), clientId_(nextClientId_++), 
      connected_(false), running_(false) {
}

ClientSession::~ClientSession() {
    stop();
}

bool ClientSession::start() {
    if (running_) {
        return false;
    }
    
    running_ = true;
    connected_ = true;
    
    receiveThread_ = std::thread(&ClientSession::receiveThread, this);
    sendThread_ = std::thread(&ClientSession::sendThread, this);
    
    return true;
}

void ClientSession::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    connected_ = false;
    
    #ifdef _WIN32
        closesocket(socket_);
    #else
        close(socket_);
    #endif
    
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
    
    if (sendThread_.joinable()) {
        sendThread_.join();
    }
}

void ClientSession::sendMessage(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(sendQueueMutex_);
    sendQueue_.push(data);
}

bool ClientSession::receiveData(std::vector<uint8_t>& buffer) {
    MessageHeader header;
    ssize_t bytesReceived = 0;
    
    // First, receive the header
    bytesReceived = recv(socket_, reinterpret_cast<char*>(&header), sizeof(MessageHeader), 0);
    
    if (bytesReceived <= 0) {
        return false;
    }
    
    if (bytesReceived != sizeof(MessageHeader)) {
        // Partial header received, would need buffering in production
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
        if (bytesReceived != static_cast<ssize_t>(header.payloadSize)) {
            return false;
        }
    }
    
    return true;
}

bool ClientSession::sendData(const std::vector<uint8_t>& data) {
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        ssize_t bytesSent = send(socket_, 
                                reinterpret_cast<const char*>(data.data() + totalSent),
                                data.size() - totalSent, 0);
        if (bytesSent <= 0) {
            return false;
        }
        totalSent += bytesSent;
    }
    return true;
}

void ClientSession::receiveThread() {
    while (running_ && connected_) {
        std::vector<uint8_t> buffer;
        
        if (!receiveData(buffer)) {
            connected_ = false;
            break;
        }
        
        Message msg;
        if (Serializer::deserialize(buffer, msg)) {
            // Handle join message
            if (msg.type == MessageType::JOIN && username_.empty()) {
                username_ = msg.sender;
                if (router_) {
                    router_->onClientJoined(this, username_);
                }
            }
            
            // Route message through router
            if (router_) {
                router_->routeMessage(this, msg);
            }
        }
    }
    
    // Notify router of disconnection
    if (router_ && !username_.empty()) {
        router_->onClientLeft(this, username_);
    }
}

void ClientSession::sendThread() {
    while (running_ && connected_) {
        std::vector<uint8_t> data;
        
        {
            std::lock_guard<std::mutex> lock(sendQueueMutex_);
            if (!sendQueue_.empty()) {
                data = sendQueue_.front();
                sendQueue_.pop();
            }
        }
        
        if (!data.empty()) {
            if (!sendData(data)) {
                connected_ = false;
                break;
            }
        } else {
            // Small sleep to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}


#include "MessageRouter.h"
#include "../shared/Serializer.h"
#include "../shared/Message.h"
#include <iostream>

MessageRouter::MessageRouter() {
}

MessageRouter::~MessageRouter() {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.clear();
    usernameToClient_.clear();
}

void MessageRouter::addClient(ClientSession* client) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.push_back(client);
}

void MessageRouter::removeClient(ClientSession* client) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    clients_.erase(std::remove(clients_.begin(), clients_.end(), client), clients_.end());
    if (client && !client->getUsername().empty()) {
        usernameToClient_.erase(client->getUsername());
    }
}

void MessageRouter::routeMessage(ClientSession* sender, const Message& msg) {
    if (msg.type == MessageType::TEXT) {
        // Broadcast text message to all clients
        broadcastMessage(msg, sender);
    } else if (msg.type == MessageType::USER_LIST) {
        // Send user list to requesting client
        if (sender) {
            Message userListMsg(MessageType::USER_LIST, "SERVER", "");
            std::vector<std::string> users = getUserList();
            std::string userListStr;
            for (size_t i = 0; i < users.size(); ++i) {
                userListStr += users[i];
                if (i < users.size() - 1) {
                    userListStr += ",";
                }
            }
            userListMsg.content = userListStr;
            
            std::vector<uint8_t> data = Serializer::serialize(userListMsg);
            sender->sendMessage(data);
        }
    }
}

void MessageRouter::broadcastMessage(const Message& msg, ClientSession* exclude) {
    std::vector<uint8_t> data = Serializer::serialize(msg);
    
    std::lock_guard<std::mutex> lock(clientsMutex_);
    for (ClientSession* client : clients_) {
        if (client != exclude && client->isConnected()) {
            client->sendMessage(data);
        }
    }
}

void MessageRouter::onClientJoined(ClientSession* client, const std::string& username) {
    if (!client) return;
    
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        usernameToClient_[username] = client;
    }
    
    // Broadcast join message
    Message joinMsg(MessageType::JOIN, username, username + " joined the chat");
    broadcastMessage(joinMsg, client);
    
    // Send user list update
    sendUserListUpdate();
    
    std::cout << "Client " << username << " joined (ID: " << client->getClientId() << ")" << std::endl;
}

void MessageRouter::onClientLeft(ClientSession* client, const std::string& username) {
    if (!client) return;
    
    removeClient(client);
    
    // Broadcast leave message
    Message leaveMsg(MessageType::LEAVE, username, username + " left the chat");
    broadcastMessage(leaveMsg);
    
    // Send user list update
    sendUserListUpdate();
    
    std::cout << "Client " << username << " left (ID: " << client->getClientId() << ")" << std::endl;
}

std::vector<std::string> MessageRouter::getUserList() const {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    std::vector<std::string> users;
    for (const auto& pair : usernameToClient_) {
        if (pair.second && pair.second->isConnected()) {
            users.push_back(pair.first);
        }
    }
    return users;
}

void MessageRouter::sendUserListUpdate() {
    std::vector<std::string> users = getUserList();
    std::string userListStr;
    for (size_t i = 0; i < users.size(); ++i) {
        userListStr += users[i];
        if (i < users.size() - 1) {
            userListStr += ",";
        }
    }
    
    Message userListMsg(MessageType::USER_LIST, "SERVER", userListStr);
    broadcastMessage(userListMsg);
}


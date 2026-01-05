#ifndef MESSAGEROUTER_H
#define MESSAGEROUTER_H

#include "ClientSession.h"
#include "../shared/Message.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <string>

class MessageRouter {
public:
    MessageRouter();
    ~MessageRouter();
    
    void addClient(ClientSession* client);
    void removeClient(ClientSession* client);
    void routeMessage(ClientSession* sender, const Message& msg);
    void broadcastMessage(const Message& msg, ClientSession* exclude = nullptr);
    void onClientJoined(ClientSession* client, const std::string& username);
    void onClientLeft(ClientSession* client, const std::string& username);
    std::vector<std::string> getUserList() const;
    
private:
    std::vector<ClientSession*> clients_;
    std::unordered_map<std::string, ClientSession*> usernameToClient_;
    mutable std::mutex clientsMutex_;
    
    void sendUserListUpdate();
};

#endif // MESSAGEROUTER_H


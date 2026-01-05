#ifndef CLIENT_H
#define CLIENT_H

#include "Network.h"
#include "UI.h"
#include "../shared/Message.h"
#include <string>

class Client {
public:
    Client();
    ~Client();
    
    bool connect(const std::string& host, uint16_t port, const std::string& username);
    void disconnect();
    void sendTextMessage(const std::string& text);
    void requestUserList();
    bool isConnected() const;
    
private:
    void onMessageReceived(const Message& msg);
    void onInputReceived(const std::string& input);
    void handleCommand(const std::string& command);
    
    Network network_;
    UI ui_;
    std::string username_;
    bool connected_;
};

#endif // CLIENT_H


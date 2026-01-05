#ifndef UI_H
#define UI_H

#include "../shared/Message.h"
#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>

class UI {
public:
    using InputCallback = std::function<void(const std::string&)>;
    
    UI();
    ~UI();
    
    void start();
    void stop();
    void displayMessage(const Message& msg);
    void displaySystemMessage(const std::string& msg);
    void setInputCallback(InputCallback callback);
    void setUsername(const std::string& username);
    void updateUserList(const std::vector<std::string>& users);
    
private:
    void inputThread();
    void clearScreen();
    void printHeader();
    void printMessages();
    void printInputPrompt();
    
    std::vector<Message> messages_;
    std::vector<std::string> systemMessages_;
    std::vector<std::string> userList_;
    std::string username_;
    std::atomic<bool> running_;
    std::thread inputThread_;
    std::mutex messagesMutex_;
    
    InputCallback inputCallback_;
    std::mutex callbackMutex_;
};

#endif // UI_H


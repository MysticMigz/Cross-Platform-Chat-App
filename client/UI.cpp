#include "UI.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

UI::UI() : running_(false) {
}

UI::~UI() {
    stop();
}

void UI::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    inputThread_ = std::thread(&UI::inputThread, this);
    
    clearScreen();
    printHeader();
}

void UI::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    if (inputThread_.joinable()) {
        inputThread_.join();
    }
}

void UI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void UI::printHeader() {
    std::cout << "========================================\n";
    std::cout << "     Cross-Platform Chat Application\n";
    std::cout << "========================================\n";
    if (!username_.empty()) {
        std::cout << "Logged in as: " << username_ << "\n";
    }
    std::cout << "========================================\n\n";
}

void UI::displayMessage(const Message& msg) {
    std::lock_guard<std::mutex> lock(messagesMutex_);
    messages_.push_back(msg);
    
    // Keep only last 100 messages
    if (messages_.size() > 100) {
        messages_.erase(messages_.begin());
    }
    
    // Print message
    std::string typeStr;
    switch (msg.type) {
        case MessageType::TEXT:
            std::cout << "[" << msg.sender << "]: " << msg.content << std::endl;
            break;
        case MessageType::JOIN:
            std::cout << ">>> " << msg.content << std::endl;
            break;
        case MessageType::LEAVE:
            std::cout << "<<< " << msg.content << std::endl;
            break;
        case MessageType::SYSTEM:
            std::cout << "[SYSTEM]: " << msg.content << std::endl;
            break;
        case MessageType::USER_LIST:
            if (!msg.content.empty()) {
                std::istringstream iss(msg.content);
                std::string user;
                std::vector<std::string> users;
                while (std::getline(iss, user, ',')) {
                    users.push_back(user);
                }
                updateUserList(users);
            }
            break;
        default:
            std::cout << "[UNKNOWN]: " << msg.content << std::endl;
    }
    
    printInputPrompt();
}

void UI::displaySystemMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(messagesMutex_);
    systemMessages_.push_back(msg);
    
    std::cout << "[SYSTEM]: " << msg << std::endl;
    printInputPrompt();
}

void UI::updateUserList(const std::vector<std::string>& users) {
    userList_ = users;
    // Could display user list in a sidebar if implementing GUI
}

void UI::setInputCallback(InputCallback callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    inputCallback_ = callback;
}

void UI::setUsername(const std::string& username) {
    username_ = username;
}

void UI::printInputPrompt() {
    std::cout << "> ";
    std::cout.flush();
}

void UI::inputThread() {
    std::string input;
    
    while (running_) {
        printInputPrompt();
        
        std::getline(std::cin, input);
        
        if (!input.empty()) {
            std::lock_guard<std::mutex> lock(callbackMutex_);
            if (inputCallback_) {
                inputCallback_(input);
            }
        }
        
        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


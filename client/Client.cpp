#include "Client.h"
#include <iostream>
#include <sstream>

Client::Client() : connected_(false) {
    network_.setMessageCallback([this](const Message& msg) {
        onMessageReceived(msg);
    });
    
    ui_.setInputCallback([this](const std::string& input) {
        onInputReceived(input);
    });
}

Client::~Client() {
    disconnect();
}

bool Client::connect(const std::string& host, uint16_t port, const std::string& username) {
    if (connected_) {
        return false;
    }
    
    username_ = username;
    
    if (!network_.connect(host, port)) {
        return false;
    }
    
    // Send join message
    Message joinMsg(MessageType::JOIN, username, "");
    network_.sendMessage(joinMsg);
    
    ui_.setUsername(username);
    ui_.start();
    connected_ = true;
    
    ui_.displaySystemMessage("Connected to server at " + host + ":" + std::to_string(port));
    
    return true;
}

void Client::disconnect() {
    if (!connected_) {
        return;
    }
    
    // Send leave message
    if (network_.isConnected()) {
        Message leaveMsg(MessageType::LEAVE, username_, "");
        network_.sendMessage(leaveMsg);
    }
    
    network_.disconnect();
    ui_.stop();
    connected_ = false;
}

void Client::sendTextMessage(const std::string& text) {
    if (!connected_ || !network_.isConnected()) {
        return;
    }
    
    Message msg(MessageType::TEXT, username_, text);
    network_.sendMessage(msg);
}

void Client::requestUserList() {
    if (!connected_ || !network_.isConnected()) {
        return;
    }
    
    Message msg(MessageType::USER_LIST, username_, "");
    network_.sendMessage(msg);
}

bool Client::isConnected() const {
    return connected_ && network_.isConnected();
}

void Client::onMessageReceived(const Message& msg) {
    ui_.displayMessage(msg);
}

void Client::onInputReceived(const std::string& input) {
    if (input.empty()) {
        return;
    }
    
    // Check for commands
    if (input[0] == '/') {
        handleCommand(input);
        return;
    }
    
    // Send as text message
    sendTextMessage(input);
}

void Client::handleCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "/quit" || cmd == "/exit") {
        disconnect();
        ui_.displaySystemMessage("Disconnected from server");
    } else if (cmd == "/users" || cmd == "/list") {
        requestUserList();
    } else if (cmd == "/help") {
        ui_.displaySystemMessage("Available commands:");
        ui_.displaySystemMessage("  /quit, /exit - Disconnect from server");
        ui_.displaySystemMessage("  /users, /list - List connected users");
        ui_.displaySystemMessage("  /help - Show this help message");
    } else {
        ui_.displaySystemMessage("Unknown command: " + cmd + ". Type /help for available commands.");
    }
}


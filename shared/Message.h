#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <chrono>
#include <cstdint>

enum class MessageType : uint8_t {
    TEXT = 0,
    JOIN = 1,
    LEAVE = 2,
    ERROR_MSG = 3,
    SYSTEM = 4,
    USER_LIST = 5
};

struct Message {
    MessageType type;
    std::string sender;
    std::string content;
    std::string timestamp;
    uint32_t messageId;
    
    Message() : type(MessageType::TEXT), messageId(0) {}
    
    Message(MessageType t, const std::string& s, const std::string& c)
        : type(t), sender(s), content(c), messageId(0) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        timestamp = std::to_string(time_t);
    }
};

#endif // MESSAGE_H


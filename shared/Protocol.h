#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>

// Protocol constants
constexpr uint32_t PROTOCOL_MAGIC = 0x43484154; // "CHAT"
constexpr uint16_t PROTOCOL_VERSION = 1;

// Message header structure (sent before each message)
struct MessageHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t messageType;
    uint32_t payloadSize;
    uint32_t messageId;
    
    MessageHeader() : magic(PROTOCOL_MAGIC), version(PROTOCOL_VERSION), 
                     messageType(0), payloadSize(0), messageId(0) {}
};

// Protocol message types
enum class ProtocolMessageType : uint16_t {
    CLIENT_HELLO = 100,
    SERVER_HELLO = 101,
    CLIENT_JOIN = 102,
    CLIENT_LEAVE = 103,
    TEXT_MESSAGE = 104,
    USER_LIST_REQUEST = 105,
    USER_LIST_RESPONSE = 106,
    ERROR_MESSAGE = 107,
    HEARTBEAT = 108
};

// Protocol error codes
enum class ProtocolError : uint16_t {
    NONE = 0,
    INVALID_MESSAGE = 1,
    USERNAME_TAKEN = 2,
    SERVER_FULL = 3,
    UNAUTHORIZED = 4,
    INTERNAL_ERROR = 5
};

#endif // PROTOCOL_H


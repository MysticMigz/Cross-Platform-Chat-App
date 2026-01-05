#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "Message.h"
#include "Protocol.h"
#include <vector>
#include <cstring>
#include <sstream>

class Serializer {
public:
    // Serialize a message to binary format
    static std::vector<uint8_t> serialize(const Message& msg) {
        std::vector<uint8_t> buffer;
        
        // Serialize header
        MessageHeader header;
        header.messageType = static_cast<uint16_t>(msg.type);
        header.messageId = msg.messageId;
        
        // Calculate payload size
        uint32_t payloadSize = sizeof(uint32_t) + // sender length
                              msg.sender.size() +
                              sizeof(uint32_t) + // content length
                              msg.content.size() +
                              sizeof(uint32_t) + // timestamp length
                              msg.timestamp.size();
        
        header.payloadSize = payloadSize;
        
        // Write header
        buffer.resize(sizeof(MessageHeader) + payloadSize);
        std::memcpy(buffer.data(), &header, sizeof(MessageHeader));
        
        size_t offset = sizeof(MessageHeader);
        
        // Write sender
        uint32_t len = static_cast<uint32_t>(msg.sender.size());
        std::memcpy(buffer.data() + offset, &len, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::memcpy(buffer.data() + offset, msg.sender.data(), len);
        offset += len;
        
        // Write content
        len = static_cast<uint32_t>(msg.content.size());
        std::memcpy(buffer.data() + offset, &len, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::memcpy(buffer.data() + offset, msg.content.data(), len);
        offset += len;
        
        // Write timestamp
        len = static_cast<uint32_t>(msg.timestamp.size());
        std::memcpy(buffer.data() + offset, &len, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::memcpy(buffer.data() + offset, msg.timestamp.data(), len);
        
        return buffer;
    }
    
    // Deserialize a message from binary format
    static bool deserialize(const std::vector<uint8_t>& buffer, Message& msg) {
        if (buffer.size() < sizeof(MessageHeader)) {
            return false;
        }
        
        MessageHeader header;
        std::memcpy(&header, buffer.data(), sizeof(MessageHeader));
        
        // Validate magic number
        if (header.magic != PROTOCOL_MAGIC) {
            return false;
        }
        
        msg.type = static_cast<MessageType>(header.messageType);
        msg.messageId = header.messageId;
        
        size_t offset = sizeof(MessageHeader);
        
        // Read sender
        if (offset + sizeof(uint32_t) > buffer.size()) return false;
        uint32_t len;
        std::memcpy(&len, buffer.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        if (offset + len > buffer.size()) return false;
        msg.sender.assign(reinterpret_cast<const char*>(buffer.data() + offset), len);
        offset += len;
        
        // Read content
        if (offset + sizeof(uint32_t) > buffer.size()) return false;
        std::memcpy(&len, buffer.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        if (offset + len > buffer.size()) return false;
        msg.content.assign(reinterpret_cast<const char*>(buffer.data() + offset), len);
        offset += len;
        
        // Read timestamp
        if (offset + sizeof(uint32_t) > buffer.size()) return false;
        std::memcpy(&len, buffer.data() + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        if (offset + len > buffer.size()) return false;
        msg.timestamp.assign(reinterpret_cast<const char*>(buffer.data() + offset), len);
        
        return true;
    }
    
    // Serialize a simple string message (for quick messages)
    static std::vector<uint8_t> serializeString(const std::string& str) {
        std::vector<uint8_t> buffer;
        uint32_t len = static_cast<uint32_t>(str.size());
        buffer.resize(sizeof(uint32_t) + len);
        std::memcpy(buffer.data(), &len, sizeof(uint32_t));
        std::memcpy(buffer.data() + sizeof(uint32_t), str.data(), len);
        return buffer;
    }
    
    // Deserialize a simple string message
    static bool deserializeString(const std::vector<uint8_t>& buffer, std::string& str) {
        if (buffer.size() < sizeof(uint32_t)) {
            return false;
        }
        uint32_t len;
        std::memcpy(&len, buffer.data(), sizeof(uint32_t));
        if (buffer.size() < sizeof(uint32_t) + len) {
            return false;
        }
        str.assign(reinterpret_cast<const char*>(buffer.data() + sizeof(uint32_t)), len);
        return true;
    }
};

#endif // SERIALIZER_H


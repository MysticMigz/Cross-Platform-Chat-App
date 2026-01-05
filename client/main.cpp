#include "Client.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    uint16_t port = 8080;
    std::string username;
    
    // Parse command line arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <username> [host] [port]" << std::endl;
        std::cout << "Example: " << argv[0] << " Alice 127.0.0.1 8080" << std::endl;
        return 1;
    }
    
    username = argv[1];
    
    if (argc > 2) {
        host = argv[2];
    }
    
    if (argc > 3) {
        port = static_cast<uint16_t>(std::atoi(argv[3]));
    }
    
    Client client;
    
    std::cout << "Connecting to " << host << ":" << port << " as " << username << "..." << std::endl;
    
    if (!client.connect(host, port, username)) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    
    // Main loop - client runs until disconnected
    while (client.isConnected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\nDisconnected from server. Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return 0;
}


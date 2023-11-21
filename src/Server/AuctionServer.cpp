// AuctionServer.cpp

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>

// Define necessary data structures for auctions, users, etc.

struct Auction {
    int auctionID;
    std::string name;
    std::string assetFileName;
    // Add other relevant fields
};

struct Bid {
    int userID;
    int auctionID;
    int value;
    // Add other relevant fields
};

struct User {
    std::string userID;
    std::string password;
    bool loggedIn;
    // Add other relevant fields
};

class AuctionServer {
private:
    std::vector<Auction> auctions;
    std::vector<Bid> bids;
    std::unordered_map<std::string, User> users;

public:
    // Implement necessary functions for handling requests
    void handleLogin(std::string userID, std::string password);
    void handleOpenAuction(std::string userID, std::string name, std::string assetFileName, int startValue, int duration);
    // Add other functions based on your specifications
};

int main() {
    // Implement the main loop for the Auction Server
    AuctionServer auctionServer;
    // Set up and listen for incoming connections
    // Handle incoming requests
    return 0;
}

void AuctionServer::communicateWithServerUDP(const std::string& message) {
    // Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Cannot open socket\n";
        return;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(std::stoi(ASport));
    if (inet_pton(AF_INET, ASIP.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address\n";
        return;
    }

    // Send the message to the server
    if (sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Cannot send message\n";
        return;
    }

    // Close the socket
    close(sockfd);
}

void AuctionServer::handleLogin(std::string userID, std::string password) {
     // Check if the user is already registered
    auto it = users.find(userID);
    if (it != users.end()) {
        // The user is already registered, check the password
        if (it->second.password == password) {
            // The password is correct, log the user in
            it->second.loggedIn = true;
            // Send a reply with status = OK
            std::string message = "RLI OK";
            communicateWithServerUDP(message);
        } else {
            // The password is incorrect, send a reply with status = NOK
            std::string message = "RLI NOK";
            communicateWithServerUDP(message);
        }
    } else {
        // The user is not registered, register and log the user in
        User newUser;
        newUser.userID = userID;
        newUser.password = password;
        newUser.loggedIn = true;
        users[userID] = newUser;
        // Send a reply with status = REG
        std::string message = "RLI REG";
        communicateWithServerUDP(message);
    }
}


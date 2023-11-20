#ifndef USER_H
#define USER_H

#include "common/game.hpp"

class User { // User class
private:
    std::string username;
    std::string password;
    std::vector<Auction> auctions;
    std::vector<Bid> bids;

public:
    User(std::string username, std::string password);
    std::string getUsername();
    std::string getPassword();
    std::vector<Auction> getAuctions();
    std::vector<Bid> getBids();
    void addAuction(Auction auction);
    void addBid(Bid bid);
};



#endif
#ifndef PACKET_HANDLERS_H
#define PACKET_HANDLERS_H

#include <sstream>

#include "../common/constants.hpp"
#include "auction_server_state.hpp"

class userTag {  // IOManip helper
  uint32_t user_id;

 public:
  explicit userTag(uint32_t __user_id) : user_id{__user_id} {}
  friend std::ostream &operator<<(std::ostream &os, const userTag &obj) {
    os << "[user " << std::setfill('0') << std::setw(USER_ID_MAX_LEN)
       << obj.user_id << "] ";
    return os;
  }
};

class auctionTag {  // IOManip helper
  uint32_t auction_id;

 public:
  explicit auctionTag(uint32_t __auction_id) : auction_id{__auction_id} {}
  friend std::ostream &operator<<(std::ostream &os, const auctionTag &obj) {
    os << "[auction " << std::setfill('0') << std::setw(AUCTION_ID_MAX_LEN)
       << obj.auction_id << "] ";
    return os;
  }
};

// UDP

void handle_login_user(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state);

void handle_logout_user(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state);

void handle_unregister_user(std::stringstream &buffer, Address &addr_from,
                            AuctionServerState &state);

void handle_list_myauctions(std::stringstream &buffer, Address &addr_from,
                            AuctionServerState &state);

void handle_list_mybids(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state);

void handle_list_auctions(std::stringstream &buffer, Address &addr_from,
                          AuctionServerState &state);

void handle_show_record(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state);


// TCP
void handle_open_auction(int connection_fd, AuctionServerState &state);

void handle_close_auction(int connection_fd, AuctionServerState &state);

void handle_show_asset(int connection_fd, AuctionServerState &state);

void handle_bid(int connection_fd, AuctionServerState &state);


#endif
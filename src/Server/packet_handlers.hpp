#ifndef PACKET_HANDLERS_H
#define PACKET_HANDLERS_H

#include <sstream>

#include "common/constants.hpp"
#include "auction_server_state.hpp"

class userTag {  // IOManip helper
  uint32_t user_id;

 public:
  explicit userTag(uint32_t __user_id) : user_id{__user_id} {}
  friend std::ostream &operator<<(std::ostream &os, const userTag &obj) {
    os << "[user " << std::setfill('0') << std::setw(user_ID_MAX_LEN)
       << obj.user_id << "] ";
    return os;
  }
};

// UDP

void handle_login_user(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state);


// TCP

void handle_scoreboard(int connection_fd, AuctionServerState &state);


void handle_state(int connection_fd, AuctionServerState &state);

#endif
#ifndef SERVER_STATE_H
#define SERVER_STATE_H

#include <netdb.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>


#include "../common/protocol.hpp"
#include "../common/exceptions.hpp"
#include "../common/file_manager.hpp"
#include "../common/constants.hpp"
#include "../common/auction_data.hpp"
#include "user_data.hpp"

class Address {
 public:
  int socket;
  struct sockaddr_in addr;
  socklen_t size;
};


class DebugStream {
  bool active;

 public:
  DebugStream(bool __active) : active{__active} {};

  template <class T>
  DebugStream& operator<<(T val) {
    if (active) {
      std::cout << val;
    }
    return *this;
  }

  DebugStream& operator<<(std::ostream& (*f)(std::ostream&)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  DebugStream& operator<<(std::ostream& (*f)(std::ios&)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }

  DebugStream& operator<<(std::ostream& (*f)(std::ios_base&)) {
    if (active) {
      f(std::cout);
    }
    return *this;
  }
};

class AuctionServerState;

typedef void (*UdpPacketHandler)(std::stringstream&, Address&,
                                 AuctionServerState&);
typedef void (*TcpPacketHandler)(int connection_fd, AuctionServerState&);

class AuctionServerState {
  std::unordered_map<std::string, UdpPacketHandler> udp_packet_handlers;
  std::unordered_map<std::string, TcpPacketHandler> tcp_packet_handlers;
  
  std::mutex AuctionsLock;
  std::mutex UsersLock;
  
  void setup_sockets();

 public:
  int udp_socket_fd = -1;
  int tcp_socket_fd = -1;
  struct addrinfo* server_udp_addr = NULL;
  struct addrinfo* server_tcp_addr = NULL;
  DebugStream cdebug;
  u_int32_t auctionsCount = 0;
  FileManager& file_manager;


  AuctionServerState ( std::string& port, bool __verbose, FileManager& __file_manager );
  ~AuctionServerState();
  void resolveServerAddress(std::string& port);
  void registerPacketHandlers();
  void callUdpPacketHandler(std::string packet_id, std::stringstream& stream,
                            Address& addr_from);
  void callTcpPacketHandler(std::string packet_id, int connection_fd);
};

/** Exceptions **/

// There is an on-going Auction with a user ID
class AuctionAlreadyStartedException : public std::runtime_error {
 public:
  AuctionAlreadyStartedException()
      : std::runtime_error(
            "There is already an on-going Auction with this user ID.") {}
};

// There is no on-going Auction with a user ID
class NoAuctionFoundException : public std::runtime_error {
 public:
  NoAuctionFoundException()
      : std::runtime_error("There is no on-going Auction with this user ID.") {}
};

#endif
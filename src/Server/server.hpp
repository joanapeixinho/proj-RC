#ifndef SERVER_H
#define SERVER_H

#include <csignal>

#include "common/constants.hpp"
#include "auction_server.hpp"
#include "auction_server_state.hpp"
#include "worker_pool.hpp"

class Server {
 public:
  char* programPath;
  std::string port = DEFAULT_PORT;
  bool verbose = false;
  Server(int argc, char* argv[]);
};

void main_tcp(AuctionServerState& state);

void wait_for_udp_packet(AuctionServerState& server_state);

void handle_packet(std::stringstream& buffer, Address& addr_from,
                   AuctionServerState& server_state);

void wait_for_tcp_packet(AuctionServerState& server_state, WorkerPool& pool);

void init_server(AuctionServerState& server_state);

#endif
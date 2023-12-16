#include "auction_server_state.hpp"

#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "../common/common.hpp"
#include "../common/protocol.hpp"
#include "packet_handlers.hpp"

AuctionServerState::AuctionServerState(std::string &port, bool __verbose, FileManager& fileManager, uint32_t __auctionsCount)
    : cdebug{DebugStream(__verbose)} , file_manager{fileManager} { 
  this->setup_sockets();
  this->resolveServerAddress(port);
  this->registerPacketHandlers();
  this->auctionsCount = __auctionsCount;
  
}

AuctionServerState::~AuctionServerState() {
  if (this->udp_socket_fd != -1) {
    close(this->udp_socket_fd);
  }
  if (this->tcp_socket_fd != -1) {
    close(this->tcp_socket_fd);
  }
  if (this->server_udp_addr != NULL) {
    freeaddrinfo(this->server_udp_addr);
  }
  if (this->server_tcp_addr != NULL) {
    freeaddrinfo(this->server_tcp_addr);
  }
}

void AuctionServerState::registerPacketHandlers() {
  // UDP
  udp_packet_handlers.insert({LoginServerbound::ID, handle_login_user});
  udp_packet_handlers.insert({LogoutServerbound::ID, handle_logout_user});
  udp_packet_handlers.insert({UnregisterServerbound::ID, handle_unregister_user});
  udp_packet_handlers.insert({ListMyAuctionsServerbound::ID, handle_list_myauctions});
  udp_packet_handlers.insert({MyBidsServerbound::ID, handle_list_mybids});
  udp_packet_handlers.insert({ListAuctionsServerbound::ID, handle_list_auctions});
  udp_packet_handlers.insert({ShowRecordServerbound::ID, handle_show_record});

  // TCP
  tcp_packet_handlers.insert({OpenAuctionServerbound::ID, handle_open_auction});
  tcp_packet_handlers.insert({CloseAuctionServerbound::ID, handle_close_auction});
  tcp_packet_handlers.insert({ShowAssetServerbound::ID, handle_show_asset});
  tcp_packet_handlers.insert({BidServerbound::ID, handle_bid});

}

void AuctionServerState::setup_sockets() {
  // Create a UDP socket
  if ((this->udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw UnrecoverableError("Failed to create a UDP socket", errno);
  }
  struct timeval read_timeout_udp;
  read_timeout_udp.tv_sec = SERVER_RECV_RESTART_TIMEOUT_SECONDS;
  read_timeout_udp.tv_usec = 0;
  if (setsockopt(this->udp_socket_fd, SOL_SOCKET, SO_RCVTIMEO,
                 &read_timeout_udp, sizeof(read_timeout_udp)) < 0) {
    throw UnrecoverableError("Failed to set UDP read timeout socket option",
                             errno);
  }

  // Create a TCP socket
  if ((this->tcp_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    throw UnrecoverableError("Failed to create a TCP socket", errno);
  }
  const int enable = 1;
  if (setsockopt(this->tcp_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable,
                 sizeof(int)) < 0) {
    throw UnrecoverableError("Failed to set TCP reuse address socket option",
                             errno);
  }
  struct timeval read_timeout;
  read_timeout.tv_sec = SERVER_RECV_RESTART_TIMEOUT_SECONDS;
  read_timeout.tv_usec = 0;
  if (setsockopt(this->tcp_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout,
                 sizeof(read_timeout)) < 0) {
    throw UnrecoverableError("Failed to set TCP read timeout socket option",
                             errno);
  }
  struct timeval write_timeout;
  write_timeout.tv_sec = TCP_WRITE_TIMEOUT_SECONDS;
  write_timeout.tv_usec = 0;
  if (setsockopt(this->tcp_socket_fd, SOL_SOCKET, SO_SNDTIMEO, &write_timeout,
                 sizeof(write_timeout)) < 0) {
    throw UnrecoverableError("Failed to set TCP write timeout socket option",
                             errno);
  }
}

void AuctionServerState::resolveServerAddress(std::string &port) {
  struct addrinfo hints;
  int addr_res;
  const char *port_str = port.c_str();
  // Get UDP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;       // IPv4
  hints.ai_socktype = SOCK_DGRAM;  // UDP socket
  hints.ai_flags = AI_PASSIVE;     // Listen on 0.0.0.0
  if ((addr_res =
           getaddrinfo(NULL, port_str, &hints, &this->server_udp_addr)) != 0) {
    throw UnrecoverableError(
        std::string("Failed to get address for UDP connection: ") +
        gai_strerror(addr_res));
  }
  // bind socket
  if (bind(this->udp_socket_fd, this->server_udp_addr->ai_addr,
           this->server_udp_addr->ai_addrlen)) {
    throw UnrecoverableError("Failed to bind UDP address", errno);
  }

  // Get TCP address
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;        // IPv4
  hints.ai_socktype = SOCK_STREAM;  // TCP socket
  hints.ai_flags = AI_PASSIVE;      // Listen on 0.0.0.0
  if ((addr_res = getaddrinfo(NULL, port.c_str(), &hints,
                              &this->server_tcp_addr)) != 0) {
    throw UnrecoverableError(
        std::string("Failed to get address for TCP connection: ") +
        gai_strerror(addr_res));
  }

  if (bind(this->tcp_socket_fd, this->server_tcp_addr->ai_addr,
           this->server_tcp_addr->ai_addrlen)) {
    throw UnrecoverableError("Failed to bind TCP address", errno);
  }

  std::cout << "Listening for connections on port " << port << std::endl;
}


void AuctionServerState::callUdpPacketHandler(std::string packet_id,
                                           std::stringstream &stream,
                                           Address &addr_from) {
  auto handler = this->udp_packet_handlers.find(packet_id);
  if (handler == this->udp_packet_handlers.end()) {
    cdebug << "Received unknown Packet ID" << std::endl;
    throw InvalidPacketException();
  }

  handler->second(stream, addr_from, *this);
}

void AuctionServerState::callTcpPacketHandler(std::string packet_id,
                                           int connection_fd) {
  auto handler = this->tcp_packet_handlers.find(packet_id);
  if (handler == this->tcp_packet_handlers.end()) {
    cdebug << "Received unknown Packet ID" << std::endl;
    throw InvalidPacketException();
  }

  handler->second(connection_fd, *this);
}

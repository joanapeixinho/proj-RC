#ifndef USER_STATE_H
#define USER_STATE_H

#include <netdb.h>

// #include "client_game.hpp"
#include "common/protocol.hpp"

class UserState {
  int udp_socket_fd = -1;
  int tcp_socket_fd = -1;
  struct addrinfo* server_udp_addr = NULL;
  struct addrinfo* server_tcp_addr = NULL;

  void setupSockets();
  void resolveServerAddress(std::string& hostname, std::string& port);
  void sendUdpPacket(UdpPacket& packet);
  void waitForUdpPacket(UdpPacket& packet);
  void openTcpSocket();
  void sendTcpPacket(TcpPacket& packet);
  void waitForTcpPacket(TcpPacket& packet);
  void closeTcpSocket();

 public:
  // ClientGame* game = NULL;

  UserState(std::string& hostname, std::string& port);
  ~UserState();
  // bool hasActiveGame();
  // bool hasGame();
  // void startGame(ClientGame* game);
  void sendUdpPacketAndWaitForReply(UdpPacket& out_packet,
                                    UdpPacket& in_packet);
  void sendTcpPacketAndWaitForReply(TcpPacket& out_packet,
                                    TcpPacket& in_packet);
};

#endif
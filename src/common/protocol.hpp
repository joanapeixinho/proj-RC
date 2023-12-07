#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sys/socket.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "constants.hpp"

// Thrown when the PacketID does not match what was expected
class UnexpectedPacketException : public std::runtime_error {
 public:
  UnexpectedPacketException()
      : std::runtime_error(
            "The server did not reply with the expected response, so it was "
            "ignored. Please try again or connect to a different Auction "
            "server.") {}
};

// Thrown when the PacketID is correct, but the schema is wrong
class InvalidPacketException : public std::runtime_error {
 public:
  InvalidPacketException()
      : std::runtime_error(
            "The response given by the server is not correctly structured, so "
            "it was ignored. Please try again or connect to a different Auction "
            "server.") {}
};

// Thrown when serialization error occurs
class PacketSerializationException : public std::runtime_error {
 public:
  PacketSerializationException()
      : std::runtime_error(
            "There was an error while preparing a request, "
            "please try again and restart the application if the problem "
            "persists.") {}
};

// Thrown when timeout for reading/writing packet occurs
class ConnectionTimeoutException : public std::runtime_error {
 public:
  ConnectionTimeoutException()
      : std::runtime_error(
            "Could not connect to the Auction server, please check your internet "
            "connection and try again.") {}
};

// Thrown when an error related to I/O occurs
class IOException : public std::runtime_error {
 public:
  IOException()
      : std::runtime_error(
            "IO error while reading/writting from/to filesystem") {}
};

// Thrown when an error related to I/O occurs
class OperationCancelledException : public std::runtime_error {
 public:
  OperationCancelledException()
      : std::runtime_error("Operation cancelled by user") {}
};

class UdpPacket {
 private:
  void readChar(std::stringstream &buffer, char chr);

 protected:
  void readPacketId(std::stringstream &buffer, const char *id);
  void readSpace(std::stringstream &buffer);
  char readChar(std::stringstream &buffer);
  char readAlphabeticalChar(std::stringstream &buffer);
  void readPacketDelimiter(std::stringstream &buffer);
  std::string readString(std::stringstream &buffer, uint32_t max_len);
  std::string readAlphabeticalString(std::stringstream &buffer,
                                     uint32_t max_len);
  uint32_t readInt(std::stringstream &buffer);
  uint32_t readUserId(std::stringstream &buffer);

 public:
  virtual std::stringstream serialize() = 0;
  virtual void deserialize(std::stringstream &buffer) = 0;

  virtual ~UdpPacket() = default;
};


// Start New Auction Packet (LIN)
class LoginServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LIN";
  uint32_t user_id;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to Start Auction Packet (RLI)
class ReplyLoginClientbound : public UdpPacket {
 public:
  enum status { OK, NOK, REG, ERR };
  static constexpr const char *ID = "RLI";
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Logout Packet (LOU)
class LogoutServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LOU";
  uint32_t user_id;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to Logout Packet (RLO)
class ReplyLogoutClientbound : public UdpPacket {
 public:
  enum status { OK, NOK, UNR};
  static constexpr const char *ID = "RLI";
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Unregister Packet (UNR)
class UnregisterServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "UNR";
  uint32_t user_id;
  std::string password;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to Unregister Packet (RUN)
class ReplyUnregisterClientbound : public UdpPacket {
 public:
  enum status { OK, NOK, UNR };
  static constexpr const char *ID = "RUN";
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// List MyAuctions Packet (LMA)

class ListMyAuctionsServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LMA";
  uint32_t user_id;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to List MyAuctions Packet (RLM)

class ReplyListMyAuctionsClientbound : public UdpPacket {
 public:
  enum status { OK, NLG, NOK };
  static constexpr const char *ID = "RLM";
  std::vector<std::pair<uint32_t, bool>> myAuctions;
  
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// List my bids Packet (LMB)

class ListBidsServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LMB";
  uint32_t user_id;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to List my bids Packet (RMB)

class ReplyListBidsClientbound : public UdpPacket {
 public:
  enum status { OK, NOK, NLG };
  static constexpr const char *ID = "RMB";
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// List Auctions Packet (LST)

class ListAuctionsServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LST";
  uint32_t user_id;

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

// Reply to List Auctions Packet (RLS)

class ReplyListAuctionsClientbound : public UdpPacket {
 public:
  enum status { OK, NOK };
  static constexpr const char *ID = "RLS";
  status status;
  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};


class ErrorUdpPacket : public UdpPacket {
 public:
  static constexpr const char *ID = "ERR";

  std::stringstream serialize();
  void deserialize(std::stringstream &buffer);
};

class MyAuctionsServerbound : public UdpPacket {
 public:
  static constexpr const char *ID = "LMA";
  uint32_t user_id;

  void send(int fd);
  void receive(int fd);
};


class TcpPacket {
 private:
  char delimiter = 0;

  void readChar(int fd, char chr);

 protected:
  void writeString(int fd, const std::string &str);
  void readPacketId(int fd, const char *id);
  void readSpace(int fd);
  char readChar(int fd);
  void readPacketDelimiter(int fd);
  std::string readString(const int fd);
  uint32_t readInt(const int fd);
  uint32_t readUserId(const int fd);
  void readAndSaveToFile(const int fd, const std::string &file_name,
                         const size_t file_size, const bool cancellable);

 public:
  virtual void send(int fd) = 0;
  virtual void receive(int fd) = 0;

  virtual ~TcpPacket() = default;
};

class OpenAuctionServerbound : public TcpPacket {
 public:
  static constexpr const char *ID = "OPA";
  uint32_t user_id;
  std::string password;
  std::string auction_name;
  uint32_t start_value;
  uint32_t time_active;
  std::string file_name;
  std::filesystem::path file_path;

  void send(int fd);
  void receive(int fd);
};

class ReplyOpenAuctionClientbound : public TcpPacket {
 public:
  enum status { OK, NOK, NLG };
  static constexpr const char *ID = "ROA";
  status status;
  uint32_t auction_id; //TODO: confirmar que é deste tipo o ID

  void send(int fd);
  void receive(int fd);
};

class CloseAuctionServerbound : public TcpPacket {
 public:
  static constexpr const char *ID = "CLS";
  uint32_t user_id;
  std::string password;
  uint32_t auction_id;

  void send(int fd);
  void receive(int fd);
};

class ReplyCloseAuctionClientbound : public TcpPacket {
 public:
  enum status { OK, EAU, EOW, END, NLG };
  static constexpr const char *ID = "RCL";
  status status;

  void send(int fd);
  void receive(int fd);
};


class ScoreboardServerbound : public TcpPacket {
 public:
  static constexpr const char *ID = "GSB";

  void send(int fd);
  void receive(int fd);
};

class ScoreboardClientbound : public TcpPacket {
 public:
  enum status { OK, EMPTY };
  static constexpr const char *ID = "RSB";
  status status;
  std::string file_name;
  std::string file_data;

  void send(int fd);
  void receive(int fd);
};

class StateServerbound : public TcpPacket {
 public:
  static constexpr const char *ID = "STA";
  uint32_t user_id;

  void send(int fd);
  void receive(int fd);
};

class StateClientbound : public TcpPacket {
 public:
  enum status { ACT, FIN, NOK };
  static constexpr const char *ID = "RST";
  status status;
  std::string file_name;
  std::string file_data;

  void send(int fd);
  void receive(int fd);
};



class ErrorTcpPacket : public TcpPacket {
 public:
  static constexpr const char *ID = "ERR";

  void send(int fd);
  void receive(int fd);
};

void send_packet(UdpPacket &packet, int socket, struct sockaddr *address,
                 socklen_t addrlen);

void wait_for_packet(UdpPacket &packet, int socket);

void write_user_id(std::stringstream &buffer, const uint32_t user_id);

uint32_t parse_packet_user_id(std::string &id_str);

void sendFile(int connection_fd, std::filesystem::path image_path);

uint32_t getFileSize(std::filesystem::path file_path);

#endif
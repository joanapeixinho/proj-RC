#include "protocol.hpp"

#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "common.hpp"
#include "protocol.hpp"

extern bool is_shutting_down;

void UdpPacket::readPacketId(std::stringstream &buffer, const char *packet_id) {
  char current_char;
  const char *errorID = ErrorUdpPacket::ID;
  int errorID_counter = 0;
  while (*packet_id != '\0') {
    buffer >> current_char;
    if (!buffer.good() || current_char != *packet_id) {
      // Check if the unexpected packet is an error packet
      if (current_char != errorID[errorID_counter]) {

        // If it is not an error packet, throw an exception
        throw UnexpectedPacketException();
      } else {

        // While the packet seems to be an error packet, keep reading
        errorID_counter++;
      }

      // If it's confirmed that the packet is an error packet, throw an
      // exception
      if (errorID_counter == 3) {
        // Without the packet delimiter, the error packet would be invalid
        readPacketDelimiter(buffer);
        throw ErrorUdpPacketException();
      }
    }
    ++packet_id;
  }
}

void UdpPacket::readChar(std::stringstream &buffer, char chr) {
  if (readChar(buffer) != chr) {
    throw InvalidPacketException();
  }
}

char UdpPacket::readChar(std::stringstream &buffer) {
  char c;
  buffer >> c;
  if (!buffer.good()) {
    throw InvalidPacketException();
  }
  return c;
}

char UdpPacket::readAlphabeticalChar(std::stringstream &buffer) {
  char c;
  buffer >> c;

  if (!buffer.good() || !isalpha((unsigned char)c)) {
    throw InvalidPacketException();
  }
  return (char)tolower((unsigned char)c);
}

void UdpPacket::readSpace(std::stringstream &buffer) { readChar(buffer, ' '); }

void UdpPacket::readPacketDelimiter(std::stringstream &buffer) {
  readChar(buffer, '\n');
  buffer.peek();
  if (!buffer.eof()) { // If there is more data in the buffer, the packet is
                       // invalid
    throw InvalidPacketException();
  }
}

std::string UdpPacket::readString(std::stringstream &buffer, uint32_t max_len) {
  std::string str;
  uint32_t i = 0;
  while (i < max_len) {
    char c = (char)buffer.get();
    if (!buffer.good()) {
      throw InvalidPacketException();
    }
    if (c == ' ' || c == '\n') {
      buffer.unget();
      break;
    }
    str += c;
    ++i;
  }
  return str;
};

uint32_t UdpPacket::readInt(std::stringstream &buffer) {
  int64_t i;
  buffer >> i;
  if (!buffer.good() || i < 0 || i > INT32_MAX) {
    throw InvalidPacketException();
  }
  return (uint32_t)i;
};

uint32_t UdpPacket::readUserId(std::stringstream &buffer) {
  std::string id_str = readString(buffer, USER_ID_STR_LEN);
  return parse_packet_user_id(id_str);
};

uint32_t UdpPacket::readAuctionId(std::stringstream &buffer) {
  std::string id_str = readString(buffer, AUCTION_ID_MAX_LEN);
  return parse_packet_auction_id(id_str);
};

// ----- Packet type seriliazation and deserialization methods -----
std::stringstream LoginServerbound::serialize() {
  std::stringstream buffer;
  buffer << LoginServerbound::ID << " ";
  write_user_id(buffer, user_id);
  buffer << " " << password;
  buffer << std::endl;
  return buffer;
};

void LoginServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  user_id = readUserId(buffer);
  readSpace(buffer);
  password = readString(buffer, PASSWORD_LEN);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyLoginClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyLoginClientbound::ID << " ";
  if (status == ReplyLoginClientbound::status::OK) {
    buffer << "OK";
  } else if (status == ReplyLoginClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyLoginClientbound::status::REG) {
    buffer << "REG";
  } else if (status == ReplyLoginClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
}

void ReplyLoginClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyLoginClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "REG") {
    status = REG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream LogoutServerbound::serialize() {
  std::stringstream buffer;
  buffer << LogoutServerbound::ID << " ";
  write_user_id(buffer, user_id);
  buffer << " " << password;
  buffer << std::endl;
  return buffer;
};

void LogoutServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  user_id = readUserId(buffer);
  readSpace(buffer);
  password = readString(buffer, PASSWORD_LEN);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyLogoutClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyLogoutClientbound::ID << " ";
  if (status == ReplyLogoutClientbound::status::OK) {
    buffer << "OK";
  } else if (status == ReplyLogoutClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyLogoutClientbound::status::UNR) {
    buffer << "UNR";
  } else if (status == ReplyLogoutClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyLogoutClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyLogoutClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "UNR") {
    status = UNR;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream UnregisterServerbound::serialize() {
  std::stringstream buffer;
  buffer << UnregisterServerbound::ID << " ";
  write_user_id(buffer, user_id);
  buffer << " " << password << std::endl;
  return buffer;
};

void UnregisterServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  user_id = readUserId(buffer);
  readSpace(buffer);
  password = readString(buffer, PASSWORD_LEN);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyUnregisterClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyUnregisterClientbound::ID << " ";
  if (status == ReplyUnregisterClientbound::status::OK) {
    buffer << "OK";
  } else if (status == ReplyUnregisterClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyUnregisterClientbound::status::UNR) {
    buffer << "UNR";
  } else if (status == ReplyUnregisterClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyUnregisterClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyUnregisterClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "UNR") {
    status = UNR;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ListMyAuctionsServerbound::serialize() {
  std::stringstream buffer;
  buffer << ListMyAuctionsServerbound::ID << " ";
  write_user_id(buffer, user_id);
  buffer << std::endl;
  return buffer;
};

void ListMyAuctionsServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  user_id = readUserId(buffer);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyListMyAuctionsClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyListMyAuctionsClientbound::ID << " ";
  if (status == ReplyListMyAuctionsClientbound::status::OK) {
    buffer << "OK " << auctionsToString(auctions);
  } else if (status == ReplyListMyAuctionsClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyListMyAuctionsClientbound::status::NLG) {
    buffer << "NLG";
  } else if (status == ReplyListMyAuctionsClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyListMyAuctionsClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyListMyAuctionsClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
    auctions = readAuctions(buffer);
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "NLG") {
    status = NLG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream MyBidsServerbound::serialize() {
  std::stringstream buffer;
  buffer << MyBidsServerbound::ID << " ";
  write_user_id(buffer, user_id);
  buffer << std::endl;
  return buffer;
};

void MyBidsServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  user_id = readUserId(buffer);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyMyBidsClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyMyBidsClientbound::ID << " ";
  if (status == ReplyMyBidsClientbound::status::OK) {
    buffer << "OK " << auctionsToString(auctions);
  } else if (status == ReplyMyBidsClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyMyBidsClientbound::status::NLG) {
    buffer << "NLG";
  } else if (status == ReplyMyBidsClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyMyBidsClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyMyBidsClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
    auctions = readAuctions(buffer);
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "NLG") {
    status = NLG;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ListAuctionsServerbound::serialize() {
  std::stringstream buffer;
  buffer << ListAuctionsServerbound::ID << std::endl;
  return buffer;
};

void ListAuctionsServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readPacketDelimiter(buffer);
};

std::stringstream ReplyListAuctionsClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyListAuctionsClientbound::ID << " ";
  if (status == ReplyListAuctionsClientbound::status::OK) {
    buffer << "OK " << auctionsToString(auctions);
  } else if (status == ReplyListAuctionsClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyListAuctionsClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw PacketSerializationException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyListAuctionsClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyListAuctionsClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);
  if (status_str == "OK") {
    status = OK;
    auctions = readAuctions(buffer);
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ShowRecordServerbound::serialize() {
  std::stringstream buffer;
  buffer << ShowRecordServerbound::ID << " ";
  write_auction_id(buffer, auction_id);
  buffer << std::endl;
  return buffer;
};

void ShowRecordServerbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  // Serverbound packets don't read their ID
  readSpace(buffer);
  auction_id = readAuctionId(buffer);
  readPacketDelimiter(buffer);
};

std::stringstream ReplyShowRecordClientbound::serialize() {
  std::stringstream buffer;
  buffer << ReplyShowRecordClientbound::ID << " ";

  if (status == ReplyShowRecordClientbound::status::OK) {
    buffer << "OK ";
    write_user_id(buffer, auction.getOwnerId());
    buffer << " " << auction.getName() << " " << auction.getAssetFname();
    buffer << " " << auction.getInitialBid() << " ";
    buffer << auction.getStartTimeString() << " ";
    buffer << auction.getDurationSeconds();

    if (auction.hasBids()) {
      const std::vector<Bid> &bids = auction.getBids();
      std::vector<Bid>::size_type numBidsToRetrieve = 50;
      std::vector<Bid>::size_type totalBids = bids.size();
      std::vector<Bid>::size_type startIndex =
          totalBids > numBidsToRetrieve ? totalBids - numBidsToRetrieve : 0;

      for (std::vector<Bid>::size_type i = startIndex; i < totalBids; ++i) {
        // Access bids by index
        const Bid &currentBid = bids[i];
        // Perform operations with currentBid
        buffer << " B ";
        write_user_id(buffer, currentBid.bidder_user_id);
        buffer << " " << currentBid.bid_value;
        buffer << " " << currentBid.date_time;
        buffer << " " << currentBid.sec_time;
      }
    }
    if (!auction.isActive()) {
      buffer << " E ";
      buffer << auction.getEndTimeString();
      buffer << " " << auction.getEndTimeSec();
    }
  } else if (status == ReplyShowRecordClientbound::status::NOK) {
    buffer << "NOK";
  } else if (status == ReplyShowRecordClientbound::status::ERR) {
    buffer << "ERR";
  } else {
    throw InvalidPacketException();
  }
  buffer << std::endl;
  return buffer;
};

void ReplyShowRecordClientbound::deserialize(std::stringstream &buffer) {
  buffer >> std::noskipws;
  readPacketId(buffer, ReplyShowRecordClientbound::ID);
  readSpace(buffer);
  auto status_str = readString(buffer, PACKET_ID_LEN);

  if (status_str == "OK") {
    status = OK;
    readSpace(buffer);
    // Read auction data
    auction.setOwnerId(readUserId(buffer));
    readSpace(buffer);
    auction.setName(readString(buffer, AUCTION_NAME_MAX_LENGTH));
    readSpace(buffer);
    auction.setAssetFname(readString(buffer, ASSET_NAME_MAX_LENGTH));
    readSpace(buffer);
    auction.setInitialBid(readInt(buffer));
    readSpace(buffer);

    startTime = read_date_time(buffer);
    // Don't need to readSpace() because read date time reads an extra space

    auction.setDurationSeconds(readInt(buffer));

    // Read bids and end time
    int bidCounter = 0;
    if (buffer.peek() != '\n') { // Check if there are bids or end time
      while (buffer.peek() == ' ') {
        readSpace(buffer);
        if (buffer.peek() == 'B') { // Read bid
          auction.addBid(readBid(buffer));
          bidCounter++;
          if (bidCounter > 50) { // You cant receive more than 50 bids
            throw InvalidPacketException();
          }
        } else if (buffer.peek() == 'E') { // Read end time
          readChar(buffer);
          readSpace(buffer);
          endTime = read_date_time(buffer);
          auction.setEndTime(endTime);
          auction.setEndTimeSec(readInt(buffer));
          break;
        }
      }
    }
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(buffer);
};

std::stringstream ErrorUdpPacket::serialize() {
  std::stringstream buffer;
  buffer << ErrorUdpPacket::ID << std::endl;
  return buffer;
};

void ErrorUdpPacket::deserialize(std::stringstream &buffer) {
  (void)buffer;
  // unimplemented
};

void TcpPacket::writeString(int fd, const std::string &str) {
  const char *buffer = str.c_str();
  ssize_t bytes_to_send = (ssize_t)str.length();
  ssize_t bytes_sent = 0;
  while (bytes_sent < bytes_to_send) {
    ssize_t sent =
        write(fd, buffer + bytes_sent, (size_t)(bytes_to_send - bytes_sent));
    if (sent < 0) {
      throw PacketSerializationException();
    }
    bytes_sent += sent;
  }
}

void TcpPacket::readPacketId(int fd, const char *packet_id) {
  char current_char;
  const char *errorID = ErrorUdpPacket::ID;
  int errorID_counter = 0;
  while (*packet_id != '\0') {
    if (read(fd, &current_char, 1) != 1 || current_char != *packet_id) {
      // Check if the unexpected packet is an error packet
      if (current_char != errorID[errorID_counter]) {

        // If it is not an error packet, throw an exception
        throw UnexpectedPacketException();
      } else { // While the packet seems to be an error packet, keep reading
        errorID_counter++;
      }
      // If it's confirmed that the packet is an error packet, throw an
      // exception
      if (errorID_counter == 3) {
        // Without the packet delimiter, the error packet would be invalid
        readPacketDelimiter(fd);
        throw ErrorTcpPacketException();
      }
    }
    ++packet_id;
  }
}

void TcpPacket::readChar(int fd, char chr) {
  if (readChar(fd) != chr) {
    throw InvalidPacketException();
  }
}

char TcpPacket::readChar(int fd) {
  char c = 0;
  if (delimiter != 0) {
    // use last read char instead, since it wasn't consumed yet
    c = delimiter;
    delimiter = 0;
    return c;
  }
  if (read(fd, &c, 1) != 1) {
    throw InvalidPacketException();
  }
  return c;
}

void TcpPacket::readSpace(int fd) { readChar(fd, ' '); }

void TcpPacket::readPacketDelimiter(int fd) { readChar(fd, '\n'); }

std::string TcpPacket::readString(const int fd) {
  std::string result;
  char c = 0;

  while (!std::iswspace((wint_t)c)) {
    if (read(fd, &c, 1) != 1) {
      throw InvalidPacketException();
    }
    result += c;
  }
  delimiter = c;

  result.pop_back();

  return result;
}

uint32_t TcpPacket::readInt(const int fd) {
  std::string int_str = readString(fd);
  try {
    size_t converted = 0;
    int64_t result = static_cast<uint32_t>(std::stoi(int_str, &converted, 10));
    if (converted != int_str.length() || std::iswspace((wint_t)int_str.at(0)) ||
        result < 0 || result > INT32_MAX) {
      throw InvalidPacketException();
    }
    return (uint32_t)result;
  } catch (InvalidPacketException &ex) {
    throw ex;
  } catch (...) {
    throw InvalidPacketException();
  }
}

uint32_t TcpPacket::readUserId(const int fd) {
  std::string id_str = readString(fd);
  return parse_packet_user_id(id_str);
}

uint32_t TcpPacket::readAuctionId(const int fd) {
  std::string id_str = readString(fd);
  return parse_packet_auction_id(id_str);
}

uint32_t TcpPacket::readFileSize(const int fd) {
  std::string file_size_str = readString(fd);
  if (file_size_str.length() > ASSET_FILE_SIZE_MAX_LEN) {
    throw InvalidPacketException();
  }
  for (char c : file_size_str) {
    if (!isdigit(c)) {
      throw InvalidPacketException();
    }
  }
  try {
    int file_size = std::stoi(file_size_str);
    if (file_size < 0 || file_size > (int)ASSET_MAX_BYTES) {
      throw InvalidPacketException();
    }
    return (uint32_t)file_size;
  } catch (...) {
    throw InvalidPacketException();
  }
}

std::string TcpPacket::readFileName(const int fd) {
  std::string str = readString(fd);
  if (str.length() > ASSET_NAME_MAX_LENGTH) {
    throw InvalidPacketException();
  }
  for (uint32_t i = 0; i < str.length(); ++i) {
    char c = str[i];
    if (!isalpha((unsigned char)c) && !isdigit(c) && c != '-' && c != '_' &&
        c != '.') {
      throw InvalidPacketException();
    }
  }
  return str;
}

void TcpPacket::readAndSaveToFile(const int fd, const std::string &file_name,
                                  const size_t file_size,
                                  const bool cancellable) {
  std::ofstream file(file_name);
  if (!file.good()) {
    throw IOException();
  }

  size_t remaining_size = file_size;
  size_t to_read;
  ssize_t n;
  char buffer[FILE_BUFFER_LEN];

  if (cancellable) {
    std::cout << "Downloading file from server. Press ENTER to cancel download."
              << std::endl;
  }

  bool skip_stdin = false;
  while (remaining_size > 0) {
    fd_set file_descriptors;
    FD_ZERO(&file_descriptors);
    FD_SET(fd, &file_descriptors);
    if (!skip_stdin && cancellable) {
      FD_SET(fileno(stdin), &file_descriptors);
    }

    struct timeval timeout;
    timeout.tv_sec = TCP_READ_TIMEOUT_SECONDS;
    timeout.tv_usec = 0;

    int ready_fd = select(std::max(fd, fileno(stdin)) + 1, &file_descriptors,
                          NULL, NULL, &timeout);
    if (is_shutting_down) {
      std::cout << "Cancelling TCP download, user is shutting down..."
                << std::endl;
      throw OperationCancelledException();
    }
    if (ready_fd == -1) {
      perror("select");
      throw ConnectionTimeoutException();
    } else if (FD_ISSET(fd, &file_descriptors)) {
      // Read from socket
      to_read = std::min(remaining_size, (size_t)FILE_BUFFER_LEN);
      n = read(fd, buffer, to_read);
      if (n <= 0) {
        file.close();
        throw InvalidPacketException();
      }
      file.write(buffer, n);
      if (!file.good()) {
        file.close();
        throw IOException();
      }
      remaining_size -= (size_t)n;

      size_t downloaded_size = file_size - remaining_size;
      if (((downloaded_size - (size_t)n) * 100 / file_size) %
              PROGRESS_BAR_STEP_SIZE >
          (downloaded_size * 100 / file_size) % PROGRESS_BAR_STEP_SIZE) {
        std::cout << "Downloading Asset: " << downloaded_size * 100 / file_size
                  << "%" << std::endl;
      }
    } else if (FD_ISSET(fileno(stdin), &file_descriptors)) {
      if (std::cin.peek() != '\n') {
        skip_stdin = true;
        continue;
      }
      std::cin.get();
      std::cout << "Cancelling TCP download" << std::endl;
      throw OperationCancelledException();
    } else {
      throw ConnectionTimeoutException();
    }
  }

  file.close();
}

void OpenAuctionServerbound::send(int fd) {
  std::stringstream stream;
  file_size = getFileSize(file_path);

  stream << OpenAuctionServerbound::ID << " ";
  write_user_id(stream, user_id);
  stream << " " << password << " " << auction_name << " ";
  stream << start_value << " " << time_active << " ";
  stream << file_name << " " << file_size << " ";

  writeString(fd, stream.str());

  stream.str(std::string()); // clears the stream
  stream.clear();            // resets any error flags
  sendFile(fd, file_path);   // Send file_data

  writeString(fd, "\n"); // Send packet delimiter
}

void OpenAuctionServerbound::receive(int fd) {
  // Serverbound packets don't read their ID
  readSpace(fd);
  user_id = readUserId(fd);
  readSpace(fd);
  password = readString(fd);
  readSpace(fd);
  auction_name = readString(fd);
  readSpace(fd);
  start_value = readInt(fd);
  readSpace(fd);
  time_active = readInt(fd);
  readSpace(fd);
  file_name = readFileName(fd);
  readSpace(fd);
  file_size = readFileSize(fd);
  readSpace(fd);
  readAndSaveToFile(fd, file_name, file_size, false);
  readPacketDelimiter(fd);
}

void ReplyOpenAuctionClientbound::send(int fd) {
  std::stringstream stream;
  stream << ReplyOpenAuctionClientbound::ID << " ";

  if (status == OK) {
    stream << "OK ";
    write_auction_id(stream, auction_id);
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw PacketSerializationException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void ReplyOpenAuctionClientbound::receive(int fd) {
  readPacketId(fd, ReplyOpenAuctionClientbound::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    this->status = OK;
    readSpace(fd);
    this->auction_id = readAuctionId(fd);
  } else if (status_str == "NOK") {
    this->status = NOK;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void CloseAuctionServerbound::send(int fd) {
  std::stringstream stream;
  stream << CloseAuctionServerbound::ID << " ";
  write_user_id(stream, user_id);
  stream << " " << password << " ";
  write_auction_id(stream, auction_id);
  stream << std::endl;
  writeString(fd, stream.str());
}

void CloseAuctionServerbound::receive(int fd) {
  // Serverbound packets don't read their ID
  readSpace(fd);
  user_id = readUserId(fd);
  readSpace(fd);
  password = readString(fd);
  readSpace(fd);
  auction_id = readAuctionId(fd);
  readPacketDelimiter(fd);
}

void ReplyCloseAuctionClientbound::send(int fd) {
  std::stringstream stream;
  stream << ReplyCloseAuctionClientbound::ID << " ";
  if (status == OK) {
    stream << "OK";
  } else if (status == EAU) {
    stream << "EAU";
  } else if (status == EOW) {
    stream << "EOW";
  } else if (status == END) {
    stream << "END";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw PacketSerializationException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void ReplyCloseAuctionClientbound::receive(int fd) {
  readPacketId(fd, ReplyCloseAuctionClientbound::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    this->status = OK;
  } else if (status_str == "EAU") {
    this->status = EAU;
  } else if (status_str == "EOW") {
    this->status = EOW;
  } else if (status_str == "END") {
    this->status = END;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void ShowAssetServerbound::send(int fd) {
  std::stringstream stream;
  stream << ShowAssetServerbound::ID << " ";
  write_auction_id(stream, auction_id);
  stream << std::endl;
  writeString(fd, stream.str());
}

void ShowAssetServerbound::receive(int fd) {
  // Serverbound packets don't read their ID
  readSpace(fd);
  auction_id = readAuctionId(fd);
  readPacketDelimiter(fd);
}

void ReplyShowAssetClientbound::send(int fd) {
  std::stringstream stream;
  stream << ReplyShowAssetClientbound::ID << " ";
  if (status == OK) {
    file_size = getFileSize(file_path);
    stream << "OK " << file_name << " " << file_size << " ";
    writeString(fd, stream.str());
    stream.str(std::string()); // clears the stream
    stream.clear();            // resets any error flags
    sendFile(fd, file_path);
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw PacketSerializationException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void ReplyShowAssetClientbound::receive(int fd) {
  readPacketId(fd, ReplyShowAssetClientbound::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "OK") {
    status = OK;
    readSpace(fd);
    file_name = readFileName(fd);
    readSpace(fd);
    file_size = readFileSize(fd);
    readSpace(fd);
    readAndSaveToFile(fd, file_name, file_size, false);
  } else if (status_str == "NOK") {
    status = NOK;
  } else if (status_str == "ERR") {
    status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void BidServerbound::send(int fd) {
  std::stringstream stream;
  stream << BidServerbound::ID << " ";
  write_user_id(stream, user_id);
  stream << " " << password << " ";
  write_auction_id(stream, auction_id);
  stream << " " << fillZeros(bid_value, BID_MAX_LEN) << std::endl;
  writeString(fd, stream.str());
}

void BidServerbound::receive(int fd) {
  // Serverbound packets don't read their ID
  readSpace(fd);
  user_id = readUserId(fd);
  readSpace(fd);
  password = readString(fd);
  readSpace(fd);
  auction_id = readAuctionId(fd);
  readSpace(fd);
  bid_value = readInt(fd);
  readPacketDelimiter(fd);
}

void ReplyBidClientbound::send(int fd) {
  std::stringstream stream;
  stream << ReplyBidClientbound::ID << " ";
  if (status == ACC) {
    stream << "ACC";
  } else if (status == NOK) {
    stream << "NOK";
  } else if (status == NLG) {
    stream << "NLG";
  } else if (status == ILG) {
    stream << "ILG";
  } else if (status == REF) {
    stream << "REF";
  } else if (status == ERR) {
    stream << "ERR";
  } else {
    throw PacketSerializationException();
  }
  stream << std::endl;
  writeString(fd, stream.str());
}

void ReplyBidClientbound::receive(int fd) {
  readPacketId(fd, ReplyBidClientbound::ID);
  readSpace(fd);
  auto status_str = readString(fd);
  if (status_str == "ACC") {
    this->status = ACC;
  } else if (status_str == "NOK") {
    this->status = NOK;
  } else if (status_str == "NLG") {
    this->status = NLG;
  } else if (status_str == "ILG") {
    this->status = ILG;
  } else if (status_str == "REF") {
    this->status = REF;
  } else if (status_str == "ERR") {
    this->status = ERR;
  } else {
    throw InvalidPacketException();
  }
  readPacketDelimiter(fd);
}

void ErrorTcpPacket::send(int fd) {
  writeString(fd, ErrorTcpPacket::ID);
  writeString(fd, "\n");
}

void ErrorTcpPacket::receive(int fd) {
  (void)fd;
  // unimplemented
}

// Packet sending and receiving
void send_packet(UdpPacket &packet, int socket, struct sockaddr *address,
                 socklen_t addrlen) {
  const std::stringstream buffer = packet.serialize();
  ssize_t n = sendto(socket, buffer.str().c_str(), buffer.str().length(), 0,
                     address, addrlen);
  if (n == -1) {
    throw UnrecoverableError("Failed to send UDP packet", errno);
  }
}

void wait_for_packet(UdpPacket &packet, int socket) {
  fd_set file_descriptors;
  FD_ZERO(&file_descriptors);
  FD_SET(socket, &file_descriptors);

  struct timeval timeout;
  timeout.tv_sec = UDP_TIMEOUT_SECONDS; // wait for a response before throwing
  timeout.tv_usec = 0;

  int ready_fd = select(socket + 1, &file_descriptors, NULL, NULL, &timeout);
  if (is_shutting_down) {
    throw OperationCancelledException();
  }
  if (ready_fd == -1) {
    throw UnrecoverableError("Failed waiting for UDP packet on select", errno);
  } else if (ready_fd == 0) {
    throw ConnectionTimeoutException();
  }

  std::stringstream data;
  char buffer[SOCKET_BUFFER_LEN];

  ssize_t n = recvfrom(socket, buffer, SOCKET_BUFFER_LEN, 0, NULL, NULL);
  if (n == -1) {
    throw UnrecoverableError("Failed waiting for UDP packet on recvfrom",
                             errno);
  }

  data.write(buffer, n);

  packet.deserialize(data);
}

std::string read_date_time(std::stringstream &buffer) {
  std::string date, time;
  std::getline(buffer, date, ' ');
  std::getline(buffer, time, ' ');
  return date + " " + time;
}

void write_auction_id(std::stringstream &buffer, const uint32_t auction_id) {
  buffer << std::setfill('0') << std::setw(AUCTION_ID_MAX_LEN) << auction_id;
  buffer.copyfmt(std::ios(NULL)); // reset formatting
}

void write_user_id(std::stringstream &buffer, const uint32_t user_id) {
  buffer << std::setfill('0') << std::setw(USER_ID_STR_LEN) << user_id;
  buffer.copyfmt(std::ios(NULL)); // reset formatting
}

uint32_t parse_packet_user_id(std::string &id_str) {
  if (id_str.length() != USER_ID_STR_LEN) {
    throw InvalidPacketException();
  }
  for (char c : id_str) {
    if (!isdigit(c)) {
      throw InvalidPacketException();
    }
  }
  try {
    int i = std::stoi(id_str);
    if (i < 0 || i > (int)USER_ID_MAX) {
      throw InvalidPacketException();
    }
    return (uint32_t)i;
  } catch (...) {
    throw InvalidPacketException();
  }
}

uint32_t parse_packet_auction_id(std::string &id_str) {
  if (id_str.length() != AUCTION_ID_MAX_LEN) {
    throw InvalidPacketException();
  }
  for (char c : id_str) {
    if (!isdigit(c)) {
      throw InvalidPacketException();
    }
  }
  try {
    int i = std::stoi(id_str);
    if (i < 0 || i > AUCTION_MAX_NUMBER) {
      throw InvalidPacketException();
    }
    return (uint32_t)i;
  } catch (...) {
    throw InvalidPacketException();
  }
}

void sendFile(int connection_fd, std::filesystem::path file_path) {
  std::ifstream file(file_path, std::ios::in | std::ios::binary);
  if (!file) {
    std::cerr << "Error opening file: " << file_path << std::endl;
    throw PacketSerializationException();
  }

  char buffer[FILE_BUFFER_LEN];
  while (file) {
    file.read(buffer, FILE_BUFFER_LEN);
    ssize_t bytes_read = (ssize_t)file.gcount();
    ssize_t bytes_sent = 0;
    while (bytes_sent < bytes_read) {
      ssize_t sent = write(connection_fd, buffer + bytes_sent,
                           (size_t)(bytes_read - bytes_sent));
      if (sent < 0) {
        throw PacketSerializationException();
      }
      bytes_sent += sent;
    }
  }
}

std::string auctionsToString(const std::vector<std::pair<uint32_t, bool>> &auctions) {
  std::ostringstream formattedString;
  for (size_t i = 0; i < auctions.size(); ++i) {
    formattedString << fillZeros(auctions[i].first, AUCTION_ID_MAX_LEN) << " ";
    formattedString << auctions[i].second;
    if (i != auctions.size() - 1) {
      formattedString << " ";
    }
  }
  return formattedString.str();
}

std::vector<std::pair<uint32_t, bool>>
UdpPacket::readAuctions(std::stringstream &buffer) {
  std::vector<std::pair<uint32_t, bool>> auctions;

  uint32_t auctionId;
  bool auctionStatus;
  while (buffer.peek() != '\n') {
    readSpace(buffer);
    auctionId = readAuctionId(buffer);
    readSpace(buffer);
    auctionStatus = readInt(buffer);
    // emplace_back adds element to the end of the vector
    auctions.emplace_back(auctionId, auctionStatus);
  }

  return auctions;
}

Bid UdpPacket::readBid(std::stringstream &buffer) {
  Bid bid = Bid();
  readChar(buffer, 'B');
  readSpace(buffer);
  bid.bidder_user_id = readUserId(buffer);
  readSpace(buffer);
  bid.bid_value = readInt(buffer);
  readSpace(buffer);
  bid.date_time = read_date_time(buffer);
  bid.sec_time = readInt(buffer);
  return bid;
}

std::string auctionID_ToString(uint32_t auction_id) {
  return fillZeros(auction_id, AUCTION_ID_MAX_LEN);
}

std::string fillZeros(uint32_t number, int length) {
  std::string str = std::to_string(number);
  uint32_t len = static_cast<uint32_t>(length);
  while (str.length() < len) {
    str = '0' + str;
  }

  return str;
}

uint32_t getFileSize(std::filesystem::path file_path) {
  try {
    return (uint32_t)std::filesystem::file_size(file_path);
  } catch (...) {
    throw PacketSerializationException();
  }
}
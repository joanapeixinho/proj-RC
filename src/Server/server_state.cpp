#include "server_state.hpp"

#include <unistd.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "common/common.hpp"
#include "common/protocol.hpp"
#include "packet_handlers.hpp"

AuctionServerState::AuctionServerState(std::string &__word_file_path,
                                 std::string &port, bool __verbose,
                                 bool __select_randomly)
    : select_randomly{__select_randomly}, cdebug{DebugStream(__verbose)} {
  this->setup_sockets();
  this->resolveServerAddress(port);
  this->registerWords(__word_file_path);
  this->scoreboard.loadFromFile();
  srand((uint32_t)time(NULL));  // Initialize rand seed
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
  udp_packet_handlers.insert({StartAuctionServerbound::ID, handle_start_Auction});
  udp_packet_handlers.insert({GuessLetterServerbound::ID, handle_guess_letter});
  udp_packet_handlers.insert({GuessWordServerbound::ID, handle_guess_word});
  udp_packet_handlers.insert({QuitAuctionServerbound::ID, handle_quit_Auction});
  udp_packet_handlers.insert({RevealWordServerbound::ID, handle_reveal_word});

  // TCP
  tcp_packet_handlers.insert({ScoreboardServerbound::ID, handle_scoreboard});
  tcp_packet_handlers.insert({HintServerbound::ID, handle_hint});
  tcp_packet_handlers.insert({StateServerbound::ID, handle_state});
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

void AuctionServerState::registerWords(std::string &__word_file_path) {
  try {
    std::filesystem::path word_file_path(std::filesystem::current_path());
    word_file_path.append(__word_file_path);

    std::cout << "Reading words from " << word_file_path << std::endl;

    std::ifstream word_file(word_file_path);
    if (!word_file.is_open()) {
      throw UnrecoverableError("Failed to open word file");
    }

    std::string line;
    while (std::getline(word_file, line)) {
      auto split_index = line.find(' ');
      Word word;
      word.word = line.substr(0, split_index);

      if (word.word.length() < WORD_MIN_LEN ||
          word.word.length() > WORD_MAX_LEN) {
        std::cerr << "[WARNING] Word '" << word.word << "' is not between "
                  << WORD_MIN_LEN << " and " << WORD_MAX_LEN
                  << " characters long. Ignoring" << std::endl;
        continue;
      }

      if (split_index != std::string::npos) {
        std::filesystem::path hint_file_path(word_file_path);
        hint_file_path.remove_filename().append(line.substr(split_index + 1));

        if (!hint_file_path.has_filename()) {
          std::cerr << "[WARNING] Hint file path " << hint_file_path
                    << ", for word '" << word.word << "' is not a file."
                    << std::endl;
        }

        word.hint_path = hint_file_path;
      } else {
        std::cerr << "[WARNING] Word '" << word.word
                  << "' does not have an hint file" << std::endl;
        word.hint_path = std::nullopt;
      }
      this->words.push_back(word);
    }

    if (words.size() == 0) {
      std::cerr << "[FATAL] There are no valid words in the provided word file"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    std::cout << "Loaded " << words.size() << " word(s)" << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Failed to open word file: " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  } catch (...) {
    std::cerr << "Failed to open word file: unknown" << std::endl;
    exit(EXIT_FAILURE);
  }
}

Word &AuctionServerState::selectRandomWord() {
  uint32_t index;
  if (select_randomly) {
    index = (uint32_t)rand() % (uint32_t)this->words.size();
  } else {
    index = (this->current_word_index) % (uint32_t)this->words.size();
    this->current_word_index = index + 1;
  }
  return this->words[index];
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

AuctionServerSync AuctionServerState::createAuction(uint32_t user_id) {
  std::scoped_lock<std::mutex> g_lock(AuctionsLock);

  auto Auction = Auctions.find(user_id);
  if (Auction != Auctions.end()) {
    {
      AuctionServerSync Auction_sync = AuctionServerSync(Auction->second);
      if (Auction_sync->isOnGoing()) {
        if (Auction_sync->hasStarted()) {
          throw AuctionAlreadyStartedException();
        }
        return Auction_sync;
      }
    }

    std::cout << "Deleting Auction" << std::endl;
    // Delete existing Auction, so we can create a new one below
    Auctions.erase(Auction);
  }

  Word &word = this->selectRandomWord();
  // Some C++ magic to create an instance of the class inside the map, without
  // moving it, since mutexes can't be moved
  auto inserted = Auctions.emplace(
      std::piecewise_construct, std::forward_as_tuple(user_id),
      std::forward_as_tuple(user_id, word.word, word.hint_path));

  if (inserted.first->second.loadFromFile(true)) {
    // Loaded from file successfully, recheck if it has started
    if (inserted.first->second.hasStarted()) {
      throw AuctionAlreadyStartedException();
    }
    return AuctionServerSync(inserted.first->second);
  }

  return AuctionServerSync(Auctions.at(user_id));
}

AuctionServerSync AuctionServerState::getAuction(uint32_t user_id) {
  std::scoped_lock<std::mutex> g_lock(AuctionsLock);

  auto Auction = Auctions.find(user_id);
  if (Auction == Auctions.end()) {
    // Try to load from disk

    // Some C++ magic to create an instance of the class inside the map, without
    // moving it, since mutexes can't be moved
    auto inserted = Auctions.emplace(
        std::piecewise_construct, std::forward_as_tuple(user_id),
        std::forward_as_tuple(user_id, std::string(), std::nullopt));

    if (!inserted.first->second.loadFromFile(false)) {
      // Failed to load, throw exception
      Auctions.erase(inserted.first);
      throw NoAuctionFoundException();
    }

    return AuctionServerSync(inserted.first->second);
  }

  return AuctionServerSync(Auction->second);
}
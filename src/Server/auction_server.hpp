#ifndef auction_server_H
#define auction_server_H

#include <filesystem>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>


class AuctionServer : public Auction {
 private:
  std::string word;
  std::optional<std::filesystem::path> hint_path;
  uint32_t lettersRemaining;
  std::vector<char> plays;
  std::vector<std::string> word_guesses;

  std::vector<uint32_t> getIndexesOfLetter(char letter);

 public:
  std::mutex lock;

  AuctionServer(uint32_t __userId, std::string __word,
             std::optional<std::filesystem::path> __hint_path);
  std::vector<uint32_t> guessLetter(char letter, uint32_t trial);
  bool guessWord(std::string& word, uint32_t trial);
  bool hasLost();
  bool hasWon();
  bool hasStarted();
  uint32_t getScore();
  std::string getStateString();
  std::string getWord();
  std::string getWordProgress();
  std::optional<std::filesystem::path> getHintFilePath();
  std::string getHintFileName();
  void saveToFile();
  bool loadFromFile(bool on_going_only);
};

class AuctionServerSync {
 private:
  std::unique_lock<std::mutex> slock;

 public:
  AuctionServer& Auction;

  AuctionServerSync(AuctionServer& __Auction) : slock{__Auction.lock}, Auction{__Auction} {};

  AuctionServer& operator*() {
    return Auction;
  }
  AuctionServer* operator->() {
    return &Auction;
  }
};

/** Exceptions **/

class DuplicateLetterGuessException : public std::runtime_error {
 public:
  DuplicateLetterGuessException()
      : std::runtime_error(
            "That letter has already been guessed in this Auction before.") {}
};

class DuplicateWordGuessException : public std::runtime_error {
 public:
  DuplicateWordGuessException()
      : std::runtime_error(
            "That word has already been guessed in this Auction before.") {}
};

class InvalidTrialException : public std::runtime_error {
 public:
  InvalidTrialException()
      : std::runtime_error(
            "The trial number does not match what was expected.") {}
};

class AuctionHasEndedException : public std::runtime_error {
 public:
  AuctionHasEndedException()
      : std::runtime_error(
            "Auction has already ended, therefore no actions can take place.") {}
};

#endif
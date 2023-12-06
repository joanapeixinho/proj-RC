#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "user_state.hpp"

class CommandHandler;

class CommandManager {
  std::vector<std::shared_ptr<CommandHandler>> handlerList;
  std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers;

 public:
  void printHelp();
  void registerCommand(std::shared_ptr<CommandHandler> handler);
  void waitForCommand(UserState& state);
};

class CommandHandler {
 protected:
  CommandHandler(const char* __name, const std::optional<const char*> __alias,
                 const std::optional<const char*> __usage,
                 const char* __description)
      : name{__name},
        alias{__alias},
        usage{__usage},
        description{__description} {}

 public:
  const char* name;
  const std::optional<const char*> alias;
  const std::optional<const char*> usage;
  const char* description;
  virtual void handle(std::string args, UserState& state) = 0;
};

class LoginCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  LoginCommand() : CommandHandler("login", "sng", "UID", "Login User") {}
};

class LogoutCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  LogoutCommand() : CommandHandler("logout", "sng", std::nullopt, "Logout User") {}
};


class UnregisterCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  UnregisterCommand() : CommandHandler("unregister", "sng", std::nullopt, "Unregister User") {}
};

class ListMyAuctionsCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  ListCommand() : CommandHandler("list", "sng", std::nullopt, "List Users") {}
};


class BidCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  BidCommand() : CommandHandler("bid", "sng", "UID", "Bid on Auction") {}
};


class HelpCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  HelpCommand() : CommandHandler("info", "sng", "UID", "Help Auction") {}
};

class ShowRecordCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  ShowRecordCommand() : CommandHandler("record", "sng", "UID", "Show Record") {}
};

class ShowAssetCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  ShowAssetCommand() : CommandHandler("asset", "sng", "UID", "Show Asset") {}
};

class MyAuctionsCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  MyAuctionsCommand() : CommandHandler("myauctions", "sng", "UID", "Show My Auctions") {}
};

class MyBidsCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  MyBidsCommand() : CommandHandler("mybids", "sng", "UID", "Show My Bids") {}
};

class OpenAuctionCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  OpenAuctionCommand() : CommandHandler("open", "sng", "UID", "Open Auction") {}
};

class CloseAuctionCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  CloseAuctionCommand() : CommandHandler("close", "sng", "UID", "Close Auction") {}
};

class ExitCommand : public CommandHandler {
  void handle(std::string args, UserState& state);

 public:
  ExitCommand() : CommandHandler("exit", "sng", "UID", "Exit Application") {}
};



// void write_word(std::ostream& stream, char* word, uint32_t word_len);

// bool is_Auction_active(userState& state);

// void print_Auction_progress(userState& state);

uint32_t parse_user_id(std::string& args);

void display_file(std::string filename);

#endif
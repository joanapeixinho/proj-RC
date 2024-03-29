#ifndef COMMANDS_H
#define COMMANDS_H

#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/protocol.hpp"
#include "user_state.hpp"

class CommandHandler;

class CommandManager {
  std::vector<std::shared_ptr<CommandHandler>> handlerList;
  std::unordered_map<std::string, std::shared_ptr<CommandHandler>> handlers;

public:
  void printHelp();
  void registerCommand(std::shared_ptr<CommandHandler> handler);
  void waitForCommand(UserState &state);
};

class CommandHandler {
protected:
  CommandHandler(const char *__name, const std::optional<const char *> __alias,
                 const std::optional<const char *> __usage,
                 const char *__description)
      : name{__name}, alias{__alias}, usage{__usage},
        description{__description} {}

public:
  const char *name;
  const std::optional<const char *>
      alias; // Alternative name to call the command
  const std::optional<const char *> usage;
  const char *description;
  virtual void handle(std::string args, UserState &state) = 0;
};

class LoginCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  LoginCommand()
      : CommandHandler("login", std::nullopt, "<UID> <password>",
                       "Login User") {}
};

class LogoutCommand : public CommandHandler {
  // handle is public to allow logout from main after CTRL+C
public:
  void handle(std::string args, UserState &state);

  LogoutCommand()
      : CommandHandler("logout", std::nullopt, std::nullopt, "Logout User") {}
};

class UnregisterCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  UnregisterCommand()
      : CommandHandler("unregister", std::nullopt, std::nullopt,
                       "Unregister User") {}
};

class ListCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  ListCommand()
      : CommandHandler("list", "l", std::nullopt, "List active Auctions") {}
};

class BidCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  BidCommand()
      : CommandHandler("bid", "b", "<AID> <bid_value>", "Bid on Auction") {}
};

class HelpCommand : public CommandHandler {
  void handle(std::string args, UserState &state);
  CommandManager &manager;

public:
  HelpCommand(CommandManager &__manager)
      : CommandHandler("help", "?", std::nullopt, "Show command list"),
        manager(__manager) {}
};

class ShowAssetCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  ShowAssetCommand()
      : CommandHandler("show_asset", "sa", "<AID>", "Show Asset") {}
};

class ListMyAuctionsCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  ListMyAuctionsCommand()
      : CommandHandler("myauctions", "ma", std::nullopt, "Show My Auctions") {}
};

class MyBidsCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  MyBidsCommand()
      : CommandHandler("mybids", "mb", std::nullopt, "Show My Bids") {}
};

class OpenAuctionCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  OpenAuctionCommand()
      : CommandHandler("open", std::nullopt,
                       "<name> <asset_fname> <start_value> <timeactive> ",
                       "Open Auction") {}
};

class CloseAuctionCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  CloseAuctionCommand()
      : CommandHandler("close", std::nullopt, "<AID>", "Close Auction") {}
};

class ExitCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  ExitCommand()
      : CommandHandler("exit", std::nullopt, std::nullopt, "Exit Application") {
  }
};

class ShowRecordCommand : public CommandHandler {
  void handle(std::string args, UserState &state);

public:
  ShowRecordCommand()
      : CommandHandler("show_record", "sr", "<AID>", "Show Auction Record") {}
};

bool is_numeric(std::string &str);

bool isValidAuctionName(const std::string &str);

bool is_alphanumeric(std::string &str);

uint32_t parse_user_id(std::string &args);

void printBidsInfo(std::vector<Bid> bids);

void printAuctions(const std::vector<std::pair<uint32_t, bool>> &auctions);

#endif
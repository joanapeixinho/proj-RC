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
  LoginCommand() : CommandHandler("login", "sg", "UID", "Login User") {}
};

/*
class GuessLetterCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  GuessLetterCommand()
      : CommandHandler("play", "pl", "letter", "Guess a letter") {}
};

class GuessWordCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  GuessWordCommand() : CommandHandler("guess", "gw", "word", "Guess a word") {}
};

class ScoreboardCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  ScoreboardCommand()
      : CommandHandler("scoreboard", "sb", std::nullopt,
                       "Display the scoreboard") {}
};

class HintCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  HintCommand()
      : CommandHandler("hint", "h", std::nullopt,
                       "Get a hint for the current word") {}
};

class StateCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  StateCommand() : CommandHandler("state", "st", std::nullopt, "Show state") {}
};

class QuitCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  QuitCommand()
      : CommandHandler("quit", std::nullopt, std::nullopt, "Quit Auction") {}
};

class ExitCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  ExitCommand()
      : CommandHandler("exit", std::nullopt, std::nullopt, "Exit application") {
  }
};

class RevealCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  RevealCommand()
      : CommandHandler("reveal", "rv", std::nullopt, "Reveal word") {}
};

class HelpCommand : public CommandHandler {
  void handle(std::string args, userState& state);
  CommandManager& manager;

 public:
  HelpCommand(CommandManager& __manager)
      : CommandHandler("help", "?", std::nullopt, "Show command list"),
        manager(__manager) {}
};

class KillCommand : public CommandHandler {
  void handle(std::string args, userState& state);

 public:
  KillCommand() : CommandHandler("kill", "kl", "UID", "Kill Auction on server") {}
};
*/
// void write_word(std::ostream& stream, char* word, uint32_t word_len);

// bool is_Auction_active(userState& state);

// void print_Auction_progress(userState& state);

uint32_t parse_user_id(std::string& args);

void display_file(std::string filename);

#endif
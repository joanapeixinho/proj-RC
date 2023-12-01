#include "commands.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

// #include "client_Auction.hpp"
#include "common/protocol.hpp"

extern bool is_shutting_down;

void CommandManager::printHelp() {
  std::cout << std::endl << "Available commands:" << std::endl << std::left;

  for (auto it = this->handlerList.begin(); it != this->handlerList.end();
       ++it) {
    auto handler = *it;
    std::ostringstream ss;
    ss << handler->name;
    if (handler->usage) {
      ss << " " << *handler->usage;
    }
    std::cout << std::setw(HELP_MENU_COMMAND_COLUMN_WIDTH) << ss.str();
    std::cout << std::setw(HELP_MENU_DESCRIPTION_COLUMN_WIDTH)
              << handler->description;
    if (handler->alias) {
      std::cout << "(Alias: " << *handler->alias << ")";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << std::resetiosflags(std::ios_base::basefield);
}

void CommandManager::registerCommand(std::shared_ptr<CommandHandler> handler) {
  this->handlerList.push_back(handler);
  this->handlers.insert({handler->name, handler});
  if (handler->alias) {
    this->handlers.insert({*handler->alias, handler});
  }
}

void CommandManager::waitForCommand(UserState& state) {
  std::cout << "> ";

  std::string line;
  std::getline(std::cin, line);

  if (std::cin.eof() || is_shutting_down) {
    return;
  }

  auto splitIndex = line.find(' ');

  std::string commandName;
  if (splitIndex == std::string::npos) { // In case there is no space
    commandName = line;
    line = "";
  } else {
    commandName = line.substr(0, splitIndex);
    line.erase(0, splitIndex + 1);
  }

  if (commandName.length() == 0) {
    return;
  }

  auto handler = this->handlers.find(commandName);
  if (handler == this->handlers.end()) {
    std::cout << "Unknown command" << std::endl;
    return;
  }

  try {
    // "handler" is an iterator & "handler->second" is a pointer to the handler instance
    handler->second->handle(line, state);
  } catch (std::exception& e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
  } catch (...) {
    std::cout << "[ERROR] An unknown error occurred." << std::endl;
  }
}

/* Command handlers */
void LoginCommand::handle(std::string args, UserState& state) {
  uint32_t user_id;
  // Argument parsing
  try {
    user_id = parse_user_id(args);
  } catch (...) {
    std::cout << "Invalid user ID. It must be a positive number up to "
              << USER_ID_MAX_LEN << " digits" << std::endl;
    return;
  }

  // Populate and send packet
  LoginServerbound packet_out;
  packet_out.user_id = user_id;

  ReplyLoginClientbound rli;
  state.sendUdpPacketAndWaitForReply(packet_out, rli);

  switch (rli.status) {
    case ReplyLoginClientbound::status::OK:
      // Login user
      state.login();
      // Output Login info
      std::cout << "Logged in successfully!" << std::endl;
      break;

    case ReplyLoginClientbound::status::NOK:
      std::cout
          << "Failed to login: the password does not match this UserID."
          << std::endl;
      break;

    case ReplyLoginClientbound::status::ERR:
    default:
      std::cout << "Auction failed to start: packet was wrongly structured."
                << std::endl;
      break;
  }
}

uint32_t parse_user_id(std::string& args) {
  size_t converted = 0;
  long user_id = std::stol(args, &converted, 10);
  if (converted != args.length() || user_id <= 0 ||
      user_id > USER_ID_MAX) {
    throw std::runtime_error("invalid user id");
  }

  return (uint32_t)user_id;
}

void display_file(std::string filename) {
  std::ifstream f(filename);
  if (f.is_open()) {
    std::cout << f.rdbuf() << std::endl;
  } else {
    std::cout
        << "Failed to open file to display. Please open the file manually:"
        << filename << std::endl;
  }
}
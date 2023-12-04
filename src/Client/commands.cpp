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
  auto splitIndex = args.find(' ');
  std::string user_id_str = args.substr(0, splitIndex);
  args.erase(0, splitIndex + 1);
  std::string password = args;
  
  // Argument parsing
  try {
    user_id = parse_user_id(user_id_str);
  } catch (...) {
    std::cout << "Invalid user ID. It must be a positive number up to "
              << USER_ID_MAX_LEN << " digits" << std::endl;
    return;
  }
  // Check if Password is too long
  if (password.length() > PASSWORD_MAX_LEN) {
    std::cout << "Invalid password. It must be at most " << PASSWORD_MAX_LEN 
              << " characters long" << std::endl;
    return;
  }
  // Check if Password is AlphaNumeric
  for (char c : password) {
    if (!isalnum(c)) {
      std::cout << "Invalid password. It must be alphanumeric" << std::endl;
      return;
    }
  }


  // Populate and send packet
  LoginServerbound packet_out;
  packet_out.user_id = user_id;
  packet_out.password = password;

  ReplyLoginClientbound rli;
  state.sendUdpPacketAndWaitForReply(packet_out, rli);

  switch (rli.status) {
    case ReplyLoginClientbound::status::OK:
      // Login user
      state.login(user_id, password);
      // Output Login info
      std::cout << "Logged in successfully!" << std::endl;
      break;

    case ReplyLoginClientbound::status::NOK:
      std::cout
          << "Failed to login: the password does not match the UserID."
          << std::endl;
      break;

    case ReplyLoginClientbound::status::REG:
    default:
      // Registered user is still logged in
      state.login(user_id, password);
      std::cout << "New user registered successfully!" << std::endl;
      break;
  }
}

void LogoutCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to logout: you need to be logged in to logout." 
        << std::endl;
    return;
  }

    // Populate and send packet
  LogoutServerbound packet_out;
  packet_out.user_id = state.user_id;
  packet_out.password = state.password;

  ReplyLogoutClientbound lou;
  state.sendUdpPacketAndWaitForReply(packet_out, lou);

  switch (lou.status) {
    case ReplyLogoutClientbound::status::OK:
      // Logout user
      state.logout();
      // Output Logout info
      std::cout << "Logged out successfully!" << std::endl;
      break;

    case ReplyLogoutClientbound::status::NOK:
      std::cout
          << "Failed to logout: the user is not logged in."
          << std::endl;
      break;

    case ReplyLogoutClientbound::status::UNR:
    default:
      std::cout 
          << "Failed to logout: the user is not registered." 
          << std::endl;
      break;
  }
}

void UnregisterCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to unregister: you need to be logged in to unregister." 
        << std::endl;
    return;
  }

  // Populate and send packet
  UnregisterServerbound packet_out;
  packet_out.user_id = state.user_id;
  packet_out.password = state.password;

  ReplyUnregisterClientbound lou;
  state.sendUdpPacketAndWaitForReply(packet_out, lou);

  switch (lou.status) {
    case ReplyUnregisterClientbound::status::OK:
      // When you unregister, you are logged out
      state.logout();
      // Output unregister info
      std::cout << "Un-registered successfully!" << std::endl;
      break;

    case ReplyUnregisterClientbound::status::NOK:
      std::cout
          << "Failed to un-register: the user is not logged in."
          << std::endl;
      break;

    case ReplyUnregisterClientbound::status::UNR:
    default:
      std::cout 
          << "Failed to un-register: the user is not registered." 
          << std::endl;
      break;
  }
}

void ExitCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (state.isLoggedIn()) {
    std::cout 
        << "Failed to exit: please logout before exiting." 
        << std::endl;
    return;
  }
  is_shutting_down = true;
}

void OpenAuctionCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to open auction: you need to be logged in to open an auction." 
        << std::endl;
    return;
  }

  // Argument parsing
  auto splitIndex = args.find(' ');
  std::string asset_name = args.substr(0, splitIndex);
  args.erase(0, splitIndex + 1);
  std::string asset_description = args;

  // TODO: Is there any rescriction on the asset name or description???

  // Populate and send packet
  OpenAuctionServerbound packet_out;
  packet_out.user_id = state.user_id;
  packet_out.password = state.password;
  packet_out.asset_name = asset_name;
  packet_out.asset_description = asset_description;

  ReplyOpenAuctionClientbound roa;
  state.sendTcpPacketAndWaitForReply(packet_out, roa);

  switch (roa.status) {
    case ReplyOpenAuctionClientbound::status::OK:
      // Output open auction info
      std::cout << "Auction opened successfully!" << std::endl;
      break;

    case ReplyOpenAuctionClientbound::status::NOK:
      std::cout
          << "Failed to open auction: the user is not logged in."
          << std::endl;
      break;

    case ReplyOpenAuctionClientbound::status::UNR:
    default:
      std::cout 
          << "Failed to open auction: the user is not registered." 
          << std::endl;
      break;
  }
}


uint32_t parse_user_id(std::string& args) {
  size_t converted = 0;
  // stol -> String TO Long int; converted -> number of characters converted to int
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
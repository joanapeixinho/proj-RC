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

/*
void GuessLetterCommand::handle(std::string args, PlayerState& state) {
  // Check if there is a game running
  if (!is_game_active(state)) {
    return;
  }
  // Argument parsing
  if (args.length() != 1 || args[0] < 'a' || args[0] > 'z') {
    std::cout << "Invalid letter. It must be a single lowercase letter"
              << std::endl;
    return;
  }
  char guess = args[0];

  // Populate and send packet
  GuessLetterServerbound packet_out;
  packet_out.user_id = state.Auction->getuserId();
  packet_out.guess = guess;
  packet_out.trial = state.Auction->getCurrentTrial();

  GuessLetterClientbound rlg;
  state.sendUdpPacketAndWaitForReply(packet_out, rlg);

  // Check packet status
  if (rlg.status == GuessLetterClientbound::status::ERR) {
    std::cout
        << "Word guess failed: there is no on-going Auction for this user on "
           "the server. Use 'state' command to check the state of the Auction."
        << std::endl;
    return;
  }

  switch (rlg.status) {
    case GuessLetterClientbound::status::OK:
      // Update Auction state
      for (auto it = rlg.pos.begin(); it != rlg.pos.end(); ++it) {
        state.Auction->updateWordChar(*it - 1, guess);
      }
      state.Auction->increaseTrial();
      std::cout << "Letter '" << guess << "' is part of the word!" << std::endl;
      break;

    case GuessLetterClientbound::status::WIN:
      // Update Auction state
      for (uint32_t i = 0; i < state.Auction->getWordLen(); i++) {
        if (state.Auction->getWordProgress()[i] == '_') {
          state.Auction->updateWordChar(i, guess);
        }
      }
      state.Auction->increaseTrial();
      // Output Auction info
      print_Auction_progress(state);
      state.Auction->finishAuction();
      std::cout << "Letter '" << guess << "' is part of the word!" << std::endl;
      std::cout << "YOU WON!" << std::endl;
      break;

    case GuessLetterClientbound::status::DUP:
      std::cout << "Letter '" << guess << "' has already been guessed before."
                << std::endl;
      break;

    case GuessLetterClientbound::status::NOK:
      std::cout << "Letter '" << guess << "' is NOT part of the word."
                << std::endl;
      state.Auction->increaseTrial();
      state.Auction->updateNumErrors();
      break;

    case GuessLetterClientbound::status::OVR:
      state.Auction->updateNumErrors();
      state.Auction->increaseTrial();
      print_Auction_progress(state);
      state.Auction->finishAuction();
      std::cout << "Letter '" << guess << "' is NOT part of the word."
                << std::endl;
      std::cout << "Auction over. You've lost." << std::endl;
      break;

    case GuessLetterClientbound::status::INV:
      std::cout << "Client and Auction server are out-of-sync. Please quit the "
                   "current Auction and start a new one."
                << std::endl;
      break;

    case GuessLetterClientbound::status::ERR:
    default:
      break;
  }
}

void GuessWordCommand::handle(std::string args, userState& state) {
  // Check if there is a Auction running
  if (!is_Auction_active(state)) {
    return;
  }

  // Argument parsing
  if (args.length() != state.Auction->getWordLen()) {
    std::cout << "Invalid argument. It must be a word of length "
              << state.Auction->getWordLen() << std::endl;
    return;
  }

  // Populate and send packet
  GuessWordServerbound packet_out;
  packet_out.user_id = state.Auction->getuserId();
  packet_out.trial = state.Auction->getCurrentTrial();
  packet_out.guess = args;

  GuessWordClientbound rwg;
  state.sendUdpPacketAndWaitForReply(packet_out, rwg);

  // Check packet status
  if (rwg.status == GuessWordClientbound::status::ERR) {
    std::cout
        << "Word guess failed: there is no on-going Auction for this user on "
           "the server. Use 'state' command to check the state of the Auction."
        << std::endl;
    return;
  }

  switch (rwg.status) {
    case GuessWordClientbound::status::WIN:
      // Update Auction state
      for (uint32_t i = 0; i < state.Auction->getWordLen(); i++) {
        state.Auction->updateWordChar(i, args[i]);
      }
      state.Auction->increaseTrial();
      // Output Auction info
      print_Auction_progress(state);
      state.Auction->finishAuction();
      std::cout << "Word '" << args << "' is the correct word." << std::endl;
      std::cout << "YOU WON!" << std::endl;
      break;
    case GuessWordClientbound::status::DUP:
      std::cout << "Word '" << args << "' has already been guessed before."
                << std::endl;
      break;
    case GuessWordClientbound::status::NOK:
      state.Auction->updateNumErrors();
      state.Auction->increaseTrial();
      std::cout << "Word '" << args << "' is NOT the correct word."
                << std::endl;
      break;

    case GuessWordClientbound::status::OVR:
      state.Auction->updateNumErrors();
      state.Auction->increaseTrial();
      std::cout << "Word '" << args << "' is NOT the correct word."
                << std::endl;
      print_Auction_progress(state);
      state.Auction->finishAuction();
      std::cout << "Auction over. You've lost." << std::endl;
      break;

    case GuessWordClientbound::status::INV:
      std::cout << "Communicated wrong trial number" << std::endl;
      break;

    case GuessWordClientbound::status::ERR:
    default:
      break;
  }
}

void QuitCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args
  // Check if there is a Auction running
  if (!is_Auction_active(state)) {
    return;
  }

  // Populate and send packet
  QuitAuctionServerbound packet_out;
  packet_out.user_id = state.Auction->getuserId();

  QuitAuctionClientbound rq;
  state.sendUdpPacketAndWaitForReply(packet_out, rq);

  // Check packet status
  switch (rq.status) {
    case QuitAuctionClientbound::status::OK:
      std::cout << "Auction quit successfully." << std::endl;
      state.Auction->finishAuction();
      break;

    case QuitAuctionClientbound::status::NOK:
      std::cout << "Auction had already finished." << std::endl;
      state.Auction->finishAuction();
      break;

    case QuitAuctionClientbound::status::ERR:
    default:
      std::cout << "Error with the request. Please try again." << std::endl;
      break;
  }
}

void ExitCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args
  if (state.hasActiveAuction()) {
    // Populate and send packet
    QuitAuctionServerbound packet_out;
    packet_out.user_id = state.Auction->getuserId();

    QuitAuctionClientbound rq;
    state.sendUdpPacketAndWaitForReply(packet_out, rq);

    // Check packet status
    switch (rq.status) {
      case QuitAuctionClientbound::status::OK:
        std::cout << "Auction quit successfully." << std::endl;
        state.Auction->finishAuction();
        break;

      case QuitAuctionClientbound::status::NOK:
        std::cout << "Auction had already finished." << std::endl;
        state.Auction->finishAuction();
        break;

      case QuitAuctionClientbound::status::ERR:
      default:
        std::cout << "Failed to quit Auction." << std::endl;
        break;
    }
  }
  is_shutting_down = true;
}

void RevealCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args
  // Check if there is a Auction running
  if (!is_Auction_active(state)) {
    return;
  }
  // Populate and send packet
  RevealWordServerbound packet_out;
  packet_out.user_id = state.Auction->getuserId();

  RevealWordClientbound rrv;
  state.sendUdpPacketAndWaitForReply(packet_out, rrv);

  std::cout << "Word: " << rrv.word << std::endl;
}

void ScoreboardCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args

  ScoreboardServerbound scoreboard_packet;
  ScoreboardClientbound packet_reply;

  state.sendTcpPacketAndWaitForReply(scoreboard_packet, packet_reply);
  switch (packet_reply.status) {
    case ScoreboardClientbound::status::OK:
      std::cout << "Received scoreboard and saved to file." << std::endl;
      std::cout << "Path: " << packet_reply.file_name << std::endl;
      display_file(packet_reply.file_name);
      break;

    case ScoreboardClientbound::status::EMPTY:
      std::cout << "Empty scoreboard" << std::endl;
      break;

    default:
      break;
  }
}

void HintCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args
  // Check if there is a Auction running
  if (!is_Auction_active(state)) {
    return;
  }

  HintServerbound hint_packet;
  hint_packet.user_id = state.Auction->getuserId();
  HintClientbound packet_reply;

  state.sendTcpPacketAndWaitForReply(hint_packet, packet_reply);

  switch (packet_reply.status) {
    case HintClientbound::status::OK:
      std::cout << "Received hint and saved to file." << std::endl;
      std::cout << "Path: " << packet_reply.file_name << std::endl;
      break;

    case HintClientbound::status::NOK:
      std::cout << "No hint available :(" << std::endl;
      break;

    default:
      break;
  }
}

void StateCommand::handle(std::string args, userState& state) {
  (void)args;  // unused - no args
  if (!state.hasAuction()) {
    std::cout << "You need to start a Auction to use this command." << std::endl;
    return;
  }

  StateServerbound packet_out;
  packet_out.user_id = state.Auction->getuserId();

  StateClientbound packet_reply;
  state.sendTcpPacketAndWaitForReply(packet_out, packet_reply);

  switch (packet_reply.status) {
    case StateClientbound::status::ACT:
      std::cout << "There is an active Auction." << std::endl;
      std::cout << "Path to file: " << packet_reply.file_name << std::endl;
      display_file(packet_reply.file_name);
      break;
    case StateClientbound::status::FIN:
      std::cout << "There is a finished Auction." << std::endl;
      std::cout << "Path to file: " << packet_reply.file_name << std::endl;
      display_file(packet_reply.file_name);
      if (state.hasActiveAuction()) {
        state.Auction->finishAuction();
      }
      break;
    case StateClientbound::status::NOK:
      std::cout << "There is no Auction history available for this user."
                << std::endl;
      break;

    default:
      break;
  }
}

void HelpCommand::handle(std::string args, userState& state) {
  (void)args;   // unused - no args
  (void)state;  // unused
  manager.printHelp();
}

void KillCommand::handle(std::string args, userState& state) {
  uint32_t user_id;
  // Argument parsing
  try {
    user_id = parse_user_id(args);
  } catch (...) {
    std::cout << "Invalid user ID. It must be a positive number up to "
              << user_ID_MAX_LEN << " digits" << std::endl;
    return;
  }

  // Populate and send packet
  QuitAuctionServerbound packet_out;
  packet_out.user_id = user_id;

  QuitAuctionClientbound rq;
  state.sendUdpPacketAndWaitForReply(packet_out, rq);
  // Check packet status
  switch (rq.status) {
    case QuitAuctionClientbound::status::OK:
      std::cout << "Auction quit successfully." << std::endl;
      break;

    case QuitAuctionClientbound::status::NOK:
      std::cout << "There is no on-going Auction for this user." << std::endl;
      // Auction was already finished
      break;

    case QuitAuctionClientbound::status::ERR:
    default:
      std::cout << "Failed to quit Auction on server." << std::endl;
      break;
  }
}

void write_word(std::ostream& stream, char* word, uint32_t word_len) {
  for (uint32_t i = 0; i < word_len; ++i) {
    if (i != 0) {
      stream << ' ';
    }
    stream << word[i];
  }
}

bool is_Auction_active(userState& state) {
  if (!state.hasActiveAuction()) {
    std::cout << "There is no on-going Auction. Please start a Auction using the "
                 "'start' command."
              << std::endl;
  }
  return state.hasActiveAuction();
}

void print_Auction_progress(userState& state) {
  if (!state.hasActiveAuction()) {
    return;
  }
  std::cout << std::endl << "Word progress: ";
  write_word(std::cout, state.Auction->getWordProgress(),
             state.Auction->getWordLen());
  std::cout << std::endl;
  std::cout << "Current trial: " << state.Auction->getCurrentTrial() << std::endl;
  std::cout << "Number of errors: " << state.Auction->getNumErrors() << "/"
            << state.Auction->getMaxErrors() << std::endl
            << std::endl;
}
*/
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
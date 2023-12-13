#include "commands.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

// #include "client_Auction.hpp"
#include "../common/protocol.hpp"

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
  // Check if user is already logged in
  if (state.isLoggedIn()) {
    std::cout 
        << "Failed to login: the user [" << state.getUserId()
        << "] is already logged in."  << std::endl;
    return;
  }
  
  uint32_t user_id;
  auto splitIndex = args.find(' ');
  std::string user_id_str = args.substr(0, splitIndex);
  args.erase(0, splitIndex + 1);
  std::string password = args;
  
  // Argument parsing
  try {
    user_id = parse_user_id(user_id_str);
  } catch (...) {
    std::cout << "Invalid user ID. It must be "<< USER_ID_STR_LEN 
              << " digits" << std::endl;
    return;
  }
  // Check if Password is too long
  if (password.length() != PASSWORD_LEN) {
    std::cout << "Invalid password. It must be " << PASSWORD_LEN 
              << " characters long" << std::endl;
    return;
  }
  // Check if Password is AlphaNumeric
  if (!is_alphanumeric(password)) {
    std::cout << "Invalid password. It must be alphanumeric" << std::endl;
    return;
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
      // Registered user is still logged in
      state.login(user_id, password);
      std::cout << "New user registered successfully!" << std::endl;
      break;

    case ReplyLoginClientbound::status::ERR:

      std::cout
          << "Failed to login: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
    default:
      std::cout 
          << "Something went wrong. Please try again." << std::endl;
      break;
    }
}

void LogoutCommand::handle(std::string args, UserState& state) {
  //avoid unused parameter warning
  (void) args;
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to logout: you need to be logged in to logout." 
        << std::endl;
    return;
  }

    // Populate and send packet
  LogoutServerbound packet_out;
  packet_out.user_id = state.getUserId();
  packet_out.password = state.getPassword();

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
      std::cout 
          << "Failed to logout: the user is not registered." 
          << std::endl;
      break;

    case ReplyLogoutClientbound::status::ERR:
    default:
      std::cout
          << "Failed to logout: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void UnregisterCommand::handle(std::string args, UserState& state) {

  //avoid unused parameter warning
  (void) args;
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to unregister: you need to be logged in to unregister." 
        << std::endl;
    return;
  }

  // Populate and send packet
  UnregisterServerbound packet_out;
  packet_out.user_id = state.getUserId();
  packet_out.password = state.getPassword();

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
      std::cout 
          << "Failed to un-register: the user is not registered." 
          << std::endl;
      break;

    case ReplyUnregisterClientbound::status::ERR:
    default:
      std::cout
          << "Failed to un-register: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void ExitCommand::handle(std::string args, UserState& state) {
  //avoid unused parameter warning
  (void) args;
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
  std::istringstream iss(args);
  std::string auction_name;
  std::string asset_file_name;
  std::string start_value_str;
  std::string timeactive_str;
  
  if (!(iss >> auction_name) || !(iss >> asset_file_name) || 
      !(iss >> start_value_str) || !(iss >> timeactive_str)) {
    std::cout << "Invalid arguments. Usage: openauction <auction_name> "
              << "<asset_name> <start_value> <timeactive>" << std::endl;
    return;
  }
  // Check if asset_name is too long
  if (auction_name.length() > AUCTION_NAME_MAX_LENGTH) {
    std::cout << "Invalid auction name. It must be at most " << AUCTION_NAME_MAX_LENGTH 
              << " characters long" << std::endl;
    return;
  }
  // Check if asset_name is AlphaNumeric
  if (!is_alphanumeric(auction_name)) {
    std::cout << "Invalid auction name. It must be alphanumeric" << std::endl;
    return;
  }
  // Check if start_value_str is too long
  if (start_value_str.length() > AUCTION_START_VALUE_MAX_LEN) {
    std::cout << "Invalid start value. It must be at most " << AUCTION_START_VALUE_MAX_LEN 
              << " characters long" << std::endl;
    return;
  }
  // Check if timeactive_str is too long
  if (timeactive_str.length() > AUCTION_DURATION_MAX_VALUE) {
    std::cout << "Invalid auction duration. It must be at most " << AUCTION_DURATION_MAX_VALUE 
              << " characters long" << std::endl;
    return;
  }

  // Convert start_value_str to uint32_t
  uint32_t start_value = static_cast<uint32_t>(std::stoi(start_value_str, NULL, 10));
  uint32_t timeactive = static_cast<uint32_t>(std::stoi(timeactive_str, NULL, 10));



  // Populate and send packet
  OpenAuctionServerbound packet_out;
  packet_out.user_id = state.getUserId();
  packet_out.password = state.getPassword();
  packet_out.auction_name = auction_name;
  packet_out.start_value = start_value;
  packet_out.time_active = timeactive;
  packet_out.file_name = asset_file_name;
  // TODO: Remove this constant and leave the asset_file_name as the path
  packet_out.file_path = std::filesystem::path(ASSETS_RELATIVE_DIRERCTORY) / asset_file_name;


  ReplyOpenAuctionClientbound roa;
  state.sendTcpPacketAndWaitForReply(packet_out, roa);

  switch (roa.status) {
    case ReplyOpenAuctionClientbound::status::OK:
      // Output open auction info
      std::cout << "Auction opened successfully with ID ["<< roa.auction_id
                << "]!" << std::endl;
      break;

    case ReplyOpenAuctionClientbound::status::NOK:
      std::cout
          << "Failed to open auction: it could not be started."
          << std::endl;
      break;

    case ReplyOpenAuctionClientbound::status::NLG:
      std::cout 
          << "Failed to open auction: the user is not logged in." 
          << std::endl;
      break;

    case ReplyOpenAuctionClientbound::status::ERR:
    default:
      std::cout
          << "Failed to open auction: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void CloseAuctionCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to close auction: you need to be logged in to close an auction." 
        << std::endl;
    return;
  }

  // Argument parsing
  std::istringstream iss(args);
  std::string auction_id_str;
  
  if (!(iss >> auction_id_str)) {
    std::cout << "Invalid arguments. Usage: close <auction_id>" << std::endl;
    return;
  }
  // Check if auction_id_str is too long
  if (auction_id_str.length() > AUCTION_ID_MAX_LEN) {
    std::cout << "Invalid auction ID. It must be at most " << AUCTION_ID_MAX_LEN 
              << " digits long" << std::endl;
    return;
  }
  // Check if auction_id_str is Numeric
  if (!is_numeric(auction_id_str)) {
    std::cout << "Invalid auction ID. It must be a number" << std::endl;
    return;
  }
  // Convert auction_id_str to uint32_t
  uint32_t auction_id = static_cast<uint32_t>(std::stoi(auction_id_str, NULL, 10));

  // Populate and send packet
  CloseAuctionServerbound packet_out;
  packet_out.user_id = state.getUserId();
  packet_out.password = state.getPassword();
  packet_out.auction_id = auction_id;

  ReplyCloseAuctionClientbound rcl;
  state.sendTcpPacketAndWaitForReply(packet_out, rcl);

  switch (rcl.status) {
    case ReplyCloseAuctionClientbound::status::OK:
      // Output close auction info
      std::cout << "Auction closed successfully!" << std::endl;
      break;

    case ReplyCloseAuctionClientbound::status::EAU:
      std::cout
          << "Failed to close auction: the auction with ID [" 
          << auction_id << "] does not exist." << std::endl;
      break;
    
    case ReplyCloseAuctionClientbound::status::EOW:
      std::cout
          << "Failed to close auction: the auction with ID [" 
          << auction_id << "] is not owned by this user." << std::endl;
      break;
    
    case ReplyCloseAuctionClientbound::status::END:
      std::cout
          << "Failed to close auction: the auction with ID [" 
          << auction_id << "] has already ended." << std::endl;
      break;

    case ReplyCloseAuctionClientbound::status::NLG:
      std::cout 
          << "Failed to close auction: the user is not logged in." 
          << std::endl;
      break;

    case ReplyCloseAuctionClientbound::status::ERR:
    default:
      std::cout
          << "Failed to close auction: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void ListMyAuctionsCommand::handle(std::string args, UserState& state) {
  
  //avoid unused parameter warning
  (void) args;

  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to list auctions: you need to be logged in to list your auctions." 
        << std::endl;
    return;
  }

  // Populate and send packet
  ListMyAuctionsServerbound packet_out;
  packet_out.user_id = state.getUserId();

  ReplyListMyAuctionsClientbound rma;
  state.sendUdpPacketAndWaitForReply(packet_out, rma);

  switch (rma.status) {
    case ReplyListMyAuctionsClientbound::status::OK:
      // Output autions info
      std::cout << "Displaying the auctions started by you:" << std::endl;
      printAuctions(rma.auctions);
      break;

    case ReplyListMyAuctionsClientbound::status::NLG:
      std::cout 
          << "Failed to list auctions: the user has to be logged in." 
          << std::endl;
      break;
    
    case ReplyListMyAuctionsClientbound::status::NOK:
      std::cout
          << "Failed to list auctions: the user has 0 ongoing auctions."
          << std::endl;
      break;

    case ReplyListMyAuctionsClientbound::status::ERR:
    default:
      std::cout
          << "Failed to list auctions: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void MyBidsCommand::handle(std::string args, UserState& state) {
  (void) args;
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to list bids: you need to be logged in to list your bids." 
        << std::endl;
    return;
  }

  // Populate and send packet
  MyBidsServerbound packet_out;
  packet_out.user_id = state.getUserId();

  ReplyMyBidsClientbound rmb;
  state.sendUdpPacketAndWaitForReply(packet_out, rmb);

  switch (rmb.status) {
    case ReplyMyBidsClientbound::status::OK:
      // Output bids info
      std::cout 
          << "Displaying the auctions where you made a bid:" 
          << std::endl;
      printAuctions(rmb.auctions);
      break;

    case ReplyMyBidsClientbound::status::NLG:
      std::cout 
          << "Failed to list bids: the user has to be logged in." 
          << std::endl;
      break;
    
    case ReplyMyBidsClientbound::status::NOK:
      std::cout
          << "Failed to list bids: the user has 0 ongoing bids."
          << std::endl;
      break;

    case ReplyMyBidsClientbound::status::ERR:
    default:
      std::cout
          << "Failed to list bids: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void ListCommand::handle(std::string args, UserState& state) {
  (void) args;
  // Populate and send packet
  ListAuctionsServerbound packet_out;

  ReplyListAuctionsClientbound rls;
  state.sendUdpPacketAndWaitForReply(packet_out, rls);

  switch (rls.status) {
    case ReplyListAuctionsClientbound::status::OK:
      // Output autions info
      std::cout << "Displaying all the auctions:" << std::endl;
      printAuctions(rls.auctions);
      break;

    case ReplyListAuctionsClientbound::status::NOK:
      std::cout
          << "Failed to list auctions: there are no ongoing auctions."
          << std::endl;
      break;

    case ReplyListAuctionsClientbound::status::ERR:
    default:
      std::cout
          << "Failed to list auctions: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void ShowAssetCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to show asset: you need to be logged in to show an asset." 
        << std::endl;
    return;
  }

  // Argument parsing
  std::istringstream iss(args);
  std::string auction_id_str;
  
  if (!(iss >> auction_id_str)) {
    std::cout << "Invalid arguments. Usage: show_asset <auction_id>" << std::endl;
    return;
  }
  // Check if auction_id_str is too long
  if (auction_id_str.length() > AUCTION_ID_MAX_LEN) {
    std::cout << "Invalid auction ID. It must be at most " << AUCTION_ID_MAX_LEN 
              << " digits long" << std::endl;
    return;
  }
  // Check if auction_id_str is Numeric
  if (!is_numeric(auction_id_str)) {
    std::cout << "Invalid auction ID. It must be a number" << std::endl;
    return;
  }
  // Convert auction_id_str to uint32_t
  uint32_t auction_id = static_cast<uint32_t>(std::stoi(auction_id_str, NULL, 10));

  // Populate and send packet
  ShowAssetServerbound packet_out;
  packet_out.auction_id = auction_id;

  ReplyShowAssetClientbound rsa;
  state.sendTcpPacketAndWaitForReply(packet_out, rsa);

  switch (rsa.status) {
    case ReplyShowAssetClientbound::status::OK:
      // Output asset info
      std::cout 
          << "Displaying the asset '"<< rsa.file_name 
          <<"' of the auction with ID [" << auction_id << "]:" 
          << std::endl;
      display_file(rsa.file_path);
      break;

    case ReplyShowAssetClientbound::status::NOK:
      std::cout
          << "Failed to show asset: the auction with ID [" 
          << auction_id << "] does not exist." << std::endl;
      break;

    case ReplyShowAssetClientbound::status::ERR:
    default:
      std::cout
          << "Failed to show asset: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void BidCommand::handle(std::string args, UserState& state) {
  // Check if user is logged in
  if (!state.isLoggedIn()) {
    std::cout 
        << "Failed to bid: you need to be logged in to bid." 
        << std::endl;
    return;
  }

  // Argument parsing
  std::istringstream iss(args);
  std::string auction_id_str;
  std::string bid_value_str;
  
  if (!(iss >> auction_id_str) || !(iss >> bid_value_str)) {
    std::cout << "Invalid arguments. Usage: bid <auction_id> <bid_value>" << std::endl;
    return;
  }
  // Check if auction_id_str is too long
  if (auction_id_str.length() > AUCTION_ID_MAX_LEN) {
    std::cout << "Invalid auction ID. It must be at most " << AUCTION_ID_MAX_LEN 
              << " digits long" << std::endl;
    return;
  }
  // Check if auction_id_str is Numeric
  if (!is_numeric(auction_id_str)) {
    std::cout << "Invalid auction ID. It must be a number" << std::endl;
    return;
  }
  // Check if bid_value_str is too long
  if (bid_value_str.length() > BID_MAX_LEN) {
    std::cout << "Invalid bid value. It must be at most " << BID_MAX_LEN 
              << " digits long" << std::endl;
    return;
  }
  // Check if bid_value_str is Numeric
  if (!is_numeric(bid_value_str)) {
    std::cout << "Invalid bid value. It must be a number" << std::endl;
    return;
  }
  // Convert auction_id_str to uint32_t
  uint32_t auction_id = static_cast<uint32_t>(std::stoi(auction_id_str, NULL, 10));
  // Convert bid_value_str to uint32_t
  uint32_t bid_value = static_cast<uint32_t>(std::stoi(bid_value_str, NULL, 10));

  // Populate and send packet
  BidServerbound packet_out;
  packet_out.user_id = state.getUserId();
  packet_out.password = state.getPassword();
  packet_out.auction_id = auction_id;
  packet_out.bid_value = bid_value;

  ReplyBidClientbound rbd;
  state.sendTcpPacketAndWaitForReply(packet_out, rbd);

  switch (rbd.status) {
    case ReplyBidClientbound::status::ACC:
      // Output bid info
      std::cout 
          << "Bid of " << bid_value << " on auction with ID [" 
          << auction_id << "] accepted!" << std::endl;
      break;

    case ReplyBidClientbound::status::NOK:
      std::cout
          << "Failed to bid: the auction with ID [" 
          << auction_id << "] has already ended." << std::endl;
      break;

    case ReplyBidClientbound::status::NLG:
      std::cout 
          << "Failed to bid: the user is not logged in." 
          << std::endl;
      break;
    case ReplyBidClientbound::status::ILG:
      std::cout 
          << "Failed to bid: the user cannot bid on one of his own auctions." 
          << std::endl;
      break;

    case ReplyBidClientbound::status::REF:
      std::cout 
          << "Failed to bid: the bid of " << bid_value << " was lower than"
          << " the current bid of the auction [" << auction_id << "]."
          << std::endl;
      break;

    case ReplyBidClientbound::status::ERR:
    default:
      std::cout
          << "Failed to bid: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

void ShowRecordCommand::handle(std::string args, UserState& state) {
  // Argument parsing
  std::istringstream iss(args);
  std::string auction_id_str;
  
  if (!(iss >> auction_id_str)) {
    std::cout << "Invalid arguments. Usage: show_record <auction_id>" << std::endl;
    return;
  }
  // Check if auction_id_str is too long
  if (auction_id_str.length() > AUCTION_ID_MAX_LEN) {
    std::cout << "Invalid auction ID. It must be at most " << AUCTION_ID_MAX_LEN 
              << " digits long" << std::endl;
    return;
  }
  // Check if auction_id_str is Numeric
  if (!is_numeric(auction_id_str)) {
    std::cout << "Invalid auction ID. It must be a number" << std::endl;
    return;
  }
  // Convert auction_id_str to uint32_t
  uint32_t auction_id = static_cast<uint32_t>(std::stoi(auction_id_str, NULL, 10));

  // Populate and send packet
  ShowRecordServerbound packet_out;
  packet_out.auction_id = auction_id;

  ReplyShowRecordClientbound rrc;
  state.sendUdpPacketAndWaitForReply(packet_out, rrc);

  switch (rrc.status) {
    case ReplyShowRecordClientbound::status::OK:
      // Output Auction & Bids info
      std::cout 
          << "Displaying the record of the auction with ID [" << auction_id << "]:" 
          << std::endl;
      std::cout << "Auction host: " << rrc.auction.getUidString() << std::endl;
      std::cout << "Auction name: " << rrc.auction.getName() << std::endl;
      std::cout << "Auction asset: " << rrc.auction.getAssetFname() << std::endl;
      std::cout << "Auction start value: " << rrc.auction.getInitialBid() << std::endl;
      
      std::cout 
          << "Auction start date: " 
        	<< formatTime(rrc.auction.getStartTime(), "%Y-%m-%d %H:%M:%S") 
          << std::endl;
      std::cout 
          << "Auction time active: " << rrc.auction.getDurationSeconds()
          << std::endl;
      if (rrc.auction.isActive()) {
        std::cout << "Auction status: Ongoing" << std::endl;
      } else {
        std::cout << "Auction status: Closed" << std::endl;
        std::cout 
            <<  "Auction end date: " 
            << formatTime(rrc.auction.getEndTime(), "%Y-%m-%d %H:%M:%S")
            << std::endl;
        std::cout <<  "Auction end seconds: " << rrc.auction.getEndTimeSec() << std::endl;
      }
      std::cout <<  "----------- Last 50 bids: -----------" << std::endl;
      printBidsInfo(rrc.auction.getBids());
      break;

    case ReplyShowRecordClientbound::status::NOK:
      std::cout 
          << "Failed to show record: the auction [" 
          << auction_id << "] does not exist." << std::endl;
      break;

    case ReplyShowRecordClientbound::status::ERR:
    default:
      std::cout
          << "Failed to show record: an unknown error occurred on the server "
          << "side. Please try again." << std::endl;
      break;
  }
}

std::string formatTime(std::time_t timeValue, const std::string& format) {
    // Convert time_t to struct tm
    std::tm* timeInfo = std::gmtime(&timeValue); // Use gmtime for UTC time

    // Format the time
    std::ostringstream formattedTime;
    formattedTime << std::put_time(timeInfo, format.c_str());

    return formattedTime.str();
}

void printBidsInfo(const std::vector<Bid> bids) {
    for (const auto bid : bids) {
        std::cout << "Bidder User ID: " << bid.bidder_user_id << std::endl;
        std::cout << "Bid Value: " << bid.bid_value << std::endl;
        std::cout << "Date Time: " << formatTime(bid.date_time, "%Y-%m-%d %H:%M:%S")
                  << ", Seconds: " << bid.sec_time << std::endl;
        std::cout << "---------------------------------------" << std::endl;
    }
}

void printAuctions(const std::vector<std::pair<uint32_t, bool>>& auctions) {
    for (const auto& auction : auctions) {
        std::cout 
            << "Auction ID: " << auction.first
            << " - Status: " << (auction.second ? "Active" : "Inactive") 
            << std::endl;
    }
}

bool is_alphanumeric(std::string& str) {
  for (char c : str) {
    if (!isalnum(c)) {
      return false;
    }
  }
  return true;
}

bool is_numeric(std::string& str) {
  for (char c : str) {
    if (!isdigit(c)) {
      return false;
    }
  }
  return true;
}


uint32_t parse_user_id(std::string& args) {
  size_t converted = 0;
  if(args.length() != USER_ID_STR_LEN){
    throw std::runtime_error("invalid user id");
  }

  uint32_t user_id = static_cast<uint32_t>(std::stoi(args, &converted, 10));
  if (converted != args.length() || user_id > USER_ID_MAX) {
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
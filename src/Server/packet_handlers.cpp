#include "packet_handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

// UDP

void handle_login_user(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state)
{
  LoginServerbound packet;
  ReplyLoginClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    UserData user(packet.user_id, packet.password, state.file_manager);

    user.login();
    response.status = ReplyLoginClientbound::OK;

    state.cdebug << userTag(packet.user_id) << "User logged in" << std::endl;
  }
  catch (WrongPasswordException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Wrong password" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyLoginClientbound::REG;
  }
  catch (UserIdException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid user id" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  }
  catch (UserPasswordException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid user password" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyLoginClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyLoginClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyLoginClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[Login] There was an unhandled exception that prevented "
                 "the server from logging in:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_logout_user(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state)
{
  LogoutServerbound packet;
  ReplyLogoutClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    UserData user(packet.user_id, packet.password, state.file_manager);
    user.logout();
    response.status = ReplyLogoutClientbound::OK;
    state.cdebug << userTag(packet.user_id) << "User logged out" << std::endl;

    // duvida -> aqui é suposto verificar a password????
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyLogoutClientbound::UNR;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyLogoutClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyLogoutClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyLogoutClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyLogoutClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[Logout] There was an unhandled exception that prevented "
                 "the server from logging out:"
              << e.what() << std::endl;
    return;
  }
  printf("Sending packet response to LOGOUT\n");
  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

void handle_unregister_user(std::stringstream &buffer, Address &addr_from,
                            AuctionServerState &state)
{

  UnregisterServerbound packet;
  ReplyUnregisterClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    UserData user(packet.user_id, packet.password, state.file_manager);

    user.unregisterUser();
    response.status = ReplyUnregisterClientbound::OK;
    state.cdebug << userTag(packet.user_id) << "User logged out" << std::endl;
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyUnregisterClientbound::UNR;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyUnregisterClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyUnregisterClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyUnregisterClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyUnregisterClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[Unregister] There was an unhandled exception that prevented "
                 "the server from unregistering:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

void handle_list_myauctions(std::stringstream &buffer, Address &addr_from,
                            AuctionServerState &state)

{
  ListMyAuctionsServerbound packet;
  ReplyListMyAuctionsClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to list auctions"
                 << std::endl;

    UserData user(packet.user_id, state.file_manager);
    std::vector<std::pair<uint32_t, bool>> auctions = user.listMyAuctions("HOSTED");
    response.status = ReplyListMyAuctionsClientbound::OK;
    response.auctions = auctions;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyListMyAuctionsClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (UserHasNoAuctionsException &e)
  {
    response.status = ReplyListMyAuctionsClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User has no auctions" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyListMyAuctionsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyListMyAuctionsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyListMyAuctionsClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[ListMyAuctions] There was an unhandled exception that prevented "
                 "the server from listing auctions:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

void handle_list_mybids(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state)

{
  MyBidsServerbound packet;
  ReplyMyBidsClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to list bids"
                 << std::endl;

    UserData user(packet.user_id, state.file_manager);
    std::vector<std::pair<uint32_t, bool>> auctions = user.listMyAuctions("BIDDED");
    response.status = ReplyMyBidsClientbound::OK;
    response.auctions = auctions;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyMyBidsClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (UserHasNoAuctionsException &e)
  {
    response.status = ReplyMyBidsClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User has no bids" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyMyBidsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyMyBidsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyMyBidsClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[ListMyBids] There was an unhandled exception that prevented "
                 "the server from listing auctions:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

void handle_list_auctions(std::stringstream &buffer, Address &addr_from,
                          AuctionServerState &state)
{
  ListAuctionsServerbound packet;
  ReplyListAuctionsClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << "Asked to list auctions"
                 << std::endl;

    std::vector<std::pair<uint32_t, bool>> auctions = state.file_manager.getAllAuctions();
    response.status = ReplyListAuctionsClientbound::OK;
    response.auctions = auctions;
  }

  catch (NoAuctionsException &e)
  {
    response.status = ReplyListAuctionsClientbound::NOK;
    state.cdebug << "No auctions" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << "Failed to open file" << std::endl;
    //TODO response.status = ReplyListAuctionsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyListAuctionsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyListAuctionsClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[ListAuctions] There was an unhandled exception that prevented "
                 "the server from listing auctions:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

void handle_show_record(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state)
{
  ShowRecordServerbound packet;
  ReplyShowRecordClientbound response;

  try
  {
    packet.deserialize(buffer);
    state.cdebug << auctionTag(packet.auction_id) << "Asked to show record"
                 << std::endl;

    AuctionData auction = state.file_manager.getAuction(std::to_string(packet.auction_id));

    response.status = ReplyShowRecordClientbound::OK;
    response.auction = auction;
  }
  catch (AuctionDoesNotExistException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction does not exist" << std::endl;
    response.status = ReplyShowRecordClientbound::NOK;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyShowRecordClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyShowRecordClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyShowRecordClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[ShowRecord] There was an unhandled exception that prevented "
                 "the server from showing record:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr, addr_from.size);
}

// TCP

void handle_open_auction(int connection_fd, AuctionServerState &state)
{

  OpenAuctionServerbound packet;
  ReplyOpenAuctionClientbound response;

  try
  {
    packet.receive(connection_fd);

    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    UserData user(packet.user_id, state.file_manager);

    // start auction with time now in format YYYY-MM-DD HH:MM:SS

    time_t now = time(0);

    AuctionData auction(state.auctionsCount++, packet.user_id, packet.auction_name, packet.start_value, packet.time_active, packet.file_name, now, 0, 0, std::vector<Bid>());

    if (packet.file_size > ASSET_MAX_BYTES)
    {
      throw InvalidAuctionAssetException(packet.file_name);
    }

    user.openAuction(auction, packet.password);

    response.auction_id = auction.getId();

    response.status = ReplyOpenAuctionClientbound::OK;

    state.cdebug << packet.auction_name << "Auction started" << std::endl;
  }
  catch (WrongPasswordException &e) 
  {
    state.cdebug << userTag(packet.user_id) << "Wrong password" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyOpenAuctionClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (AuctionIdException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction id" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (InvalidAuctionNameException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction item name" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (InvalidAuctionInitialBidException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction initial bid" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (InvalidAuctionDurationException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction duration" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (InvalidAuctionAssetException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction asset fname or size" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  }
  catch (std::exception &e)
  {

    std::cerr << "[OpenAuction] There was an unhandled exception that prevented "
                 "the server from opening the auction:"
              << e.what() << std::endl;

    return;
  }

  response.send(connection_fd);
}

void handle_close_auction(int connection_fd, AuctionServerState &state)
{

  CloseAuctionServerbound packet;
  ReplyCloseAuctionClientbound response;

  try
  {
    packet.receive(connection_fd);
    state.cdebug << userTag(packet.user_id) << "Asked to close Auction"
                 << std::endl;

    UserData user(packet.user_id, state.file_manager);

    AuctionData auction = state.file_manager.getAuction(std::to_string(packet.auction_id));

    user.closeAuction(auction);

    response.status = ReplyCloseAuctionClientbound::OK;

    state.cdebug << packet.auction_id << "Auction closed" << std::endl;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyCloseAuctionClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    //TODO response.status = ReplyCloseAuctionClientbound::ERR; (Maybe NLG?)
  }
  catch (AuctionDoesNotExistException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction does not exist" << std::endl;
    response.status = ReplyCloseAuctionClientbound::EAU;
  }
  catch (AuctionDoesNotBelongToUserException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction not owned by user" << std::endl;
    response.status = ReplyCloseAuctionClientbound::EOW;
  }
  catch (AuctionNotActiveException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction already closed" << std::endl;
    response.status = ReplyCloseAuctionClientbound::EOW;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    //TODO response.status = ReplyCloseAuctionClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    //TODO response.status = ReplyCloseAuctionClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    //TODO response.status = ReplyCloseAuctionClientbound::ERR;
  }
  catch (std::exception &e)
  {

    std::cerr << "[CloseAuction] There was an unhandled exception that prevented "
                 "the server from closing the auction:"
              << e.what() << std::endl;

    return;
  }
  response.send(connection_fd);
}

void handle_show_asset(int connection_fd, AuctionServerState &state)
{

  ShowAssetServerbound packet;
  ReplyShowAssetClientbound response;

  try
  {
    packet.receive(connection_fd);

    AuctionData auction = state.file_manager.getAuction(std::to_string(packet.auction_id));

    std::filesystem::path asset_path = state.file_manager.showAsset(auction);

    response.status = ReplyShowAssetClientbound::OK;

    response.file_path = asset_path;

    state.cdebug << auctionTag(packet.auction_id) << "Asset shown" << std::endl;
  }
  catch (AuctionDoesNotExistException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction does not exist" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (InvalidFilePathException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Invalid file path" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (AssetDoesNotExistException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Asset does not exist" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to open file" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to write to file" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (FileReadException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Failed to read from file" << std::endl;
    response.status = ReplyShowAssetClientbound::NOK;
  }
  catch (std::exception &e)
  {

    std::cerr << "[ShowAsset] There was an unhandled exception that prevented "
                 "the server from showing the asset:"
              << e.what() << std::endl;

    return;
  }

  response.send(connection_fd);
}

void handle_bid(int connection_fd, AuctionServerState &state)
{

  BidServerbound packet;
  ReplyBidClientbound response;

  try
  {
    packet.receive(connection_fd);

    UserData user(packet.user_id, packet.password, state.file_manager);

    AuctionData auction = state.file_manager.getAuction(std::to_string(packet.auction_id));

    user.bid(auction, packet.bid_value, packet.password);

    response.status = ReplyBidClientbound::ACC;

    state.cdebug << auctionTag(packet.auction_id) << "Bid made" << std::endl;
  }
  catch (WrongPasswordException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Wrong password" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (UserNotLoggedInException &e)
  {
    response.status = ReplyBidClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (UserNotRegisteredException &e)
  {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (AuctionIdException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Invalid auction id" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (AuctionDoesNotExistException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction does not exist" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  
  catch (AuctionNotActiveException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Auction not active" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }

  catch (BidValueException &e)
  {
    state.cdebug << auctionTag(packet.auction_id) << "Bid value too low" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (UserIsOwnerException &e) {
    state.cdebug << auctionTag(packet.auction_id) << "Cannot bid on own auction" << std::endl;
    response.status = ReplyBidClientbound::ILG;
  }
  catch ( LargerBidAlreadyExistsException &e) {
    state.cdebug << auctionTag(packet.auction_id) << "Larger bid already exists" << std::endl;
    response.status = ReplyBidClientbound::REF;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyBidClientbound::NOK;
  }
  catch (std::exception &e)
  {

    std::cerr << "[Bid] There was an unhandled exception that prevented "
                 "the server from bidding:"
              << e.what() << std::endl;

    return;
  }

  response.send(connection_fd);
}

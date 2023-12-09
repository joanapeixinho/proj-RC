#include "packet_handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../common/protocol.hpp"

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
    response.status = ReplyLoginClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyLoginClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyLoginClientbound::ERR;
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
  catch (UserNotLoggedInException) {
    response.status = ReplyLogoutClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyLogoutClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyLogoutClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyLogoutClientbound::ERR;
  }
  catch (std::exception &e)
  {
    std::cerr << "[Logout] There was an unhandled exception that prevented "
                 "the server from logging out:"
              << e.what() << std::endl;
    return;
  }

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
  catch (UserNotLoggedInException) {
    response.status = ReplyUnregisterClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyUnregisterClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyUnregisterClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyUnregisterClientbound::ERR;
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
    std::vector <std::pair<uint32_t, bool>> auctions = user.listMyAuctions("HOSTED");
    response.status = ReplyListMyAuctionsClientbound::OK;
    response.auctions = auctions;
    
  }
  catch (UserNotLoggedInException) {
    response.status = ReplyListMyAuctionsClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch(UserHasNoAuctionsException &e) {
    response.status = ReplyListMyAuctionsClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User has no auctions" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyListMyAuctionsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyListMyAuctionsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyListMyAuctionsClientbound::ERR;
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
    std::vector <std::pair<uint32_t, bool>> auctions = user.listMyAuctions("BIDDED");
    response.status = ReplyMyBidsClientbound::OK;
    response.auctions = auctions;
    
  }
  catch (UserNotLoggedInException) {
    response.status = ReplyMyBidsClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch(UserHasNoAuctionsException &e) {
    response.status = ReplyMyBidsClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User has no bids" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyMyBidsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyMyBidsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyMyBidsClientbound::ERR;
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

    std::vector <std::pair<uint32_t, bool>> auctions = state.file_manager.getAllAuctions();
    response.status = ReplyListAuctionsClientbound::OK;
    response.auctions = auctions;
    
  }
  
  catch(NoAuctionsException) {
    response.status = ReplyListAuctionsClientbound::NOK;
    state.cdebug << "No auctions" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug <<  "Failed to open file" << std::endl;
    response.status = ReplyListAuctionsClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug <<  "Failed to write to file" << std::endl;
    response.status = ReplyListAuctionsClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << "Failed to read from file" << std::endl;
    response.status = ReplyListAuctionsClientbound::ERR;
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
    state.cdebug << userTag(packet.user_id) << "Asked to show record"
                 << std::endl;

    UserData user(packet.user_id, state.file_manager);
    std::vector <std::pair<uint32_t, bool>> auctions = user.listMyAuctions("ENDED");
    response.status = ReplyShowRecordClientbound::OK;
    response.auctions = auctions;
    
  }
  catch (UserNotLoggedInException) {
    response.status = ReplyShowRecordClientbound::NLG;
    state.cdebug << userTag(packet.user_id) << "User not logged in" << std::endl;
  }
  catch(UserHasNoAuctionsException &e) {
    response.status = ReplyShowRecordClientbound::NOK;
    state.cdebug << userTag(packet.user_id) << "User has no auctions" << std::endl;
  }
  catch (FileOpenException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyShowRecordClientbound::ERR;
  }
  catch (FileWriteException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyShowRecordClientbound::ERR;
  }
  catch (FileReadException &e)
  {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyShowRecordClientbound::ERR;
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

        AuctionData auction(state.auctionsCount++, packet.auction_name, packet.start_value, packet.time_active, packet.file_name);

        user.openAuction(auction);

        response.status = ReplyOpenAuctionClientbound::OK;

        state.cdebug << userTag(packet.user_id) << "Auction started" << std::endl;

    }
    catch (UserNotLoggedInException)
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
      state.cdebug << userTag(packet.user_id) << "Invalid auction asset fname" << std::endl;
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

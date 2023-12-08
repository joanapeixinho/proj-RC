#include "packet_handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "../common/protocol.hpp"

void handle_login_user(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state) {
  LoginServerbound packet;
  ReplyLoginClientbound response;

  try {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    UserData user(packet.user_id, packet.password);

    user.login();

    state.loggedInUser = user;

    response.status = ReplyLoginClientbound::OK;

    state.cdebug << userTag(packet.user_id) << "User logged in" << std::endl;
  } catch (WrongPasswordException &e) {
    state.cdebug << userTag(packet.user_id) << "Wrong password" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  } catch ( UserNotRegisteredException &e) {
    state.cdebug << userTag(packet.user_id) << "User not registered" << std::endl;
    response.status = ReplyLoginClientbound::REG;
  } catch (UserIdException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid user id" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  } catch (UserPasswordException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid user password" << std::endl;
    response.status = ReplyLoginClientbound::NOK;
  } catch (FileOpenException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyLoginClientbound::ERR;
  } catch (FileWriteException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyLoginClientbound::ERR;
  } catch (FileReadException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyLoginClientbound::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Login] There was an unhandled exception that prevented "
                 "the server from logging in:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_open_auction(std::stringstream &buffer, Address &addr_from,
                         AuctionServerState &state) {

  OpenAuctionServerbound packet;
  ReplyOpenAuctionClientbound response;

  try {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    AuctionData auction(auctionsCount++, packet.item_name, packet.initial_bid, packet.duration_seconds, packet.asset_fname);

    state.loggedInUser.openAuction(auction);

    response.status = ReplyOpenAuctionClientbound::OK;

    state.cdebug << userTag(packet.user_id) << "Auction started" << std::endl;
  } catch (AuctionIdException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid auction id" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  } catch (InvalidAuctionNameException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid auction item name" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  } catch (InvalidAuctionInitialBidException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid auction initial bid" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  } catch (InvalidAuctionDurationException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid auction duration" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  } catch (InvalidAuctionAssetException &e) {
    state.cdebug << userTag(packet.user_id) << "Invalid auction asset fname" << std::endl;
    response.status = ReplyOpenAuctionClientbound::NOK;
  } catch (FileOpenException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to open file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::ERR;
  } catch (FileWriteException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to write to file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::ERR;
  } catch (FileReadException &e) {
    state.cdebug << userTag(packet.user_id) << "Failed to read from file" << std::endl;
    response.status = ReplyOpenAuctionClientbound::ERR;
  } catch (std::exception &e) {

    std::cerr << "[OpenAuction] There was an unhandled exception that prevented "
                 "the server from opening the auction:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);


}

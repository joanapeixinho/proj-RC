#include "packet_handlers.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "common/protocol.hpp"

void handle_start_Auction(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state) {
  StartAuctionServerbound packet;
  ReplyStartAuctionClientbound response;

  try {
    packet.deserialize(buffer);
    state.cdebug << userTag(packet.user_id) << "Asked to start Auction"
                 << std::endl;

    AuctionServerSync Auction = state.createAuction(packet.user_id);

    response.status = ReplyStartAuctionClientbound::OK;
   

    Auction->saveToFile();

    state.cdebug << userTag(packet.user_id) << "Auction started with word '"
                 << Auction->getWord() << "' and with " << Auction->getMaxErrors()
                 << " errors allowed" << std::endl;
  } catch (AuctionAlreadyStartedException &e) {
    state.cdebug << userTag(packet.user_id) << "Auction already started"
                 << std::endl;
    response.status = ReplyStartAuctionClientbound::NOK;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Start Auction] Invalid packet received" << std::endl;
    response.status = ReplyStartAuctionClientbound::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Start Auction] There was an unhandled exception that prevented "
                 "the server from starting a new Auction:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}


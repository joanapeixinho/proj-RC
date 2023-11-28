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
    response.n_letters = Auction->getWordLen();
    response.max_errors = Auction->getMaxErrors();

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

void handle_guess_letter(std::stringstream &buffer, Address &addr_from,
                         AuctionServerState &state) {
  GuessLetterServerbound packet;
  GuessLetterClientbound response;
  try {
    packet.deserialize(buffer);

    state.cdebug << userTag(packet.user_id) << "Guessed letter '"
                 << packet.guess << "'" << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    response.trial = Auction->getCurrentTrial();
    auto found = Auction->guessLetter(packet.guess, packet.trial);
    // Must set trial again in case of replays
    response.trial = Auction->getCurrentTrial() - 1;

    Auction->saveToFile();

    if (Auction->hasLost()) {
      response.status = GuessLetterClientbound::status::OVR;
      state.cdebug << userTag(packet.user_id) << "Auction lost" << std::endl;
    } else if (found.size() == 0) {
      response.status = GuessLetterClientbound::status::NOK;
      state.cdebug << userTag(packet.user_id) << "Wrong letter '"
                   << packet.guess << "'" << std::endl;
    } else if (Auction->hasWon()) {
      response.status = GuessLetterClientbound::status::WIN;
      state.scoreboard.addAuction(Auction.Auction);
      state.cdebug << userTag(packet.user_id) << "Won the Auction. Word was '"
                   << Auction->getWord() << "'" << std::endl;
    } else {
      response.status = GuessLetterClientbound::status::OK;
      state.cdebug << userTag(packet.user_id) << "Correct letter '"
                   << packet.guess << "'. Trial " << response.trial
                   << ". Progress: " << Auction->getWordProgress() << std::endl;
    }
    response.pos = found;
  } catch (NoAuctionFoundException &e) {
    response.status = GuessLetterClientbound::status::ERR;
    state.cdebug << userTag(packet.user_id) << "No Auction found" << std::endl;
  } catch (DuplicateLetterGuessException &e) {
    response.status = GuessLetterClientbound::status::DUP;
    response.trial -= 1;
    state.cdebug << userTag(packet.user_id) << "Guessed duplicate letter '"
                 << packet.guess << "'" << std::endl;
  } catch (InvalidTrialException &e) {
    response.status = GuessLetterClientbound::status::INV;
    response.trial -= 1;
    state.cdebug << userTag(packet.user_id) << "Invalid trial "
                 << packet.trial << std::endl;
  } catch (AuctionHasEndedException &e) {
    response.status = GuessLetterClientbound::status::ERR;
    state.cdebug << userTag(packet.user_id) << "Auction has ended"
                 << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = GuessLetterClientbound::status::ERR;
    state.cdebug << "[Guess Letter] Invalid packet received" << std::endl;
  } catch (std::exception &e) {
    std::cerr << "[Guess Letter] There was an unhandled exception that "
                 "prevented the server from handling a letter guess:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_guess_word(std::stringstream &buffer, Address &addr_from,
                       AuctionServerState &state) {
  GuessWordServerbound packet;
  GuessWordClientbound response;
  try {
    packet.deserialize(buffer);

    state.cdebug << userTag(packet.user_id) << "Guessed word '"
                 << packet.guess << "'" << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    response.trial = Auction->getCurrentTrial();
    bool correct = Auction->guessWord(packet.guess, packet.trial);
    // Must set trial again in case of replays
    response.trial = Auction->getCurrentTrial() - 1;

    Auction->saveToFile();

    if (Auction->hasLost()) {
      response.status = GuessWordClientbound::status::OVR;
      state.cdebug << userTag(packet.user_id) << "Auction lost" << std::endl;
    } else if (correct) {
      response.status = GuessWordClientbound::status::WIN;
      state.scoreboard.addAuction(Auction.Auction);
      state.cdebug << userTag(packet.user_id) << "Guess was correct"
                   << std::endl;
    } else {
      response.status = GuessWordClientbound::status::NOK;
      state.cdebug << userTag(packet.user_id) << "Guess was wrong. Trial "
                   << response.trial << " Progress: " << Auction->getWordProgress()
                   << std::endl;
    }
  } catch (NoAuctionFoundException &e) {
    response.status = GuessWordClientbound::status::ERR;
    state.cdebug << userTag(packet.user_id) << "No Auction found" << std::endl;
  } catch (DuplicateWordGuessException &e) {
    response.status = GuessWordClientbound::status::DUP;
    response.trial -= 1;
    state.cdebug << userTag(packet.user_id) << "Guessed duplicate word '"
                 << packet.guess << "'" << std::endl;
  } catch (InvalidTrialException &e) {
    response.status = GuessWordClientbound::status::INV;
    response.trial -= 1;
    state.cdebug << userTag(packet.user_id)
                 << "Invalid trial sent. Trial sent: " << packet.trial
                 << ". Correct trial: " << (response.trial + 1) << std::endl;
  } catch (AuctionHasEndedException &e) {
    response.status = GuessWordClientbound::status::ERR;
    state.cdebug << userTag(packet.user_id) << "Auction has ended"
                 << std::endl;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Guess Word] Invalid packet" << std::endl;
    response.status = GuessWordClientbound::status::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Guess Word] There was an unhandled exception that prevented "
                 "the server from handling a word guess:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_quit_Auction(std::stringstream &buffer, Address &addr_from,
                      AuctionServerState &state) {
  QuitAuctionServerbound packet;
  QuitAuctionClientbound response;
  try {
    packet.deserialize(buffer);

    state.cdebug << userTag(packet.user_id) << "Quitting Auction" << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    if (Auction->isOnGoing()) {
      Auction->finishAuction();
      response.status = QuitAuctionClientbound::status::OK;
      state.cdebug << userTag(packet.user_id) << "Fulfilling quit request"
                   << std::endl;
    } else {
      response.status = QuitAuctionClientbound::status::NOK;
      state.cdebug << userTag(packet.user_id) << "Auction had already ended"
                   << std::endl;
    }

    Auction->saveToFile();

  } catch (NoAuctionFoundException &e) {
    response.status = QuitAuctionClientbound::status::NOK;
    state.cdebug << userTag(packet.user_id) << "No Auction found" << std::endl;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Quit] Invalid packet" << std::endl;
    response.status = QuitAuctionClientbound::status::ERR;
  } catch (std::exception &e) {
    std::cerr << "[Quit] There was an unhandled exception that prevented the "
                 "server from handling a quit Auction request:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_reveal_word(std::stringstream &buffer, Address &addr_from,
                        AuctionServerState &state) {
  RevealWordServerbound packet;
  RevealWordClientbound response;
  try {
    packet.deserialize(buffer);

    state.cdebug << userTag(packet.user_id) << "Asked to reveal word"
                 << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    state.cdebug << userTag(packet.user_id) << "Word is " << Auction->getWord()
                 << std::endl;

    response.word = Auction->getWord();
  } catch (NoAuctionFoundException &e) {
    // The protocol says we should not reply if there is not an on-going Auction
    state.cdebug << userTag(packet.user_id) << "No Auction found" << std::endl;
    return;
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Reveal] Invalid packet" << std::endl;
    // Propagate error to reply with "ERR", since there is no error code here
    throw;
  } catch (std::exception &e) {
    std::cerr << "[Reveal] There was an unhandled exception that prevented the "
                 "server from handling a word reveal:"
              << e.what() << std::endl;
    return;
  }

  send_packet(response, addr_from.socket, (struct sockaddr *)&addr_from.addr,
              addr_from.size);
}

void handle_scoreboard(int connection_fd, AuctionServerState &state) {
  ScoreboardServerbound packet;
  ScoreboardClientbound response;
  try {
    packet.receive(connection_fd);

    state.cdebug << "[Scoreboard] Received request" << std::endl;

    auto scoreboard_str = state.scoreboard.toString();
    if (scoreboard_str.has_value()) {
      response.status = ScoreboardClientbound::status::OK;
      response.file_name = "scoreboard.txt";
      response.file_data = scoreboard_str.value();
      state.cdebug << "[Scoreboard] Sending back scoreboard as per request"
                   << std::endl;
    } else {
      response.status = ScoreboardClientbound::status::EMPTY;
      state.cdebug
          << "[Scoreboard] There are no won Auctions, sending empty scoreboard"
          << std::endl;
    }
  } catch (InvalidPacketException &e) {
    state.cdebug << "[Scoreboard] Invalid packet" << std::endl;
    // Propagate error to reply with "ERR", since there is no error code here
    throw;
  } catch (std::exception &e) {
    std::cerr << "[Scoreboard] There was an unhandled exception that prevented "
                 "the server from handling a scoreboard request:"
              << e.what() << std::endl;
    return;
  }

  response.send(connection_fd);
}

void handle_hint(int connection_fd, AuctionServerState &state) {
  HintServerbound packet;
  HintClientbound response;
  try {
    packet.receive(connection_fd);

    state.cdebug << userTag(packet.user_id) << "Requested hint"
                 << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    if (!Auction->isOnGoing()) {
      response.status = HintClientbound::status::NOK;
      state.cdebug << userTag(packet.user_id)
                   << "Fulfilling hint request: Auction had already ended."
                   << std::endl;
    } else if (!Auction->getHintFilePath().has_value() ||
               !std::filesystem::exists(Auction->getHintFilePath().value())) {
      response.status = HintClientbound::status::NOK;
      state.cdebug << userTag(packet.user_id)
                   << "Current Auction doesn't have a hint to send." << std::endl;
    } else {
      response.status = HintClientbound::status::OK;
      response.file_path = Auction->getHintFilePath().value();
      response.file_name = Auction->getHintFileName();
      state.cdebug << userTag(packet.user_id)
                   << "Fulfilling hint request: sending hint from file: "
                   << Auction->getHintFilePath().value() << std::endl;
    }
  } catch (NoAuctionFoundException &e) {
    response.status = HintClientbound::status::NOK;
    state.cdebug << userTag(packet.user_id) << "Auction not found"
                 << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = HintClientbound::status::NOK;
    state.cdebug << "[Hint] Invalid packet" << std::endl;
  } catch (std::exception &e) {
    std::cerr << "[Hint] There was an unhandled exception that prevented the "
                 "server from handling a hint request:"
              << e.what() << std::endl;
    return;
  }

  response.send(connection_fd);
}

void handle_state(int connection_fd, AuctionServerState &state) {
  StateServerbound packet;
  StateClientbound response;
  try {
    packet.receive(connection_fd);

    state.cdebug << userTag(packet.user_id) << "Requested Auction state"
                 << std::endl;

    AuctionServerSync Auction = state.getAuction(packet.user_id);

    if (Auction->isOnGoing()) {
      response.status = StateClientbound::status::ACT;
      state.cdebug << userTag(packet.user_id)
                   << "Fulfilling state request: sending active Auction."
                   << std::endl;
    } else {
      response.status = StateClientbound::status::FIN;
      state.cdebug << userTag(packet.user_id)
                   << "Fulfilling state request: sending last finished Auction."
                   << std::endl;
    }
    std::stringstream file_name;
    file_name << "state_" << std::setfill('0') << std::setw(user_ID_MAX_LEN)
              << Auction->getuserId() << ".txt";
    response.file_name = file_name.str();
    response.file_data = Auction->getStateString();
  } catch (NoAuctionFoundException &e) {
    response.status = StateClientbound::status::NOK;
    state.cdebug << userTag(packet.user_id) << "Auction not found"
                 << std::endl;
  } catch (InvalidPacketException &e) {
    response.status = StateClientbound::status::NOK;
    state.cdebug << "[State] Invalid packet" << std::endl;
  } catch (std::exception &e) {
    std::cerr
        << "[State] There was an unhandled exception that prevented the server "
           "from handling a state request:"
        << e.what() << std::endl;
    return;
  }

  response.send(connection_fd);
}
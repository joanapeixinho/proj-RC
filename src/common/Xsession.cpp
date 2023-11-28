#include "Auction.hpp"

#include <cstring>

#include "protocol.hpp"

uint32_t Auction::getWordLen() {
  return wordLen;
}

uint32_t Auction::getMaxErrors() {
  return maxErrors;
}

uint32_t Auction::getuserId() {
  return userId;
}

uint32_t Auction::getCurrentTrial() {
  return currentTrial;
}

uint32_t Auction::getGoodTrials() {
  return currentTrial - numErrors - 1;
}

uint32_t Auction::getNumErrors() {
  return numErrors;
}

bool Auction::isOnGoing() {
  return onGoing;
}

void Auction::finishAuction() {
  onGoing = false;
}

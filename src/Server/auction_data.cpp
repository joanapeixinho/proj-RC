#include "auction_data.hpp"

AuctionData::AuctionData(int id, const std::string& itemName, double initialBid, int durationSeconds)
    : id(id), itemName(itemName), initialBid(initialBid), durationSeconds(durationSeconds) {}

uint32_t AuctionData::getId() const {
    return id;
}

const std::string& AuctionData::getItemName() const {
    return itemName;
}

double AuctionData::getInitialBid() const {
    return initialBid;
}

int AuctionData::getDurationSeconds() const {
    return durationSeconds;
}

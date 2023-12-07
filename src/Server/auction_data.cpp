#include "auction_data.hpp"

AuctionData::AuctionData(int id, const std::string& itemName, double initialBid, int durationSeconds, const std::string& assetFname)
    : id(id), itemName(itemName), initialBid(initialBid), durationSeconds(durationSeconds) {
    this->assetFname = assetFname;
    this->startTime = std::time(nullptr);
    }

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

std::string AuctionData::toString() const {
    std::stringstream ss;
    ss << std::setw(6) << std::setfill('0') << id << " "; // UID
    ss << itemName << " "; // name
    ss << assetFname << " "; // asset fname
    ss << initialBid << " "; // start value
    ss << durationSeconds << " "; // timeactive
    ss << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S") << " "; // start datetime
    ss << startTime; // start fulltime
    return ss.str();
}

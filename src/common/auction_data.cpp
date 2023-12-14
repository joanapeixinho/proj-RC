#include "auction_data.hpp"
#include <iostream>


AuctionData::AuctionData(uint32_t inputId, uint32_t inputUid, const std::string& inputName,
                         uint32_t inputInitialBid, uint32_t inputDurationSeconds,
                         const std::string& inputAssetFname, std::time_t inputStartTime,
                         std::time_t inputEndTime, uint32_t inputEndTimeSec,
                         const std::vector<Bid> &inputBids) :  id(inputId), uid(inputUid), name(inputName), initialBid(inputInitialBid),
      durationSeconds(inputDurationSeconds), startTime(inputStartTime), assetFname(inputAssetFname),
      endTime(inputEndTime), endTimeSec(inputEndTimeSec),
      bids(inputBids) 
{
    // Validações
    if (id > AUCTION_MAX_NUMBER) {
        throw InvalidAuctionIdException(std::to_string(id));
    }


    if (name.length() > AUCTION_NAME_MAX_LENGTH) {
        throw InvalidAuctionNameException(name);
    }

    if (initialBid > BID_MAX_VALUE) {
        throw InvalidAuctionInitialBidException(std::to_string(initialBid));
    }

    if (durationSeconds > AUCTION_DURATION_MAX_VALUE) {
        throw InvalidAuctionDurationException(std::to_string(durationSeconds));
    }

    if (assetFname.length() > ASSET_NAME_MAX_LENGTH) {
        throw InvalidAuctionAssetException(assetFname);
    }
}


std::string AuctionData::getIdString() const {
    std::ostringstream oss;
    oss << std::setw(AUCTION_ID_MAX_LEN) << std::setfill('0') << id;
    return oss.str();
}

std::string AuctionData::idToString(uint32_t id) {
    std::ostringstream oss;
    oss << std::setw(AUCTION_ID_MAX_LEN) << std::setfill('0') << id;
    return oss.str();
}

uint32_t AuctionData::getId() const {
    return id;
}

std::string AuctionData::getUidString() const {
    std::ostringstream oss;
    oss << std::setw(USER_ID_STR_LEN) << std::setfill('0') << uid;
    return oss.str();
}

uint32_t AuctionData::getOwnerId() const {
    return uid;
}

const std::string& AuctionData::getName() const {
    return name;
}

uint32_t AuctionData::getInitialBid() const {
    return initialBid;
}

uint32_t AuctionData::getDurationSeconds() const {
    return durationSeconds;
}

std::string AuctionData::toString() const {
    std::stringstream ss;
    ss << std::setw(USER_ID_STR_LEN) << std::setfill('0') << uid << " "; // UID
    ss << name << " "; // name
    ss << assetFname << " "; // asset fname
    ss << initialBid << " "; // start value
    ss << durationSeconds << " "; // timeactive
    ss << std::put_time(std::localtime(&startTime), "%Y-%m-%d %H:%M:%S") << " "; // start datetime
    ss << startTime; // start fulltime
    return ss.str();
}

std::string AuctionData::getAssetFname() const {
    return assetFname;
}

std::time_t AuctionData::getStartTime() const {
    return startTime;
}

std::time_t AuctionData::getEndTime() const {
    return endTime;
}

uint32_t AuctionData::getEndTimeSec() const {
    return endTimeSec;
}	

std::vector<Bid> AuctionData::getBids() const {
    return bids;
}

bool AuctionData::hasBids() const {
    return bids.size() > 0;
}

bool AuctionData::isActive() const {
    return active;
}

void AuctionData::setInactive() {
    active = false;
}

uint32_t AuctionData::getHighestBidValue() const {
    if (bids.empty()) {
        return initialBid;
    } else {
        //print bids.back().bid_value
        std :: cout << "bids.back().bid_value" << bids.back().bid_value << std :: endl;
        return bids.back().bid_value;
    }
    
}

void AuctionData::setOwnerId(uint32_t inputUid) {
    uid = inputUid;
}

void AuctionData::setName(const std::string& inputName) {
    name = inputName;
}

void AuctionData::setInitialBid(uint32_t inputInitialBid) {
    initialBid = inputInitialBid;
}

void AuctionData::setDurationSeconds(uint32_t inputDurationSeconds) {
    durationSeconds = inputDurationSeconds;
}

void AuctionData::setStartTime(std::time_t inputStartTime) {
    startTime = inputStartTime;
}

void AuctionData::setAssetFname(const std::string& inputAssetFname) {
    assetFname = inputAssetFname;
}

void AuctionData::setEndTime(std::time_t inputEndTime) {
    endTime = inputEndTime;
}

void AuctionData::setEndTimeSec(uint32_t inputEndTimeSec) {
    endTimeSec = inputEndTimeSec;
}

void AuctionData::addBid(Bid bid) {
    bids.push_back(bid);
}
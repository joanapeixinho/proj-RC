#include "auction_data.hpp"


 AuctionData::AuctionData(uint32_t id, uint32_t uid , std::string& name,
                             double initialBid, int durationSeconds, const std::string& assetFname, std::time_t endTime, std::time_t endTimeSec, std::time_t startTime
                             , std::vector<Bid> bids)
                             : id(id), uid(uid), name(name), initialBid(initialBid), durationSeconds(durationSeconds), 
        assetFname(assetFname), endTime(endTime), endTimeSec(endTimeSec), startTime(startTime), bids(bids)
    {

        if (id < 0 || id > AUCTION_MAX_NUMBER) {
            throw InvalidAuctionIdException(std::to_string(id));
        }

        if (name.length() > AUCTION_NAME_MAX_LENGTH) {
            throw InvalidAuctionNameException(name);
        }

        if (initialBid < 0 || initialBid > AUCTION_INITIAL_BID_MAX_VALUE) {
            throw InvalidAuctionInitialBidException(std::to_string(initialBid));
        }

        if (durationSeconds < 0 || durationSeconds > AUCTION_DURATION_MAX_VALUE) {
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

uint32_t AuctionData::getId() const {
    return id;
}

std::string AuctionData::getUidString() const {
    std::ostringstream oss;
    oss << std::setw(USER_ID_MAX_LEN) << std::setfill('0') << uid;
    return oss.str();
}

uint32_t AuctionData::getOwnerId() const {
    return uid;
}

const std::string& AuctionData::getName() const {
    return name;
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

int AuctionData::getEndTimeSec() const {
    return endTimeSec;
}	

std::vector<Bid> AuctionData::getBids() const {
    return bids;
}

bool AuctionData::isActive() const {
    return endTime > std::time(nullptr);
}

uint32_t AuctionData::getHighestBidValue() const {
    if (bids.empty()) {
        return initialBid;
    } else {
        return bids.back().bid_value;
    }
}


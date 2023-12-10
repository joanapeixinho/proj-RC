#include "auction_data.hpp"


 AuctionData::AuctionData(uint32_t id, uint32_t uid , std::string& name,
                             double initialBid, int durationSeconds, const std::string& assetFname, std::time_t endTime, std::time_t endTimeSec, std::time_t startTime
                             , std::vector<Bid> bids)
                             : id(id), uid(uid), name(name), initialBid(initialBid), durationSeconds(durationSeconds), 
        assetFname(assetFname), endTime(endTime), endTimeSec(endTimeSec), startTime(startTime), bids(bids)
    {
        startTime = std::time(nullptr);

        if (name.length() > AUCTION_NAME_MAX_LENGTH) {
            throw InvalidAuctionNameException(name);
        }

        if (initialBid < 0 || initialBid > AUCTION_INITIAL_BID_MAX_VALUE) {
            throw InvalidAuctionInitialBidException(std::to_string(initialBid));
        }

        if (durationSeconds < 0 || durationSeconds > AUCTION_DURATION_MAX_VALUE) {
            throw InvalidAuctionDurationException(std::to_string(durationSeconds));
        }
        
        //reached maximum auctions number

        if (id > AUCTION_MAX_NUMBER) {
            throw MaximumAuctionsException(std::to_string(id));
        }

    }

std::string AuctionData::getId() const {
    std::ostringstream oss;
    oss << std::setw(AUCTION_ID_MAX_LEN) << std::setfill('0') << id;
    return oss.str();
}

std::string AuctionData::getUid() const {
    std::ostringstream oss;
    oss << std::setw(USER_ID_MAX_LEN) << std::setfill('0') << uid;
    return oss.str();
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

bool AuctionData::isActive() const {
    
}


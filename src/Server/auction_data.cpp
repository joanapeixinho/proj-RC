#include "auction_data.hpp"


// Open a new auction. The User application sends a message to the AS asking to open
// a new auction, providing a short description name (represented with up to 10
// alphanumerical characters), an image (or other document file) of the asset to sell, the
// start value (represented with up to 6 digits), and the duration of the auction in
// seconds (represented with up to 5 digits). In reply, the AS informs if the request was
// successful, and the assigned auction identifier, AID, a 3-digit numbe

 AuctionData(const uint32_t, std::string& name, double initialBid, int durationSeconds, const std::string& assetFname)
        : id(id), name(name), initialBid(initialBid), durationSeconds(durationSeconds), assetFname(assetFname)
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

        if (assetFname.length() > AUCTION_ASSET_MAX_LENGTH) {
            throw InvalidAuctionAssetException(assetFname);
        }

        //reached maximum auctions number

        if (id > AUCTION_MAX_NUMBER) {
            throw MaximumAuctionsException(std::to_string(id));
        }

    }

std::string AuctionData :: getId() const {
        std::ostringstream oss;
        oss << std::setw(3) << std::setfill('0') << id;
        return oss.str();
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

std::string AuctionData::getAssetFname() const {
    return assetFname;
}

std::time_t AuctionData::getStartTime() const {
    return startTime;
}

std::time_t AuctionData::getEndTime() const {
    return startTime + durationSeconds;
}

std::string getAssetFname() const {
    return assetFname;
}


#ifndef AUCTIONDATA_HPP
#define AUCTIONDATA_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "common/constants.hpp"
#include "common/file_manager.hpp"

class AuctionData {
public:
    AuctionData(uint32_t id, std::string& name, double initialBid, int durationSeconds, const std::string& assetFname);
    void openAuction();
    std::string getId() const;
    std::string toString() const; 
    std::string getAssetFname() const;   




private:
    int id;
    int nextId;
    std::string itemName;
    double initialBid;
    int durationSeconds;
    std::time_t startTime;
    std::string assetFname;
    std::time_t endTime;
};

#endif  // AUCTIONDATA_HPP

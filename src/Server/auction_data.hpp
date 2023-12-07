#ifndef AUCTIONDATA_HPP
#define AUCTIONDATA_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

class AuctionData {
public:
    AuctionData(int id, const std::string& itemName, double initialBid, int durationSeconds);
    uint32_t getId() const;
    const std::string& getItemName() const;
    double getInitialBid() const;
    int getDurationSeconds() const;
    std::string toString() const;

private:
    int id;
    std::string itemName;
    double initialBid;
    int durationSeconds;
    std::time_t startTime;
    std::string assetFname;
    std::time_t endTime;
};

#endif  // AUCTIONDATA_HPP

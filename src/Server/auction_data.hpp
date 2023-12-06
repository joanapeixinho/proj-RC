#ifndef AUCTIONDATA_HPP
#define AUCTIONDATA_HPP

#include <string>

class AuctionData {
public:
    AuctionData(int id, const std::string& itemName, double initialBid, int durationSeconds);
    uint32_t getId() const;
    const std::string& getItemName() const;
    double getInitialBid() const;
    int getDurationSeconds() const;

private:
    int id;
    std::string itemName;
    double initialBid;
    int durationSeconds;
};

#endif  // AUCTIONDATA_HPP

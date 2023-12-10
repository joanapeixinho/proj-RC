#ifndef AUCTIONDATA_HPP
#define AUCTIONDATA_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>

#include "../common/constants.hpp"
#include "../common/file_manager.hpp"


struct Bid {
  uint32_t bidder_user_id;
  uint32_t bid_value;
  std::string date_time;
  uint32_t sec_time;
};

class AuctionData {
public:
    AuctionData() = default;
    AuctionData(uint32_t id, uint32_t uid, std::string& name, double initialBid, int durationSeconds, const std::string& assetFname, std::time_t endTime, std::time_t endTimeSec , std::time_t startTime, std::vector<Bid> bids);
    void openAuction();
    std::string getId() const;
    std::string toString() const; 
    std::string getAssetFname() const;   
    const std::string& getName() const;
    double getInitialBid() const;
    int getDurationSeconds() const;
    std::time_t getStartTime() const;
    std::time_t getEndTime() const;
    bool isActive() const;

private:
    int id;
    int uid;
    std::string name;
    double initialBid;
    int durationSeconds;
    std::time_t startTime;
    std::string assetFname;
    std::time_t endTime;
    std::time_t endTimeSec;
    std::vector<Bid> bids;
};



#endif  // AUCTIONDATA_HPP

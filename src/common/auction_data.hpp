#ifndef AUCTIONDATA_HPP
#define AUCTIONDATA_HPP

#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "constants.hpp"
#include "exceptions.hpp"

struct Bid {
  uint32_t bidder_user_id;
  uint32_t bid_value;
  std::string date_time;
  uint32_t sec_time;
};

class AuctionData {
public:
  AuctionData() = default;
  AuctionData(uint32_t inputId, uint32_t inputUid, const std::string &inputName,
              uint32_t inputInitialBid, uint32_t inputDurationSeconds,
              const std::string &inputAssetFname, std::time_t inputStartTime,
              std::string inputEndTime, uint32_t inputEndTimeSec,
              const std::vector<Bid> &inputBids);
  std::string getIdString() const;
  static std::string idToString(uint32_t id);
  uint32_t getId() const;
  std::string getUidString() const;
  std::string toString() const;
  std::string getAssetFname() const;
  const std::string &getName() const;
  uint32_t getInitialBid() const;
  uint32_t getDurationSeconds() const;
  std::time_t getStartTime() const;
  std::string getStartTimeString() const;
  std::string getEndTimeString() const;
  uint32_t getEndTimeSec() const;
  std::vector<Bid> getBids() const;
  bool hasBids() const;
  bool isActive() const;
  uint32_t getOwnerId() const;
  uint32_t getHighestBidValue() const;
  void setOwnerId(uint32_t uid);
  void setName(const std::string &name);
  void setInitialBid(uint32_t initialBid);
  void setDurationSeconds(uint32_t durationSeconds);
  void setStartTime(std::time_t startTime);
  void setAssetFname(const std::string &assetFname);
  void setEndTime(std::string endTime);
  void setEndTimeSec(uint32_t endTimeSec);
  void addBid(Bid bid);

private:
  uint32_t id;
  uint32_t uid;
  std::string name;
  uint32_t initialBid;
  uint32_t durationSeconds;
  std::time_t startTime;
  std::string assetFname;
  std::string endTime;
  uint32_t endTimeSec;
  std::vector<Bid> bids;
};

#endif // AUCTIONDATA_HPP

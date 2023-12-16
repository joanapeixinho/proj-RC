// file_manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

#include "auction_data.hpp"
#include "constants.hpp"
#include "exceptions.hpp"

class FileManager {
public:
  FileManager();
  void createUserDirectory(const std::string &userId);
  void addToUserDirectory(const std::string &userId,
                          const std::string &auctionId,
                          const std::string &directory);
  void removeUserDirectory(const std::string &userId);
  void createUserPassFile(const std::string &userId,
                          const std::string &password);
  void createUserLoginFile(const std::string &userId);
  void removeUserLoginFile(const std::string &userId);
  void removeUserFiles(const std::string &userId);
  void createAuctionDirectory(const std::string &auctionId);
  void removeUserAuctionFile(const std::string &userId,
                             const std::string &auctionId,
                             const std::string &directory);
  void createUserAuctionFile(const std::string &userId,
                             const std::string &auctionId,
                             const std::string &directory);
  void createAuctionStartFile(const std::string &auctionId,
                              const AuctionData &data);
  void createAuctionAssetFile(const std::string &auctionId,
                              const std::string &assetFname);
  void createAuctionEndFile(const std::string &auctionId,
                            const std::string &endTime,
                            const uint32_t &activeSeconds);
  void createBidsDirectory(const std::string &auctionId);
  void createBidFile(const std::string &auctionId, const std::string &bidValue,
                     const std::string &userId, std::time_t startTime);
  bool writeToFile(const std::string &fileName, const std::string &data,
                   const std::string &directory);
  std::string readFromFile(const std::string &fileName,
                           const std::string &directory);
  void safeLockUser(const std::string &userId, std::function<void()> func);
  void safeLockAuction(const std::string &auctionId,
                       std::function<void()> func);
  bool UserLoggedIn(const std::string &userId);
  bool UserRegistered(const std::string &userId);
  bool auctionIsActive(const std::string &auctionId);
  void UpdateAuction(const std::string &auctionId);
  std::string getUserPassword(const std::string &userId);
  void loginUser(const std::string &userId);
  void logoutUser(const std::string &userId);
  void registerUser(const std::string &userId, const std::string &password);
  void unregisterUser(const std::string &userId);
  std::vector<std::pair<uint32_t, bool>>
  getUserAuctions(const std::string &userId, const std::string &directory);
  std::vector<std::pair<uint32_t, bool>> getAllAuctions();
  AuctionData getAuction(const uint32_t auctionIdInt);
  std::vector<Bid> getAuctionBids(const std::string &auctionId);
  void openAuction(const std::string &userId, const AuctionData &data);
  void closeAuction(AuctionData &auction);
  std::filesystem::path showAsset(AuctionData &auction);
  void bid(AuctionData &auction, uint32_t bidValue, const std::string &userId);
  uint32_t getAuctionsCount();
  void shutdown();

private:
  std::map<std::string, std::mutex> userMutexes;
  std::map<std::string, std::mutex> auctionMutexes;
};

#endif
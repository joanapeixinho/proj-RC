// file_manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <map>
#include <mutex>
#include <functional>

#include "Server/auction_data.hpp"
#include "Server/user_data.hpp"
#include "common/constants.hpp"
#include "common/exceptions.hpp"




class FileManager {
public:
    void createUserDirectory(const std::string& userId);
    void removeUserDirectory(const std::string& userId);
    void createUserPassFile(const std::string& userId, const std::string& password);
    void removeUserPassFile(const std::string& userId);
    void createUserLoginFile(const std::string& userId);
    void removeUserLoginFile(const std::string& userId);
    void removeUserFiles(const std::string& userId);
    void createAuctionDirectory(const std::string& auctionId);
    void createUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    void removeUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    void createAuctionStartFile(const std::string& auctionId, const AuctionData& data);
    void createAuctionAssetFile(const std::string& auctionId, const std::string& assetData);
    void createAuctionEndFile(const std::string& auctionId, const std::time_t& endTime, const int& activeSeconds);
    void createBidsDirectory(const std::string& auctionId);
    void createBidFile(const std::string& auctionId, const std::string& bidValue);
    bool writeToFile(const std::string& fileName, const std::string& data, const std::string& directory);
    std::string readFromFile(const std::string& fileName, const std::string& directory);
    void safeLockUser(const std::string& userId, std::function<void()> func);
    void safeLockAuction(const std::string& auctionId, std::function<void()> func);
    std::uint32_t getUser(const std::string& userId);
    std::string getUserPassword(const std::string& userId);
    void FileManager::registerUser(const std::string& userId, const std::string& password);
    

private:
    std::map<std::string, std::mutex> userMutexes;
    std::map<std::string, std::mutex> auctionMutexes;
};

#endif
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

//include common
#include "../common/exceptions.hpp"
#include "../Server/auction_data.hpp"
#include "../common/constants.hpp"






class FileManager {
public:
    static void createUserDirectory(const std::string& userId);
    static void removeUserDirectory(const std::string& userId);
    static void createUserPassFile(const std::string& userId, const std::string& password);
    static void createUserLoginFile(const std::string& userId);
    static void removeUserLoginFile(const std::string& userId);
    static void removeUserFiles(const std::string& userId);
    static void createAuctionDirectory(const std::string& auctionId);
    static void removeUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    static void createUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    static void createAuctionStartFile(const std::string& auctionId, const AuctionData& data);
    static void createAuctionAssetFile(const std::string& auctionId, const std::string& assetData);
    static void createAuctionEndFile(const std::string& auctionId, const std::time_t& endTime, const int& activeSeconds);
    static void createBidsDirectory(const std::string& auctionId);
    static void createBidFile(const std::string& auctionId, const std::string& bidValue);
    static bool writeToFile(const std::string& fileName, const std::string& data, const std::string& directory);
    static std::string readFromFile(const std::string& fileName, const std::string& directory);
    static void safeLockUser(const std::string& userId, std::function<void()> func);
    static void safeLockAuction(const std::string& auctionId, std::function<void()> func);
    static bool UserLoggedIn(const std::string& userId);
    static bool UserRegistered(const std::string& userId);
    static bool AuctionActive(const std::string& auctionId);
    static void UpdateAuction(const std::string& auctionId);
    static std::string getUserPassword(const std::string& userId);
    static void loginUser(const std::string& userId);
    static void logoutUser(const std::string& userId);
    static void FileManager::registerUser(const std::string& userId, const std::string& password);
    static void FileManager::unregisterUser(const std::string& userId);
    static std::string FileManager::getUserAuctions(const std::string& userId);
    static void FileManager::listAllAuctions(const std::string& userId);
    static void FileManager::openAuction(const std::string& userId, const AuctionData& data);
    static void FileManager::closeAuction(const std::string& userId, const std::string& auctionId);

private:
    static std::map<std::string, std::mutex> userMutexes;
    static std::map<std::string, std::mutex> auctionMutexes;
};

#endif
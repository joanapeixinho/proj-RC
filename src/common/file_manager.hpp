// file_manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>

#include "Server/auction_data.hpp"
#include "Server/user_data.hpp"
#include "common/constants.hpp"


class FileManager {
public:
    void createUserDirectory(const std::string& userId);
    void removeUserDirectory(const std::string& userId);
    void createUserPassFile(const std::string& userId, const std::string& password);
    void removeUserPassFile(const std::string& userId);
    void createUserLoginFile(const std::string& userId, const std::string& login);
    void removeUserLoginFile(const std::string& userId);
    void removeUserFiles(const std::string& userId);
    void createAuctionDirectory(const std::string& auctionId);
    void createUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    void removeUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory);
    void createAuctionStartFile(const std::string& auctionId, 
    bool writeToFile(const std::string& fileName, const std::string& data, const std::string& directory);
    std::string readFromFile(const std::string& fileName, const std::string& directory);

};

#endif
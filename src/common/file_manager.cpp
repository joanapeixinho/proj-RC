#include "file_manager.hpp"


FileManager::FileManager() {
    std::filesystem::create_directory(BASE_DIR);
    std::string UserDir = std::string(BASE_DIR) + '/' + USER_DIR;
    std::filesystem::create_directory(UserDir);
    std::string AuctionDir = std::string(BASE_DIR) + '/' + AUCTION_DIR;
    std::filesystem::create_directory(AuctionDir);
}

bool FileManager::writeToFile(const std::string& filename, const std::string& data, const std::string& directory) {
    
    std::ofstream file(BASE_DIR + directory + '/' + filename);
    
    if (!file.is_open()) {
        throw FileOpenException(filename);
        return false;
    }

    file << data;

    if (!file.good()) {
        throw FileWriteException(filename);
        return false;
    }

    file.close();
    
    return true;
}

std::string FileManager::readFromFile(const std::string& filename, const std::string& directory) {
    std::ifstream file(BASE_DIR + directory + '/' + filename);
    std::string data;
    
    if (!file.is_open()) {
        throw FileOpenException(filename);
        return "";
    }

    std::getline(file, data, '\0'); 
    file.close();

    if (data.empty()) {
        throw FileReadException(filename);
        return "";
    }
    
    return data;
}

void FileManager::safeLockUser(const std::string& userId, std::function<void()> func) {
    try {
        std::lock_guard<std::mutex> lock(userMutexes[userId]);
        func();
    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "An unknown exception occurred.\n";
    }
}

void FileManager::safeLockAuction(const std::string& auctionId, std::function<void()> func) {
    try {
        std::lock_guard<std::mutex> lock(auctionMutexes[auctionId]);
        func();
    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "An unknown exception occurred.\n";
    }
}

void FileManager::createUserDirectory(const std::string& userId) {
    std::string UserDir = std::string(USER_DIR) + '/' + userId;
    std::filesystem::create_directory(UserDir);
    std::filesystem::create_directory(UserDir + "/HOSTED");
    std::filesystem::create_directory(UserDir + "/BIDDED");

}


void FileManager:: createUserPassFile(const std::string& userId, const std::string& password) {
    std::ofstream file(USER_DIR + '/' + userId + "_pass.txt");
    file << password;
    if (!file.good()) {
        throw FileWriteException(userId + "_pass.txt");
    }
    file.close();
}


void FileManager:: createUserLoginFile(const std::string& userId) {
    std::ofstream file(USER_DIR + '/' + userId + "_login.txt");
    file.close();
}

void FileManager::removeUserLoginFile(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR)  + userId + "_login.txt");
}

void FileManager::removeUserFiles(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR)  + userId + "_pass.txt");
    std::filesystem::remove(std::string(USER_DIR)  + userId + "_login.txt");
}

void FileManager::createAuctionDirectory(const std::string& auctionId) {
    std::filesystem::create_directory(AUCTION_DIR  + '/' + auctionId);
}


void FileManager::createUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory) {
    std::ofstream file(USER_DIR + '/' + userId + '/' + directory + '/' + auctionId);
    file.close();
}

void FileManager::removeUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory) {
    std::filesystem::remove(USER_DIR + '/' + userId + '/' + directory + '/' + auctionId);
}

void FileManager::createAuctionStartFile(const std::string& auctionId, const AuctionData& data) {
    if (!writeToFile("START (" + auctionId + ").txt", data.toString(), AUCTION_DIR + '/' + auctionId)) {
        std::cerr << "Unable to create START file for auction: " << auctionId << std::endl;
    }
}

void FileManager::createAuctionAssetFile(const std::string& auctionId, const std::string& assetData) {
    /* TO DO */
}

void FileManager::createAuctionEndFile(const std::string& auctionId, const std::time_t& endTime, const int& activeSeconds) {
    std::stringstream ss;
    ss << std::put_time(std::localtime(&endTime), "%Y-%m-%d %H:%M:%S") << " ";
    ss << activeSeconds;
    if (!writeToFile("END (" + auctionId + ").txt", ss.str(), AUCTION_DIR + '/' + auctionId)) {
        std::cerr << "Unable to create END file for auction: " << auctionId << std::endl;
    }
}

void FileManager::createBidsDirectory(const std::string& auctionId) {
    std::filesystem::create_directory(AUCTION_DIR + '/' + auctionId + "/BIDS");
}

void FileManager::createBidFile(const std::string& auctionId, const std::string& bidValue) {
    std::ofstream file(AUCTION_DIR + '/' + auctionId + "/BIDS/(" + bidValue + ").txt");
    file.close();
}

std::string FileManager::getUserPassword(const std::string& userId) {
    if (UserRegistered(userId)) {
        return readFromFile(userId + "_pass.txt", USER_DIR);
    }
}


bool FileManager::UserLoggedIn(const std::string& userId) {
    return std::filesystem::exists(USER_DIR + '/' + userId + "_login.txt");
}

bool FileManager::UserRegistered(const std::string& userId) {
    return std::filesystem::exists(USER_DIR + '/' + userId + "_pass.txt");
}

/*Returns True if END file exists and therefore auction is active*/
bool FileManager::auctionIsActive(const std::string& auctionId) {
    return !std::filesystem::exists(AUCTION_DIR + '/' + auctionId + "/END (" + auctionId + ").txt");
}

void FileManager::loginUser(const std::string& userId) {
    safeLockUser(userId, [&]() {
    createUserLoginFile(userId);
    });
   
}

void FileManager::logoutUser(const std::string& userId) {
    safeLockUser(userId, [&]() {
    removeUserLoginFile(userId);
    });

}

 void FileManager::registerUser(const std::string& userId, const std::string& password) {
    
    
    safeLockUser(userId, [&]() {
    createUserDirectory(userId);
    });

    safeLockUser(userId, [&]() {
    createUserPassFile(userId, password);
    });

}

void FileManager::unregisterUser(const std::string& userId) {
    safeLockUser(userId, [&]() {
    removeUserFiles(userId);
    });
}


std::vector<std::pair<uint32_t, bool>> FileManager::getUserAuctions(const std::string& userId, const std::string& directory) {
    std::vector<std::pair<uint32_t, bool>> auctionList;
    safeLockUser(userId, [&]() {
        for (const auto& entry : std::filesystem::directory_iterator(USER_DIR + '/' + userId + '/' + directory)) {
            //update auction
            std::string auctionId = entry.path().filename().string();
            UpdateAuction(auctionId);

            //check if auction is active
            bool isActive = auctionIsActive(auctionId);
            uint32_t intAuctionId = std::stoi(auctionId);

            //add to list
            auctionList.push_back(std::make_pair(intAuctionId, isActive));
        }
    });

    return auctionList;
}

std::vector<std::pair<uint32_t, bool>> FileManager::getAllAuctions() {
    std::vector<std::pair<uint32_t, bool>> auctionList;
    for (const auto& entry : std::filesystem::directory_iterator(AUCTION_DIR)) {
        if (std::filesystem::is_directory(entry.status())) {
            //update auction
            std::string auctionId = entry.path().filename().string();
            safeLockAuction(auctionId, [&]() {
                UpdateAuction(auctionId);
                bool isActive = auctionIsActive(auctionId);
                uint32_t intAuctionId = std::stoi(auctionId);

                auctionList.push_back(std::make_pair(intAuctionId, isActive));
            });
        }
    }

    if (auctionList.empty()) {
        throw NoAuctionsException();
    }

    return auctionList;
}

AuctionData FileManager::getAuction(const std::string& auctionId) {
    //update Auction
    safeLockAuction(auctionId, [&]() {
        UpdateAuction(auctionId);

        std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
        std::stringstream ss(startFile);
        std::string uid, name, assetFname, startValue, timeActive, startDatetime, startFulltime;
        std::getline(ss, uid, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, assetFname, ' ');
        std::getline(ss, startValue, ' ');
        std::getline(ss, timeActive, ' ');
        std::getline(ss, startDatetime, ' ');
        double initialBid = std::stod(startValue);
        int durationSeconds = std::stoi(timeActive);
        std::time_t startTime = std::stol(startFulltime);

        if (!auctionIsActive(auctionId)) {
            std::string endFile = readFromFile("END (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
            std::stringstream ss(endFile);
            std::string endDatetime, endSecTime;
            std::getline(ss, endDatetime, ' ');
            std::getline(ss, endSecTime, ' ');
            std::time_t endTimeSec = std::stoi(endSecTime);
            std::time_t endTime = std::stol(endDatetime);
            std :: vector<Bid> bids = getAuctionBids(auctionId);
            AuctionData data(std::stoi(auctionId),std::stoi(uid),  name, initialBid,
                             durationSeconds, assetFname, endTime, endTimeSec, startTime, bids);
            return data;
        }

        std :: vector<Bid> bids = getAuctionBids(auctionId);
        AuctionData data(std::stoi(auctionId), std::stoi(uid), name, initialBid,
                         durationSeconds, assetFname, 0, 0, startTime, bids);
        return data;
    });
}

std::vector<Bid> FileManager::getAuctionBids(const std::string& auctionId) {
    std::vector<Bid> bids;
    for (const auto& entry : std::filesystem::directory_iterator(AUCTION_DIR + '/' + auctionId + "/BIDS")) {
        std::string bidValue = entry.path().filename().string();
        std::string bidFile = readFromFile(bidValue, AUCTION_DIR + '/' + auctionId + "/BIDS");        std::stringstream ss(bidFile);
        std::string bidder_user_id, bid_value, date_time, sec_time;
        std::getline(ss, bidder_user_id, ' ');
        std::getline(ss, bid_value, ' ');
        std::getline(ss, date_time, ' ');
        std::getline(ss, sec_time, ' ');
        Bid bid;
        bid.bidder_user_id = std::stoi(bidder_user_id);
        bid.bid_value = std::stoi(bid_value);
        bid.date_time = date_time;
        bid.sec_time = std::stoi(sec_time);
        bids.push_back(bid);
    }
    return bids;
}

void FileManager::openAuction(const std::string& userId, const AuctionData& data) {
    
    std::string auctionId = data.getId();
    
    safeLockUser(userId, [&]() {
    createUserAuctionFile(userId, auctionId, "HOSTED");
    });
    safeLockAuction(auctionId, [&]() {
    createAuctionDirectory(auctionId);
    });
    safeLockAuction(auctionId, [&]() {
    createAuctionStartFile(auctionId, data);
    });
    safeLockAuction(auctionId, [&]() {
    createBidsDirectory(auctionId);
    });
    safeLockAuction(auctionId, [&]() {
    createAuctionAssetFile(auctionId, data.getAssetFname());
    });
}

/* check START FILE to see if the endtime has passed */
/* if it has, create END FILE */

void FileManager::UpdateAuction(const std::string& auctionId) {
    safeLockAuction(auctionId, [&]() {
        if (auctionIsActive(auctionId)) {
            std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
            std::stringstream ss(startFile);
            std::string uid, name, assetFname, startValue, timeActive, startDatetime, startFulltime;
            std::getline(ss, uid, ' ');
            std::getline(ss, name, ' ');
            std::getline(ss, assetFname, ' ');
            std::getline(ss, startValue, ' ');
            std::getline(ss, timeActive, ' ');
            std::getline(ss, startDatetime, ' ');
            std::getline(ss, startFulltime, ' ');
            std::time_t startTime = std::stol(startFulltime);
            int durationSeconds = std::stoi(timeActive);
            std::time_t endTime = startTime + durationSeconds;
            std::time_t now = std::time(nullptr);
            if (now > endTime) {
                createAuctionEndFile(auctionId, endTime, durationSeconds);
            }
        }
    });
}


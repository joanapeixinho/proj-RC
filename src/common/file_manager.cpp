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
bool FileManager::AuctionActive(const std::string& auctionId) {
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
            bool isActive = AuctionActive(auctionId);
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
                bool isActive = AuctionActive(auctionId);
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

AuctionData FileManager::getAuction (const std::string& auctionId) {

    //update Auction
    safeLockAuction(auctionId, [&]() {
        UpdateAuction(auctionId);
    });

    std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
    std::stringstream ss(startFile);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    std::string name = tokens[0];
    double initialBid = std::stod(tokens[2]);
    int durationSeconds = std::stoi(tokens[4]);
    std::time_t startTime = std::stol(tokens[6]);
    std::string assetFname = readFromFile("ASSET (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
    std::time_t endTime = startTime + durationSeconds;
    AuctionData data(std::stoi(auctionId), name, initialBid, durationSeconds, assetFname);
    return data;
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
        if (AuctionActive(auctionId)) {
            std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + '/' + auctionId);
            std::stringstream ss(startFile);
            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(ss, token, ' ')) {
                tokens.push_back(token);
            }
            std::time_t startTime = std::stol(tokens[6]);
            int durationSeconds = std::stoi(tokens[4]);
            std::time_t endTime = startTime + durationSeconds;
            std::time_t now = std::time(nullptr);
            if (now > endTime) {
                createAuctionEndFile(auctionId, endTime, durationSeconds);
            }
        }
    });
}



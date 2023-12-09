#include "file_manager.hpp"


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
    std::ifstream file(directory + '/' + filename);
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

void FileManager::removeUserDirectory(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR) + "/" + userId);
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
    if (std::filesystem::exists(USER_DIR + '/' + userId + "_pass.txt")) {
        return readFromFile(userId + "_pass.txt", USER_DIR);
    }
}


bool FileManager::UserInDir(const std::string& userId) {
    return std::filesystem::exists(USER_DIR + '/' + userId);
}

std::string FileManager::getUserPassword(const std::string& userId) {
    if (std::filesystem::exists(USER_DIR + '/' + userId + "_pass.txt")) {
        return readFromFile(userId + "_pass.txt", USER_DIR);
    }
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
    safeLockUser(userId, [&]() {
    createUserLoginFile(userId);
    });
}

void FileManager::unregisterUser(const std::string& userId) {
    safeLockUser(userId, [&]() {
    removeUserFiles(userId);
    });
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


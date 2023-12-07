#include "file_manager.hpp"

bool FileManager::writeToFile(const std::string& fileName, const std::string& data, const std::string& directory) {
    
    std::ofstream file(BASE_DIR + directory + '/' + fileName);
    
    if (!file.is_open()) {
        std::cerr << 'Failed opening file: ' << fileName << std::endl;
        return false;
    }

    file << data;
    file.close();
    
    return true;
}

std::string FileManager::readFromFile(const std::string& fileName, const std::string& directory) {
    std::ifstream file(directory + '/' + fileName);
    std::string data;
    
    if (!file.is_open()) {
        std::cerr << 'Failed opening file: ' << fileName << std::endl;
        return "";
    }

    std::getline(file, data, '\0'); // Ler todo o conteúdo do arquivo
    file.close();

    if (data.empty()) {
        std::cerr << 'Failed reading file: ' << fileName << std::endl;
        return "";
    }
    
    return data;
}

void FileManager::createUserDirectory(const std::string& userId) {
    std::string UserDir = std::string(USER_DIR) + '/' + userId;
    std::filesystem::create_directory(UserDir);

    
    std::filesystem::create_directory(UserDir + '/HOSTED');
    std::filesystem::create_directory(UserDir + '/BIDDED');

}

void FileManager::removeUserDirectory(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR) + '/' + userId);
}

void FileManager:: createUserPassFile(const std::string& userId, const std::string& password) {
    std::ofstream file(USER_DIR + '/' + userId + '_pass.txt');
    file << password;
    file.close();
}

void FileManager:: removeUserPassFile(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR)  + userId + '_pass.txt');
}

void FileManager:: createUserLoginFile(const std::string& userId, const std::string& login) {
    std::ofstream file(USER_DIR + '/' + userId + '_login.txt');
    file << login;
    file.close();
}

void FileManager:: removeUserLoginFile(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR)  + userId + '_login.txt');
}

void FileManager::removeUserFiles(const std::string& userId) {
    std::filesystem::remove(std::string(USER_DIR)  + userId + '_pass.txt');
    std::filesystem::remove(std::string(USER_DIR)  + userId + '_login.txt');
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
    std::ofstream file(AUCTION_DIR + '/' + auctionId + '/asset fname');
    file << assetData;
    file.close();
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
    std::filesystem::create_directory(AUCTION_DIR + '/' + auctionId + '/BIDS');
}

void FileManager::createBidFile(const std::string& auctionId, const std::string& bidValue) {
    std::ofstream file(AUCTION_DIR + '/' + auctionId + "/BIDS/(" + bidValue + ").txt");
    file.close();
}

/* Looks for UserId in Users Dir and if it exists, the userId is returned */
uint32_t FileManager::findUser(const std::string& userId) {
    if (std::filesystem::exists(USER_DIR + '/' + userId)) {
        //turn string into uint32_t
        uint32_t id = std::stoul(userId);
        return id;
    }
}

std:: string FileManager::findUserPass(const std::string& userId) {
    if (std::filesystem::exists(USER_DIR + '/' + userId + "_pass.txt")) {
        return readFromFile(userId + "_pass.txt", USER_DIR);
    }
}
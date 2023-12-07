#include "file_manager.hpp"

bool FileManager::writeToFile(const std::string& fileName, const std::string& data, const std::string& directory) {
    std::filesystem::path filePath = BASE_DIR;
    filePath /= directory;
    filePath /= fileName;

    std::ofstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Failed opening file: " << fileName << std::endl;
        return false;
    }

    file << data;
    file.close();

    return true;
}

std::string FileManager::readFromFile(const std::string& fileName, const std::string& directory) {
    std::filesystem::path filePath = directory;
    filePath /= fileName;

    std::ifstream file(filePath);
    std::string data;

    if (!file.is_open()) {
        std::cerr << "Failed opening file: " << fileName << std::endl;
        return "";
    }

    std::getline(file, data, '\0'); // Ler todo o conteúdo do arquivo
    file.close();

    if (data.empty()) {
        std::cerr << "Failed reading file: " << fileName << std::endl;
        return "";
    }

    return data;
}

void FileManager::createUserDirectory(const std::string& userId) {
    std::filesystem::path userDir = USER_DIR;
    userDir /= userId;

    std::filesystem::create_directory(userDir);

    std::filesystem::create_directory(userDir / "HOSTED");
    std::filesystem::create_directory(userDir / "BIDDED");
}

void FileManager::removeUserDirectory(const std::string& userId) {
    std::filesystem::remove(USER_DIR / userId);
}

void FileManager::createUserPassFile(const std::string& userId, const std::string& password) {
    std::filesystem::path passFilePath = USER_DIR;
    passFilePath /= userId + "_pass.txt";

    std::ofstream file(passFilePath);
    file << password;
    file.close();
}

void FileManager::removeUserPassFile(const std::string& userId) {
    std::filesystem::remove(USER_DIR / (userId + "_pass.txt"));
}

void FileManager::createUserLoginFile(const std::string& userId, const std::string& login) {
    std::filesystem::path loginFilePath = USER_DIR;
    loginFilePath /= userId + "_login.txt";

    std::ofstream file(loginFilePath);
    file << login;
    file.close();
}

void FileManager::removeUserLoginFile(const std::string& userId) {
    std::filesystem::remove(USER_DIR / (userId + "_login.txt"));
}

void FileManager::removeUserFiles(const std::string& userId) {
    std::filesystem::remove(USER_DIR / (userId + "_pass.txt"));
    std::filesystem::remove(USER_DIR / (userId + "_login.txt"));
}

void FileManager::createAuctionDirectory(const std::string& auctionId) {
    std::filesystem::create_directory(AUCTION_DIR / auctionId);
}

void FileManager::createUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory) {
    std::filesystem::path auctionFilePath = USER_DIR;
    auctionFilePath /= userId;
    auctionFilePath /= directory;
    auctionFilePath /= auctionId;

    std::ofstream file(auctionFilePath);
    file.close();
}

void FileManager::removeUserAuctionFile(const std::string& userId, const std::string& auctionId, const std::string& directory) {
    std::filesystem::remove(USER_DIR / userId / directory / auctionId);
}

void FileManager::createAuctionStartFile(const std::string& auctionId, const AuctionData& data) {
    std::filesystem::path startFilePath = AUCTION_DIR;
    startFilePath /= auctionId;
    startFilePath /= "START (" + auctionId + ").txt";

    if (!writeToFile(startFilePath.string(), data.toString(), "")) {
        std::cerr << "Unable to create START file for auction: " << auctionId << std::endl;
    }
}

void FileManager::createAuctionAssetFile(const std::string& auctionId, const std::string& assetData) {
    std::filesystem::path assetFilePath = AUCTION_DIR;
    assetFilePath /= auctionId;
    assetFilePath /= "asset fname";

    std::ofstream file(assetFilePath);
    file << assetData;
    file.close();
}

void FileManager::createAuctionEndFile(const std::string& auctionId, const std::time_t& endTime, const int& activeSeconds) {
    std::stringstream ss;
    ss << std::put_time(std::localtime(&endTime), "%Y-%m-%d %H:%M:%S") << " ";
    ss << activeSeconds;

    std::filesystem::path endFilePath = AUCTION_DIR;
    endFilePath /= auctionId;
    endFilePath /= "END (" + auctionId + ").txt";

    if (!writeToFile(endFilePath.string(), ss.str(), "")) {
        std::cerr << "Unable to create END file for auction: " << auctionId << std::endl;
    }
}

void FileManager::createBidsDirectory(const std::string& auctionId) {
    std::filesystem::create_directory(AUCTION_DIR / auctionId / "BIDS");
}

void FileManager::createBidFile(const std::string& auctionId, const std::string& bidValue) {
    std::filesystem::path bidFilePath = AUCTION_DIR;
    bidFilePath /= auctionId;
    bidFilePath /= "BIDS";
    bidFilePath /= "(" + bidValue + ").txt";

    std::ofstream file(bidFilePath);
    file.close();
}

uint32_t FileManager::findUser(const std::string& userId) {
    if (std::filesystem::exists(USER_DIR / userId)) {
        //turn string into uint32_t
        uint32_t id = std::stoul(userId);
        return id;
    }
}

std::string FileManager::findUserPass(const std::string& userId) {
    if (std::filesystem::exists(USER_DIR / (userId + "_pass.txt"))) {
        return readFromFile(userId + "_pass.txt", USER_DIR);
    }
}

#include "file_manager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>

#include "Server/auction_data.hpp"
#include "common/constants.hpp"


bool FileManager::writeToFile(const std::string& fileName, const std::string& data, const std::string& directory) {
    
    std::ofstream file(BASE_DIR + directory + "/" + fileName);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return false;
    }

    file << data;
    file.close();
    
    return true;
}

std::string FileManager::readFromFile(const std::string& fileName, const std::string& directory) {
    std::ifstream file(directory + "/" + fileName);
    std::string data;
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return "";
    }

    std::getline(file, data, '\0'); // Ler todo o conteúdo do arquivo
    file.close();
    
    return data;
}

/*Function to load auction to server from file*/
void FileManager::loadAuction(const std::string& fileName, std::vector<AuctionData>& auctions) {
    std::string data = readFromFile(fileName, AUCTION_DIR);
    if (data.empty()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return;
    }

    std::string line;
    std::string itemName;
    double initialBid;
    int durationSeconds;
    int id;

    std::stringstream ss(data);
    while (std::getline(ss, line)) {
        std::stringstream ss2(line);
        ss2 >> id >> itemName >> initialBid >> durationSeconds;
        auctions.push_back(AuctionData(id, itemName, initialBid, durationSeconds));
    }
}

/*Function to save auction to file*/
void FileManager::saveAuction(const std::string& fileName, const std::vector<AuctionData>& auctions) {
    std::stringstream ss;
    for (const auto& auction : auctions) {
        ss << auction.getId() << " " << auction.getItemName() << " " << auction.getInitialBid() << " " << auction.getDurationSeconds() << std::endl;
    }
    writeToFile(fileName, ss.str(), AUCTION_DIR);
}

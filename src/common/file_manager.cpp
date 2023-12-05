#include "file_manager.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>


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

bool FileManager::readFromFile(const std::string& fileName, std::string& data, const std::string& directory) {
    std::ifstream file(directory + "/" + fileName);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return false;
    }

    std::getline(file, data, '\0'); // Ler todo o conteúdo do arquivo
    file.close();
    
    return true;
}


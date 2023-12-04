#include "file_manager.hpp"
#include <iostream>
#include <fstream>

FileManager::FileManager(const std::string& baseDir) : baseDirectory(baseDir) {}

bool FileManager::writeToFile(const std::string& fileName, const std::string& data) {
    std::ofstream file(baseDirectory + "/" + fileName);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return false;
    }

    file << data;
    file.close();
    
    return true;
}

bool FileManager::readFromFile(const std::string& fileName, std::string& data) {
    std::ifstream file(baseDirectory + "/" + fileName);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << fileName << std::endl;
        return false;
    }

    std::getline(file, data, '\0'); // Ler todo o conteúdo do arquivo
    file.close();
    
    return true;
}

// Implemente outros métodos conforme necessário

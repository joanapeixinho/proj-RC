// exceptions.hpp
#include <stdexcept>
#include <string>

class FileOpenException : public std::runtime_error {
public:
    FileOpenException(const std::string& filename)
        : std::runtime_error("Failed to open file: " + filename) {}
};

class FileWriteException : public std::runtime_error {
public:
    FileWriteException(const std::string& filename)
        : std::runtime_error("Failed to write to file: " + filename) {}
};

class FileReadException : public std::runtime_error {
public:
    FileReadException(const std::string& filename)
        : std::runtime_error("Failed to read from file: " + filename) {}
};



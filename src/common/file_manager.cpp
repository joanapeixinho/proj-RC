#include "file_manager.hpp"

FileManager::FileManager()
{
    std::filesystem::create_directory(BASE_DIR);
    std::string UserDir = std::string(BASE_DIR) + std::string("/") + USER_DIR;
    std::filesystem::create_directory(UserDir);
    std::string AuctionDir = std::string(BASE_DIR) + std::string("/") + AUCTION_DIR;
    std::filesystem::create_directory(AuctionDir);
}

bool FileManager::writeToFile(const std::string &filename, const std::string &data, const std::string &directory)
{

    std::ofstream file(std::string(BASE_DIR) + directory + std::string("/") + filename);

    if (!file.is_open())
    {
        throw FileOpenException(filename);
        return false;
    }

    file << data;

    if (!file.good())
    {
        throw FileWriteException(filename);
        return false;
    }

    file.close();

    return true;
}

std::string FileManager::readFromFile(const std::string &filename, const std::string &directory)
{
    std::ifstream file(std::string(BASE_DIR) + directory + std::string("/") + filename);
    std::string data;

    if (!file.is_open())
    {
        throw FileOpenException(filename);
        return "";
    }

    std::getline(file, data, '\0');
    file.close();

    if (data.empty())
    {
        throw FileReadException(filename);
        return "";
    }

    return data;
}

void FileManager::safeLockUser(const std::string &userId, std::function<void()> func)
{
    try
    {
        std::lock_guard<std::mutex> lock(userMutexes[userId]);
        func();
    }
    catch (const std::exception &e)
    {
        std::cerr << "An exception occurred: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "An unknown exception occurred.\n";
    }
}

void FileManager::safeLockAuction(const std::string &auctionId, std::function<void()> func)
{
    try
    {
        std::lock_guard<std::mutex> lock(auctionMutexes[auctionId]);
        func();
    }
    catch (const std::exception &e)
    {
        std::cerr << "An exception occurred: " << e.what() << '\n';
    }
    catch (...)
    {
        std::cerr << "An unknown exception occurred.\n";
    }
}

void FileManager::createUserDirectory(const std::string &userId)
{
    std::string UserDir = std::string(BASE_DIR) + "/" + std::string(USER_DIR) + "/" + userId;
    std::filesystem::create_directory(UserDir);
    std::filesystem::create_directory(UserDir + "/HOSTED");
    std::filesystem::create_directory(UserDir + "/BIDDED");
}

void FileManager::createUserPassFile(const std::string &userId, const std::string &password)
{
    std::ofstream file(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "_pass.txt");
    file << password;
    if (!file.good())
    {
        throw FileWriteException(userId + "_pass.txt");
    }
    file.close();
}

void FileManager::createUserLoginFile(const std::string &userId)
{
    std::ofstream file(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "_login.txt");
    file.close();
}

void FileManager::removeUserLoginFile(const std::string &userId)
{
    std::filesystem::remove(std::string(BASE_DIR) + "/" + std::string(USER_DIR) + userId + "_login.txt");
}

void FileManager::removeUserFiles(const std::string &userId)
{
    std::filesystem::remove(std::string(BASE_DIR) + "/" + std::string(USER_DIR) + userId + "_pass.txt");
    std::filesystem::remove(std::string(BASE_DIR) + "/" + std::string(USER_DIR) + userId + "_login.txt");
}

void FileManager::createAuctionDirectory(const std::string &auctionId)
{
    std::filesystem::create_directory(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId);
}

void FileManager::createUserAuctionFile(const std::string &userId, const std::string &auctionId, const std::string &directory)
{
    std::ofstream file(std::string(BASE_DIR) + "/" + USER_DIR + std::string("/") + userId + std::string("/") + directory + std::string("/") + auctionId);
    file.close();
}

void FileManager::removeUserAuctionFile(const std::string &userId, const std::string &auctionId, const std::string &directory)
{
    std::filesystem::remove(std::string(BASE_DIR) + "/" + USER_DIR + std::string("/") + userId + std::string("/") + directory + std::string("/") + auctionId);
}

void FileManager::createAuctionStartFile(const std::string &auctionId, const AuctionData &data)
{
    if (!writeToFile("START (" + auctionId + ").txt", data.toString(), std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId))
    {
        std::cerr << "Unable to create START file for auction: " << auctionId << std::endl;
    }
}

void FileManager::createAuctionAssetFile(const std::string &auctionId, const std::string &assetFname)
{
    /* The file was created when we read the socket */
    std::filesystem::path destination_filePath = std::filesystem::path(BASE_DIR) / AUCTION_DIR / auctionId / assetFname;
    try
    {
        // Check if the source file exists and is a regular file
        if (std::filesystem::exists(assetFname) && std::filesystem::is_regular_file(assetFname))
        {
            // Move the file to the destination directory
            std::filesystem::rename(assetFname, destination_filePath);
            std::cout << "File moved successfully." << std::endl;
        }
        else
        {
            std::cerr << "Source file does not exist or is not a regular file." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void FileManager::createAuctionEndFile(const std::string &auctionId, const std::time_t &endTime, const uint32_t &activeSeconds)
{
    std::stringstream ss;
    ss << std::put_time(std::localtime(&endTime), "%Y-%m-%d %H:%M:%S") << " ";
    ss << activeSeconds;
    if (!writeToFile("END (" + auctionId + ").txt", ss.str(), std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId))
    {
        throw FileWriteException("END (" + auctionId + ").txt");
    }
}

void FileManager::createBidsDirectory(const std::string &auctionId)
{
    std::filesystem::create_directory(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS");
}

void FileManager::createBidFile(const std::string &auctionId, const std::string &bidValue)
{
    std::ofstream file(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS/(" + bidValue + ").txt");
    file.close();
}

std::string FileManager::getUserPassword(const std::string &userId)
{
    if (UserRegistered(userId))
    {
        return readFromFile(userId + "_pass.txt", std::string(BASE_DIR) + "/" + USER_DIR);
    }
    throw UserNotRegisteredException(userId);
    return "";
}

bool FileManager::UserLoggedIn(const std::string &userId)
{
    return std::filesystem::exists(std::string(BASE_DIR) + "/" + USER_DIR + std::string("/") + userId + "_login.txt");
}

bool FileManager::UserRegistered(const std::string &userId)
{
    return std::filesystem::exists(std::string(BASE_DIR) + "/" + USER_DIR + std::string("/") + userId + "_pass.txt");
}

/*Returns True if END file exists and therefore auction is active*/
bool FileManager::auctionIsActive(const std::string &auctionId)
{

    return !std::filesystem::exists(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/END (" + auctionId + ").txt");
}

void FileManager::loginUser(const std::string &userId)
{
    safeLockUser(userId, [&]()
                 { createUserLoginFile(userId); });
}

void FileManager::logoutUser(const std::string &userId)
{
    safeLockUser(userId, [&]()
                 { removeUserLoginFile(userId); });
}

void FileManager::registerUser(const std::string &userId, const std::string &password)
{

    safeLockUser(userId, [&]()
                 { createUserDirectory(userId); });

    safeLockUser(userId, [&]()
                 { createUserPassFile(userId, password); });
}

void FileManager::unregisterUser(const std::string &userId)
{
    safeLockUser(userId, [&]()
                 { removeUserFiles(userId); });
}

std::vector<std::pair<uint32_t, bool>> FileManager::getUserAuctions(const std::string &userId, const std::string &directory)
{
    std::vector<std::pair<uint32_t, bool>> auctionList;
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + USER_DIR + std::string("/") + userId + std::string("/") + directory))
    {
        std::string auctionId = entry.path().filename().string();

        safeLockAuction(auctionId, [&]()
                        {
            // update auction
            UpdateAuction(auctionId);

            // check if auction is active
            bool isActive = auctionIsActive(auctionId);
            uint32_t intAuctionId = static_cast<uint32_t>(std::stoi(auctionId));

            // add to list
            auctionList.push_back(std::make_pair(intAuctionId, isActive)); });
    }

    return auctionList;
}

std::vector<std::pair<uint32_t, bool>> FileManager::getAllAuctions()
{
    std::vector<std::pair<uint32_t, bool>> auctionList;
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + AUCTION_DIR))
    {
        if (std::filesystem::is_directory(entry.status()))
        {
            // update auction
            std::string auctionId = entry.path().filename().string();
            safeLockAuction(auctionId, [&]()
                            {
                        
                    UpdateAuction(auctionId);
                    bool isActive = auctionIsActive(auctionId);
                    uint32_t intAuctionId = static_cast<uint32_t>(std::stoi(auctionId));

                auctionList.push_back(std::make_pair(intAuctionId, isActive)); });
        }
    }

    if (auctionList.empty())
    {
        throw NoAuctionsException();
    }

    return auctionList;
}

AuctionData FileManager::getAuction(const std::string &auctionId)
{
    AuctionData data;

    safeLockAuction(auctionId, [&]()
                    {
        //check if start file exists
        if (!std::filesystem::exists(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/START (" + auctionId + ").txt")) {
            throw AuctionDoesNotExistException(auctionId);
        }

        //update Auction
        UpdateAuction(auctionId);

        std::string startFile = readFromFile("START (" + auctionId + ").txt", std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId);
        std::stringstream ss(startFile);
        std::string uid, name, assetFname, startValue, timeActive, startDatetime, startFulltime;
        std::getline(ss, uid, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, assetFname, ' ');
        std::getline(ss, startValue, ' ');
        std::getline(ss, timeActive, ' ');
        std::getline(ss, startDatetime, ' ');
        uint32_t initialBid = static_cast<uint32_t>(std::stoul(startValue));
        uint32_t durationSeconds = static_cast<uint32_t>(std::stoul(timeActive));
        std::time_t startTime = static_cast<uint32_t>(std::stoi(startFulltime));

        if (!auctionIsActive(auctionId)) {
            std::string endFile = readFromFile("END (" + auctionId + ").txt", std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId);
            std::stringstream ssEnd(endFile);
            std::string endDatetime, endSecTime;
            std::getline(ssEnd, endDatetime, ' ');
            std::getline(ssEnd, endSecTime, ' ');
            uint32_t endTimeSec = static_cast<uint32_t>(std::stoul(endSecTime));
            std::time_t endTime =(std::stoi(endDatetime));
            std :: vector<Bid> bids = getAuctionBids(auctionId);
            data = AuctionData(static_cast<uint32_t>(std::stoi(auctionId)), static_cast<uint32_t>(std::stoi(uid)), name, initialBid,
                   durationSeconds, assetFname, endTime, endTimeSec, startTime, bids);

            data.setInactive();
        } else {
            std :: vector<Bid> bids = getAuctionBids(auctionId);
            data = AuctionData(static_cast<uint32_t>(std::stoi(auctionId)),static_cast<uint32_t>(std::stoi(uid)), name, initialBid,
                             durationSeconds, assetFname, 0, 0, startTime, bids);
        } });

    return data;
}

std::vector<Bid> FileManager::getAuctionBids(const std::string &auctionId)
{
    std::vector<Bid> bids;
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS"))
    {
        safeLockAuction(auctionId, [&]()
                        {
        std::string bidValue = entry.path().filename().string();
        std::string bidFile = readFromFile(bidValue, std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS");
        std::stringstream ss(bidFile);
        std::string bidder_user_id, bid_value, date_time, sec_time;
        std::getline(ss, bidder_user_id, ' ');
        std::getline(ss, bid_value, ' ');
        std::getline(ss, date_time, ' ');
        std::getline(ss, sec_time, ' ');
        Bid bid;
        bid.bidder_user_id = static_cast<uint32_t>(std::stoi(bidder_user_id));
        bid.bid_value = static_cast<uint32_t>(std::stoi(bid_value));

        // Convert date_time string to time_t
        std::tm tm = {};
        std::stringstream ss_date_time(date_time);
        ss_date_time >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        bid.date_time = std::mktime(&tm);

        bid.sec_time = static_cast<uint32_t>(std::stoi(sec_time));
        bids.push_back(bid); });
    }

    return bids;
}

void FileManager::openAuction(const std::string &userId, const AuctionData &data)
{

    std::string auctionId = data.getIdString();

    safeLockUser(userId, [&]()
                 { createUserAuctionFile(userId, auctionId, "HOSTED"); });
    safeLockAuction(auctionId, [&]()
                    { createAuctionDirectory(auctionId); });
    safeLockAuction(auctionId, [&]()
                    { createAuctionStartFile(auctionId, data); });
    safeLockAuction(auctionId, [&]()
                    { createBidsDirectory(auctionId); });
    safeLockAuction(auctionId, [&]()
                    { createAuctionAssetFile(auctionId, data.getAssetFname()); });
}

/* check START FILE to see if the endtime has passed */
/* if it has, create END FILE */

void FileManager::UpdateAuction(const std::string &auctionId)
{

    if (auctionIsActive(auctionId))
    {
        std::string startFile = readFromFile("START (" + auctionId + ").txt", std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId);
        std::stringstream ss(startFile);
        std::string uid, name, assetFname, startValue, timeActive, startDatetime, startFulltime;
        std::getline(ss, uid, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, assetFname, ' ');
        std::getline(ss, startValue, ' ');
        std::getline(ss, timeActive, ' ');
        std::getline(ss, startDatetime, ' ');
        std::getline(ss, startFulltime, ' ');
        std::time_t startTime = static_cast<uint32_t>(std::stoi(startFulltime));
        uint32_t durationSeconds = static_cast<uint32_t>(std::stoul(timeActive));
        std::time_t endTime = startTime + durationSeconds;
        std::time_t now = std::time(nullptr);
        if (now > endTime)
        {
            createAuctionEndFile(auctionId, endTime, durationSeconds);
        }
    }
}

void FileManager::closeAuction(AuctionData &auction)
{

    safeLockAuction(auction.getIdString(), [&]()
                    {
        if (auctionIsActive(auction.getIdString())) {
            createAuctionEndFile(auction.getIdString(), auction.getEndTime(), auction.getDurationSeconds());
            auction.setInactive();
        }
        else {
            throw AuctionNotActiveException(auction.getIdString());
        } });
}

std::filesystem::path FileManager::showAsset(AuctionData &auction)
{
    std::filesystem::path assetPath;

    safeLockAuction(auction.getIdString(), [&]()
                    {
        // update auction
        UpdateAuction(auction.getIdString());

        assetPath = std::filesystem::path(BASE_DIR) / AUCTION_DIR / auction.getIdString() / auction.getAssetFname();
        if (assetPath.empty())
        {
            throw InvalidFilePathException(assetPath);
        }

        if (std::filesystem::exists(assetPath))
        {
            return assetPath;
        }

        throw AssetDoesNotExistException(assetPath);
        return std::filesystem::path(); });

    return assetPath;
}

void FileManager::bid(AuctionData &auction, uint32_t bidValue)
{
    // update auction
        safeLockAuction(
            auction.getIdString(), [&]() {
        UpdateAuction(auction.getIdString());
        
            if (auctionIsActive(auction.getIdString()))
            {
                std::string bidValueString = std::to_string(bidValue);
                createBidFile(auction.getIdString(), bidValueString);
            }
            else
            {

                throw AuctionNotActiveException(auction.getIdString());
            }
        });

        // ADD TO AUCTION BIDS
        std::string bidValueString = std::to_string(bidValue);
        safeLockAuction(auction.getIdString(), [&]()
                        { createBidFile(auction.getIdString(), bidValueString); });

        // ADD TO USER BIDDED
        std::string userId = std::to_string(bidValue);
        safeLockUser(userId, [&]()
                     { createUserAuctionFile(userId, auction.getIdString(), "BIDDED"); });
}



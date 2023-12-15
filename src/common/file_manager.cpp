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

    std::ofstream file(std::string(BASE_DIR) + std::string("/") + directory + std::string("/") + filename);

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
    std::ifstream file(std::string(BASE_DIR) + std::string("/") + directory + std::string("/") + filename);
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
    std::ofstream file(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_pass.txt");
    file << password;
    if (!file.good())
    {
        throw FileWriteException(userId + "_pass.txt");
    }
    file.close();
}

void FileManager::createUserLoginFile(const std::string &userId)
{
    std::ofstream file(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_login.txt");
    file.close();
}

void FileManager::removeUserLoginFile(const std::string &userId)
{
    std::filesystem::remove(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_login.txt");
}

void FileManager::removeUserFiles(const std::string &userId)
{
    std::filesystem::remove(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_login.txt");
    std::filesystem::remove(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_pass.txt");
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
    if (!writeToFile("START (" + auctionId + ").txt", data.toString(), AUCTION_DIR + std::string("/") + auctionId))
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

void FileManager::createAuctionEndFile(const std::string &auctionId, const std::string &endTime, const uint32_t &activeSeconds)
{
    std::stringstream ss;
    ss << endTime << " " << activeSeconds;
    if (!writeToFile("END (" + auctionId + ").txt", ss.str(), AUCTION_DIR + std::string("/") + auctionId))
    {
        throw FileWriteException("END (" + auctionId + ").txt");
    }
}

void FileManager::createBidsDirectory(const std::string &auctionId)
{
    std::filesystem::create_directory(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS");
}

void FileManager::createBidFile(const std::string &auctionId, const std::string &userId, const std::string &bidValue, std::time_t startTime)
{
    std::string bidFileName = std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS/" + bidValue + ".txt";
    std::ofstream file(bidFileName);

    // get date of now in format YYYY-MM-DD HH:MM:SS
    std::time_t bidDateTime = time(0);

    // calculate the number of seconds elapsed since the start of the auction
    std::time_t bidSecTime = bidDateTime - startTime;

    if (file.is_open())
    {
        file << userId << " " << bidValue << " " << std::put_time(std::gmtime(&bidDateTime), "%Y-%m-%d %H:%M:%S") << " " << bidSecTime;
        file.close();
    }
    else
    {
        throw FileOpenException(bidFileName);
    }
}

std::string FileManager::getUserPassword(const std::string &userId)
{
    if (UserRegistered(userId))
    {
        return readFromFile(userId + "_pass.txt", USER_DIR + std::string("/") + std::string(userId));
    }
    throw UserNotRegisteredException(userId);
    return "";
}

bool FileManager::UserLoggedIn(const std::string &userId)
{
    return std::filesystem::exists(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_login.txt");
}

bool FileManager::UserRegistered(const std::string &userId)
{
    return std::filesystem::exists(std::string(BASE_DIR) + "/" + USER_DIR + "/" + userId + "/" + userId + "_pass.txt");
}

/*Returns True if END file does not exist and therefore auction is active*/
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

    std::sort(auctionList.begin(), auctionList.end());

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

    //sort auctions by auctionID

    std::sort(auctionList.begin(), auctionList.end());

    return auctionList;
}

AuctionData FileManager::getAuction(const uint32_t auctionIdInt)
{
    AuctionData data;

    std::string auctionId = AuctionData::idToString(auctionIdInt);
    // check if start file exists
    if (!std::filesystem::exists(std::string(BASE_DIR) + std::string(AUCTION_DIR) + auctionId + "/START (" + auctionId + ").txt"))
    {
        throw AuctionDoesNotExistException(auctionId);
    }

    safeLockAuction(auctionId, [&]()
                    {
        //update Auction
        UpdateAuction(auctionId);
        std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + std::string("/") + auctionId);
        std::stringstream ss(startFile);
        std::string uid, name, assetFname, startValue, timeActive, startDate, startHour, startFulltime;
        std::getline(ss, uid, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, assetFname, ' ');
        std::getline(ss, startValue, ' ');
        std::getline(ss, timeActive, ' ');
        std::getline(ss, startDate, ' ');
        std::getline(ss, startHour, ' ');
        std::getline(ss, startFulltime, ' ');


        uint32_t initialBid = static_cast<uint32_t>(std::stoul(startValue));
        uint32_t durationSeconds = static_cast<uint32_t>(std::stoul(timeActive));
        uint32_t uidInt = static_cast<uint32_t>(std::stoul(uid));

        std::time_t startTime = static_cast<std::time_t>(std::stoll(startFulltime));

        if (!auctionIsActive(auctionId)) {
        
            std::string endFile = readFromFile("END (" + auctionId + ").txt", AUCTION_DIR + std::string("/") + auctionId);
            std::stringstream ssEnd(endFile);
            std::string endDate, endHour, endSecTime;
            std::getline(ssEnd, endDate, ' ');
            std::getline(ssEnd, endHour, ' ');
            std::getline(ssEnd, endSecTime, ' ');
           
            uint32_t endTimeSec = static_cast<uint32_t>(std::stoul(endSecTime));

          
            std :: vector<Bid> bids = getAuctionBids(auctionId);
            data = AuctionData(auctionIdInt, uidInt, name, initialBid,
                   durationSeconds, assetFname, startTime,endDate + ' ' + endHour, endTimeSec, bids);

            data.setInactive();
        } else {
            std::string endDatetime = " ";
            std :: vector<Bid> bids = getAuctionBids(auctionId);
            data = AuctionData(auctionIdInt,uidInt, name, initialBid,
                             durationSeconds, assetFname, startTime, endDatetime,0, bids);
        } });

    return data;
}

std::vector<Bid> FileManager::getAuctionBids(const std::string &auctionId)
{
    std::vector<Bid> bids;
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + AUCTION_DIR + std::string("/") + auctionId + "/BIDS"))
    {
        std::string bidValue = entry.path().filename().string();
        std::string bidFile = readFromFile(bidValue, AUCTION_DIR + std::string("/") + auctionId + "/BIDS");
        std::stringstream ss(bidFile);
        std::string bidder_user_id, bid_value, date, hours, sec_time;
        std::getline(ss, bidder_user_id, ' ');
        std::getline(ss, bid_value, ' ');
        std::getline(ss, date, ' ');
        std::getline(ss, hours, ' ');
        std::getline(ss, sec_time, ' ');
        Bid bid;
        bid.bidder_user_id = static_cast<uint32_t>(std::stoi(bidder_user_id));
        bid.bid_value = static_cast<uint32_t>(std::stoi(bid_value));
        bid.date_time = date + ' ' + hours;
        bid.sec_time = static_cast<uint32_t>(std::stoi(sec_time));
        bids.push_back(bid);
    }

    // sort bids by bid value

    std::sort(bids.begin(), bids.end(), [](const Bid &a, const Bid &b)
              { return a.bid_value < b.bid_value; });

    return bids;
}

void FileManager::openAuction(const std::string &userId, const AuctionData &data)
{

    // print
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
        std::string startFile = readFromFile("START (" + auctionId + ").txt", AUCTION_DIR + std::string("/") + auctionId);
        std::stringstream ss(startFile);
        std::string uid, name, assetFname, startValue, timeActive, startDate, startHour, startFulltime;
        std::getline(ss, uid, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, assetFname, ' ');
        std::getline(ss, startValue, ' ');
        std::getline(ss, timeActive, ' ');
        std::getline(ss, startDate, ' ');
        std::getline(ss, startHour, ' ');
        std::getline(ss, startFulltime, ' ');

        uint32_t durationSeconds = static_cast<uint32_t>(std::stoul(timeActive));

        std::time_t startTime = static_cast<std::time_t>(std::stoll(startFulltime));

        std::time_t endTime = startTime + durationSeconds;
        std::time_t now = std::time(nullptr);

        if (now > endTime)
        {
            std::ostringstream oss;
            oss << std::put_time(std::gmtime(&endTime), "%Y-%m-%d %H:%M:%S");
            std::string endTimeDate = oss.str();
            createAuctionEndFile(auctionId, endTimeDate, durationSeconds);
        }
    }
}

void FileManager::closeAuction(AuctionData &auction)
{

    std::time_t now = std::time(nullptr);

    // get string in format YYYY-MM-DD HH:MM:SS for now
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&now), "%Y-%m-%d %H:%M:%S");
    std::string endTimeDate = oss.str();

    // calculate duration of auction in seconds in uint32_t

    uint32_t durationSeconds = static_cast<uint32_t>(now - auction.getStartTime());

    safeLockAuction(auction.getIdString(), [&]()
                    {
        if (auctionIsActive(auction.getIdString())) {
            createAuctionEndFile(auction.getIdString(), endTimeDate, durationSeconds);
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

void FileManager::bid(AuctionData &auction, uint32_t bidValue, const std::string &userId)
{

    if (auctionIsActive(auction.getIdString()))
    {
        // ADD TO AUCTION BIDS
        std::string bidValueString = std::to_string(bidValue);

        safeLockAuction(auction.getIdString(), [&]()
                        { createBidFile(auction.getIdString(), userId, bidValueString, auction.getStartTime()); });

        safeLockUser(userId, [&]()
                     { createUserAuctionFile(userId, auction.getIdString(), "BIDDED"); });
    }
    else
    {

        throw AuctionNotActiveException(auction.getIdString());
    }
}

uint32_t FileManager::getAuctionsCount()
{
    uint32_t count = 1;
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + AUCTION_DIR))
    {
        if (std::filesystem::is_directory(entry.status()))
        {
            count++;
        }
    }
    return count;
}

void FileManager::shutdown()
{
    // logout all users
    for (const auto &entry : std::filesystem::directory_iterator(std::string(BASE_DIR) + "/" + USER_DIR))
    {
        if (std::filesystem::is_directory(entry.status()))
        {
            std::string userId = entry.path().filename().string();
            logoutUser(userId);
        }
    }
}
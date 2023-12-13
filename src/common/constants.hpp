#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT "58037" // 58000 + GN (Our GroupNumber is 37)

#define UDP_TIMEOUT_SECONDS (4)
#define UDP_RESEND_TRIES (3)
#define TCP_READ_TIMEOUT_SECONDS (15)
#define TCP_WRITE_TIMEOUT_SECONDS (20 * 60)  // 20 minutes
#define SERVER_RECV_RESTART_TIMEOUT_SECONDS (3)

#define SOCKET_BUFFER_LEN (256)
#define PACKET_ID_LEN (3)

#define USER_ID_STR_LEN (6)
#define USER_ID_MAX ((uint32_t)pow(10, USER_ID_STR_LEN) - 1)
#define PASSWORD_LEN (8)
#define NO_USER_ID 1000000

#define AUCTION_NAME_MAX_LENGTH (10)
#define BID_MAX_VALUE (999999)
#define AUCTION_START_VALUE_MAX_LEN (6)
#define AUCTION_DURATION_MAX_VALUE (99999)
#define AUCTION_ID_MAX_LEN (3)
#define AUCTION_MAX_NUMBER (999)

#define ASSET_NAME_MAX_LENGTH (24)
#define ASSET_FILE_SIZE_MAX_LEN (8)
#define ASSET_MAX_BYTES (1000000)

#define BID_MAX_LEN (6)
#define BID_MAX_VALUE (999999)

#define ASSETS_RELATIVE_DIRERCTORY "./../../ASSETS/"

#define EXCEPTION_RETRY_MAX (3)

#define FILE_BUFFER_LEN (512)

#define PROGRESS_BAR_STEP_SIZE (10)

#define BASE_DIR "ASDIR/"
#define AUCTION_DIR "AUCTIONS/"
#define USER_DIR "USERS/"


#define HELP_MENU_COMMAND_COLUMN_WIDTH (24)
#define HELP_MENU_DESCRIPTION_COLUMN_WIDTH (35)
#define HELP_MENU_ALIAS_COLUMN_WIDTH (40)

#define TCP_WORKER_POOL_SIZE (50)
#define TCP_MAX_QUEUE_SIZE (5)

#endif
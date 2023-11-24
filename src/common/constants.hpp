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

#define USER_ID_MAX_LEN (6)
#define USER_ID_MAX ((uint32_t)pow(10, USER_ID_MAX_LEN) - 1)

#define EXCEPTION_RETRY_MAX (3)

#define FILE_BUFFER_LEN (512)
/*
#define PROGRESS_BAR_STEP_SIZE (10)

#define WORD_MIN_LEN (3)
#define WORD_MAX_LEN (30)
#define TRIAL_MIN (1)
#define TRIAL_MAX (99)

#define GAMEDATA_FOLDER_NAME ".gamedata"

#define SCOREBOARD_MAX_ENTRIES (10)
#define SCOREBOARD_FILE_NAME "scoreboard.dat"

#define GAMES_FOLDER_NAME "games"
*/

#define HELP_MENU_COMMAND_COLUMN_WIDTH (20)
#define HELP_MENU_DESCRIPTION_COLUMN_WIDTH (40)
#define HELP_MENU_ALIAS_COLUMN_WIDTH (40)

#define TCP_WORKER_POOL_SIZE (50)
#define TCP_MAX_QUEUE_SIZE (5)

#endif
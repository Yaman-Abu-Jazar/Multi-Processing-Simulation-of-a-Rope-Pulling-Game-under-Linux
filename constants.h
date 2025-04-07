
#ifndef __CONSTANTS__
#define __CONSTANTS__
#define _XOPEN_SOURCE 500

static int NUMBER_OF_PLAYERS = 0;
static int ENERGY_MIN = 0;
static int ENERGY_MAX = 0;
static int TEAM_SIZE = 0;
static int EFFORT_MAX = 0;
static int EFFORT_MIN = 0;
static char PUBLIC_FIFO[50] = "";
static int EFFORT_THRESHOLD = 0;
static int ACCIDENT_TIME_MAX = 0;
static int ACCIDENT_TIME_MIN = 0;
static int NUMBER_OF_ROUNDS = 0;
static int MAX_TIME = 0;
static int MAX_ROUNDS = 0;

typedef struct {
    int player_id;
    int energy;
    int effort;
    int position;
    pid_t pid;
} PlayerInfo;

typedef enum {
    NO_ONE = 0,
    TEAM_1 = 1,
    TEAM_2 = 2
} Winner;

typedef enum {
    READY = 0,
    START = 1,
    ENERGY_REQUEST = 2,
    MOVE_TEAM1 = 3,
    MOVE_TEAM2 = 4,
    TEAM1_WIN = 5,
    TEAM2_WIN = 6
} Message;

#endif
#include "header.h"
#include "constants.h"
#include "functions.h"

void user_handler1(int sig);
void user_handler2(int sig);
void user_handler3(int sig);

int id;
int pipes_1[2];
PlayerInfo info;
int falling_flag;
int time_to_rejoin;
int multiplied_energy;
int chance = 10;
int time1 = 0;

int main(int argc, char **argv){

    if(argc < 2){
        perror("Error");
        exit(-5);
    }

    // Read the constants 
    readFile("arguments.txt");
    // Seed the random number generator with the current time
    srand(time(NULL) ^ getpid());

    // generate a random energy for the player
    int energy = (int) (ENERGY_MIN + (rand() % (ENERGY_MAX - ENERGY_MIN)));
    // generate a random effort for the player
    int effort = (int) (EFFORT_MIN + (rand() % (EFFORT_MAX - EFFORT_MIN)));
    // random time required to rejoin after falling (1-3) sec
    time_to_rejoin = (int) (ACCIDENT_TIME_MIN + (rand() % (ACCIDENT_TIME_MAX - ACCIDENT_TIME_MIN)));
    time1 = time_to_rejoin;

    id = atoi(argv[1]);

    pipes_1[0] = atoi(argv[2]);
    pipes_1[1] = atoi(argv[3]);
    
    //close(pipes_1[0]);

    info.player_id = id;
    info.energy = energy;
    info.effort = effort;
    info.position = 0;
    info.pid = getpid();
    
    if(write(pipes_1[1], &info, sizeof(info)) == -1){
        perror("Write Pipe Error.\n");
        exit(-7);
    }

    usleep(500000);

    if(read(pipes_1[0], &info, sizeof(PlayerInfo)) == -1){
        perror("Read Position Error.\n");
        exit(-8);
    }

    if(sigset(SIGUSR2, user_handler1) == -1){
        perror("Signal Handler 2 Error.\n");
        exit(-6);
    }

    if(sigset(SIGUSR1, user_handler2) == -1){
        perror("Signal Handler 1 Error.\n");
        exit(-6);
    }

    if(sigset(SIGINT, user_handler3) == -1){
        perror("Signal Handler 3 Error.\n");
        exit(-6);
    }

    pause();

    return 0;
}

void user_handler3(int sig){
    close(pipes_1[0]);
    close(pipes_1[1]);
    exit(-12);
}

void user_handler1(int sig){
    printf("player %d in team 1 is ready in his position %d\n", info.player_id, info.position);
    sleep(2);
}

void user_handler2(int sig){
    // printf("Signal %d has reached.\n", sig);
    if(falling_flag == 1){
        time1--;
        if(time1 == 0){
            falling_flag = 0;
            time1 = time_to_rejoin;
            chance = 5;
            printf("Player %d in team 1 has came back.\n", info.player_id);
        }
    }
    // Generate a random number between 0 and 99
    if ((rand() % 101) < chance) {
        printf("Player %d in team 1 has fallen PID = %d  He will come back after %d seconds.\n",info.player_id, info.pid, time_to_rejoin);
        chance = 0;
        falling_flag = 1;
        multiplied_energy = 0;
        
    } else if(chance != 0){
        multiplied_energy = info.energy * info.position;
        info.energy -= info.effort;
        if(info.energy <= 0){
            info.energy = 1;
        }
    }
    if(write(pipes_1[1], &multiplied_energy, sizeof(int)) == -1){
        perror("Write Pipe Error.\n");
        exit(-7);
    }
    sleep(2);
    
}
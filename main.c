
#include "header.h"
#include "constants.h"
#include "functions.h"

pid_t drawer;
char msg[BUFSIZ];

int main(int argc, char **argv){

    // Determine the id of the referee (parent process)
    pid_t referee_id = getpid();
    // Read the arguments
    readFile("arguments.txt");

    int pipe_drawer[2];
    pipe(pipe_drawer);

    /* Create the drawer */

    int pipes_1[4][2]; // 4 players → 4 pipes for team 1
    int pipes_2[4][2]; // 4 players → 4 pipes for team 2

    pid_t team_1[TEAM_SIZE];
    pid_t team_2[TEAM_SIZE];

    PlayerInfo team1[TEAM_SIZE], team2[TEAM_SIZE];

    int positions_1[TEAM_SIZE], positions_2[TEAM_SIZE];

    int team_1_sum = 0, team_2_sum = 0, t_effort1 = 0, t_effort2 = 0;
    int energies_1[TEAM_SIZE], energies_2[TEAM_SIZE];

    // flags to manage the round
    int round = 0, win = 0, subtraction = 0;
    int round_count, round_team1 = 0, round_team2 = 0;
    int two_rounds_flag = 0, time_flag = 0, effort_flag = 0;

    time_t start, now;

    int fd;

    // Create pipes
    for (int i = 0; i < 4; i++){
        pipe(pipes_1[i]);
        pipe(pipes_2[i]);
    }

    // Create the processes (players) of team 1
    for (int i = 0; i < TEAM_SIZE; i++){
        team_1[i] = fork();

        if (team_1[i] == -1){
            perror("FORK ERROR.\n");
            exit(-1);
        }else if (team_1[i] == 0){
            // each player go to its class in team 1
            char j[12], k[12], l[12];
            sprintf(j, "%d", i);
            sprintf(k, "%d", pipes_1[i][0]);
            sprintf(l, "%d", pipes_1[i][1]);
            execlp("./team1", "team1", j, k, l, NULL);
            perror("Exec Error.\n");
            exit(-2);
        }else{
            // close(pipes_1[i][1]);
        }
    }

    // Create the processes (players) of team 2
    for (int i = 0; i < TEAM_SIZE; i++){
        team_2[i] = fork();

        if (team_2[i] == -1){
            perror("FORK ERROR.\n");
            exit(-1);
        }else if (team_2[i] == 0){
            // each player go to its class in team 2
            char j[12], k[12], l[12];
            sprintf(j, "%d", i);
            sprintf(k, "%d", pipes_2[i][0]);
            sprintf(l, "%d", pipes_2[i][1]);
            execlp("./team2", "team2", j, k, l, NULL);
            perror("Exec Error.\n");
            exit(-2);
        }else{
            // just continue
        }
    }


    for (int i = 0; i < TEAM_SIZE; i++){
        read(pipes_1[i][0], &team1[i], sizeof(PlayerInfo));
    }

    for (int i = 0; i < TEAM_SIZE; i++){
        read(pipes_2[i][0], &team2[i], sizeof(PlayerInfo));
    }

    qsort(team1, 4, sizeof(PlayerInfo), compare_energy);
    qsort(team2, 4, sizeof(PlayerInfo), compare_energy);


    for (int i = 0; i < TEAM_SIZE; i++){
        team1[i].position = i + 1;
    }

    for (int i = 0; i < TEAM_SIZE; i++){
        team2[i].position = i + 1;
    }

    printf("Team 1 :\n");

    for (int i = 0; i < TEAM_SIZE; i++){
        printf("Player ID = %d\t Energy = %d\tPosition = %d\tEffort = %d\tPID = %d\n", team1[i].player_id, team1[i].energy, team1[i].position, team1[i].effort, team1[i].pid);
    }

    printf("Team 2 :\n");

    for (int i = 0; i < TEAM_SIZE; i++)
    {
        printf("Player ID = %d\t Energy = %d\tPosition = %d\tEffort = %d\tPID = %d\n", team2[i].player_id, team2[i].energy, team2[i].position, team2[i].effort, team2[i].pid);
    }

    for (int i = 0; i < TEAM_SIZE; i++)
    {
        write(pipes_1[i][1], &team1[i], sizeof(PlayerInfo));
    }

    for (int i = 0; i < TEAM_SIZE; i++)
    {
        write(pipes_2[i][1], &team2[i], sizeof(PlayerInfo));
    }

    sleep(1);

    
    Winner winner = NO_ONE;
   

    if((drawer = fork()) == -1){
        perror("The drawer fork error\n");
        exit(-1);
    }
    if(!drawer){
        char k[12], l[12];
        sprintf(k, "%d", pipe_drawer[0]);
        sprintf(l, "%d", pipe_drawer[1]);
        execlp("./drawer", "drawer", k, l, (char *) NULL);

        // If the program not have enough memory then will raise error
        perror("exec Failure\n");
        exit(-1);
    }
    printf("Drawer PID : %d\n\n", drawer);
    strcpy(msg, "start");
    write(pipe_drawer[1], &msg, sizeof(msg));
    time(&start);
   // /starting the Game
    for (round_count = 1 ; round_count <= NUMBER_OF_ROUNDS ; round_count++){
        printf("\nRound %d.\n", round_count);

        printf("\nGet Ready.\n\n");
        strcpy(msg, "Get Ready");
        
        write(pipe_drawer[1], &msg, sizeof(msg));
        

        for (int i = 0; i < TEAM_SIZE; i++){
            kill(team_1[i], SIGUSR2);
            kill(team_2[i], SIGUSR2);
        }

        sleep(1);

        printf("\nStart.\n\n");
        
        round = 1;

        for (int i = 0; i < TEAM_SIZE; i++){
            kill(team_1[i], SIGUSR1);
            kill(team_2[i], SIGUSR1);
        }
        t_effort1 = 0;
        t_effort2 = 0;
        win = 0;
        while (round){
            sleep(1);

            for (int i = 0; i < TEAM_SIZE; i++){
                if (read(pipes_1[i][0], &energies_1[i], sizeof(int)) == -1){
                    perror("Read Error.\n");
                    exit(-8);
                } else if(energies_1[i] == 0){
                    sprintf(msg, "Player with id %d in Team 1 has fallen.", i);
                    write(pipe_drawer[1], &msg, sizeof(msg));
                }
            }

            for (int i = 0; i < TEAM_SIZE; i++){
                if (read(pipes_2[i][0], &energies_2[i], sizeof(int)) == -1){
                    perror("Read Error.\n");
                    exit(-8);
                } else if(energies_2[i] == 0){
                    sprintf(msg, "Player with id %d in Team 2 has fallen.", i);
                    write(pipe_drawer[1], &msg, sizeof(msg));
                }
            }

            for (int i = 0; i < TEAM_SIZE; i++){
                team_1_sum += energies_1[i];
                team_2_sum += energies_2[i];
            }

            printf("Team 1 Energies = %d\tTeam 2 Energies = %d\n", team_1_sum, team_2_sum);

            subtraction = team_1_sum - team_2_sum;
            team_1_sum = 0;
            team_2_sum = 0;
            printf("subtraction result : %d\n", subtraction);

            if (subtraction > 0){
                strcpy(msg,"Team1");
                printf("Team 1 is heading towards winning.\n");
                t_effort1 += subtraction;
            } else if (subtraction < 0){
                strcpy(msg,"Team2");
                subtraction = 0 - subtraction;
                t_effort2 += subtraction;
                printf("Team 2 is heading towards winning.\n");
            } else {
                // Draw
            }
            write(pipe_drawer[1], &msg, sizeof(msg));
            time(&now);  // Get the current time

            // Check if the time difference has reached the desired duration
            if (difftime(now, start) >= MAX_TIME) {
                win = 1; 
                time_flag = 1;
                winner = NO_ONE;
            }

            if (t_effort1 >= EFFORT_THRESHOLD || t_effort2 >= EFFORT_THRESHOLD){
                win = 1;
            }

            if (win == 1){
                round = 0;
                break;
            }
            for (int i = 0; i < TEAM_SIZE; i++){
                kill(team_1[i], SIGUSR1);
                kill(team_2[i], SIGUSR1);
            }

            sleep(1);
            printf("\n");
        }

        if (t_effort1 >= EFFORT_THRESHOLD){
            printf("\nTeam 1 wins round %d.\n", round_count);
            strcpy(msg, "win1");
            write(pipe_drawer[1], &msg, sizeof(msg));
            round_team1++;
            if(round_team1 >= MAX_ROUNDS){
                
                break;
            }
            if(winner == TEAM_1){
                printf("\nTeam 1 wins. since they win 2 consecutive rounds\n");
                strcpy(msg, "Team 1 wins");
                write(pipe_drawer[1], &msg, sizeof(msg));
                two_rounds_flag = 1;
                break;
            }
            winner = TEAM_1;
        } else if(t_effort2 >= EFFORT_THRESHOLD){
            printf("\nTeam 2 wins round %d.\n", round_count);
            strcpy(msg, "win2");
            write(pipe_drawer[1], &msg, sizeof(msg));
            round_team2++;
            if(round_team2 >= MAX_ROUNDS){
                break;
            }
            if(winner == TEAM_2){
                printf("\nTeam 2 wins. since they win 2 consecutive rounds\n");
                strcpy(msg, "Team 2 wins");
                write(pipe_drawer[1], &msg, sizeof(msg));
                two_rounds_flag = 1;
                break;
            }
            winner = TEAM_2;
        }
        if(time_flag){
            break;
        }
    }

    printf("\nTeam 1 wins %d round(s).\nTeam 2 wins %d round(s).\n", round_team1, round_team2);

    if(time_flag){
        printf("\nTimed out.\n");
        if(round_team1 > round_team2){
            printf("Team 1 wins.\n");
            strcpy(msg, "Team 1 wins");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = TEAM_1;
        } else if(round_team1 < round_team2) {
            printf("Team 2 wins.\n");
            strcpy(msg, "Team 2 wins");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = TEAM_2;    
        } else {
            printf("\nDraw.\n");
            strcpy(msg, "Draw");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = NO_ONE;
        }
    } else if(!time_flag && !two_rounds_flag){
        if(round_team1 > round_team2){
            printf("Team 1 wins.\n");
            strcpy(msg, "Team 1 wins");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = TEAM_1;
        } else if(round_team1 < round_team2) {
            printf("Team 2 wins.\n");
            strcpy(msg, "Team 2 wins");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = TEAM_2;
        } else {
            printf("\nDraw.\n");
            strcpy(msg, "Draw");
            write(pipe_drawer[1], &msg, sizeof(msg));
            winner = NO_ONE;
        }
    } 

    sleep(10);
    strcpy(msg, "GameOver");
    write(pipe_drawer[1], &msg, sizeof(msg));
    printf("\nThe Game is Over.\n");
    // End other processes and close the pipe ends
    for (int i = 0; i < TEAM_SIZE; i++){
        kill(team_1[i], SIGINT);
        kill(team_2[i], SIGINT);
        close(pipes_1[i][0]);
        close(pipes_1[i][1]);
        close(pipes_2[i][0]);
        close(pipes_2[i][1]);
    }

    close(pipe_drawer[0]);
    close(pipe_drawer[1]);

    return 0;
}
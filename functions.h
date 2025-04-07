


#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"


void readFile(char* filename){
    char line[200];
    char label[50];

    FILE *file;

    file = fopen(filename, "r");

    if (file == NULL){
        perror("The file not exist\n");
        exit(-2);
    }


    char separator[] = "=";

    while(fgets(line, sizeof(line), file) != NULL){

        char *str = strtok(line, separator);
        strncpy(label, str, sizeof(label));
        str = strtok(NULL, separator);


        if (strcmp(label, "NUMBER_OF_PLAYERS") == 0){
            NUMBER_OF_PLAYERS = atoi(str);
        }
        if (strcmp(label, "TEAM_SIZE") == 0){
            TEAM_SIZE = atoi(str);
        }
        if (strcmp(label, "ENERGY_MAX") == 0){
            ENERGY_MAX = atoi(str);
        }
        if (strcmp(label, "ENERGY_MIN") == 0){
            ENERGY_MIN = atoi(str);
        }
        if (strcmp(label, "EFFORT_MIN") == 0){
            EFFORT_MIN = atoi(str);
        }
        if (strcmp(label, "EFFORT_MAX") == 0){
            EFFORT_MAX = atoi(str);
        }
        if (strcmp(label, "PUBLIC_FIFO") == 0){
            strncpy(PUBLIC_FIFO, str, sizeof(PUBLIC_FIFO));
        }
        if (strcmp(label, "EFFORT_THRESHOLD") == 0){
            EFFORT_THRESHOLD = atoi(str);
        }
        if (strcmp(label, "ACCIDENT_TIME_MAX") == 0){
            ACCIDENT_TIME_MAX = atoi(str);
        }
        if (strcmp(label, "NUMBER_OF_ROUNDS") == 0){
            NUMBER_OF_ROUNDS = atoi(str);
        }
        if (strcmp(label, "ACCIDENT_TIME_MIN") == 0){
            ACCIDENT_TIME_MIN = atoi(str);
        }
        if (strcmp(label, "MAX_TIME") == 0){
            MAX_TIME = atoi(str);
        }
        if (strcmp(label, "MAX_ROUNDS") == 0){
            MAX_ROUNDS = atoi(str);
        }

    }

    fclose(file);
}

int compare_energy(const void *a, const void *b) {
    PlayerInfo *pa = (PlayerInfo *)a;
    PlayerInfo *pb = (PlayerInfo *)b;
    return pa->energy - pb->energy;
}


#endif
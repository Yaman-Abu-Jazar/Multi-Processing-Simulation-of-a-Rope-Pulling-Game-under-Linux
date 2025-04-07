#include "header.h"
#include "constants.h"
#include "functions.h"

char msg_r[BUFSIZ];
int pipe_drawer[2];
int round_number = 0;
int score_1 = 0;
int score_2 = 0;
int timer = 0;
time_t start_time = 0;
float position_offset = 0.0f;  // Centered by default
int lifting_rope = 0;
float rope_target_offset = 0.0f;
float rope_y_offset = 0.0f;        // Start on ground
int rope_lifted = 0;               // Flag: has the rope been lifted?
int win1 = 0, win2 = 0, draw = 0;
int fallen = 0;
char fall_message[BUFSIZ];


void display();
void init();
void reshape(int w, int h);
void drawText(float x, float y, const char *text, float r, float g, float b);
void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
void drawCircle(float cx, float cy, float r, int num_segments, float red, float green, float blue);
void drawReferee();
void drawPlayers();
void drawRope();
void readFromPipe(int value);
void cleanUpAndExit();

void startOpenGL() {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rope Pulling Game");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(1000, readFromPipe, 0); // Start reading every second
    init();
    glutMainLoop();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(1, 1, 1, 1);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    char timerText[20];
    sprintf(timerText, "Time Left: %d", timer);
    drawText(340, 500, timerText, 0, 0, 0); // Black color

    drawText(300, 580, "ROPE PULLING GAME", 1, 0, 0);

    char roundText[30];
    sprintf(roundText, "Round: %d", round_number);
    drawText(10, 570, roundText, 0, 0.5, 0); // Dark green

    char scoreText[50];
    sprintf(scoreText, "Team 1 : %d   Team 2 : %d", score_1, score_2);
    drawText(280, 30, scoreText, 0, 0, 0.7); // Dark blue
    drawText(300, 50, "Score of Teams", 0, 0, 0.7); // Dark blue

    if(win1 == 1){
        drawText(310, 80, "🏆 Winner : Team 1", 1, 0, 0); // red
    } else if(win2 == 1){
        drawText(310, 80, "🏆 Winner : Team 2", 1, 0, 0); // red
    } else if(draw == 1){
        drawText(310, 80, "Draw", 1, 0, 0); // red
    }

    if(fallen){
        drawText(250, 100, fall_message, 1, 0, 0);
        fallen--;
    }
    
    if (start_time > 0) {
        time_t now = time(NULL);
        int elapsed = (int)(now - start_time);
        timer = MAX_TIME - elapsed;
        if (timer < 0) timer = 0;
        
    }
    if (timer <= 0 && start_time > 0) {
        printf("⏰ Timer finished!\n");
        start_time = 0; // Stop further updates
        strcpy(timerText, "Timed Out.");
    }

    if (lifting_rope && rope_y_offset < rope_target_offset) {
        rope_y_offset += 5.0f; // Adjust this speed as needed
        if (rope_y_offset >= rope_target_offset) {
            rope_y_offset = rope_target_offset;
            lifting_rope = 0;
        }
    }

    drawText(140, 440, "Team 2", 0, 0, 1); 
    drawText(580, 440, "Team 1", 1, 0, 0); 

    drawRectangle(390, 200, 410, 320, 0, 0, 0);

    drawRope();
    drawReferee();
    drawPlayers();
    
    glFlush();
}

void drawText(float x, float y, const char *text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void drawCircle(float cx, float cy, float r, int num_segments, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawReferee() {
    // Head
    drawCircle(400, 450, 10, 15, 1, 0.8, 0.6);

    // Eyes
    drawCircle(396, 453, 2, 10, 0, 0, 0);
    drawCircle(404, 453, 2, 10, 0, 0, 0);

    // Hat
    drawRectangle(390, 460, 410, 470, 0, 0, 0); 
    drawRectangle(385, 468, 415, 472, 0, 0, 0); 

    // Body (black uniform)
    drawRectangle(390, 390, 410, 450, 0, 0, 0);

    // Arms
    drawRectangle(375, 420, 390, 430, 0, 0, 0);
    drawRectangle(410, 420, 425, 430, 0, 0, 0);

    // Legs
    drawRectangle(395, 370, 400, 390, 0, 0, 0);
    drawRectangle(405, 370, 410, 390, 0, 0, 0);
}

void drawSinglePlayer(float x, float y, float r, float g, float b) {
    // Head
    drawCircle(x, y + 50, 10, 15, 1, 0.8, 0.6);

    // Eyes
    drawCircle(x - 3, y + 53, 2, 10, 0, 0, 0);
    drawCircle(x + 3, y + 53, 2, 10, 0, 0, 0);

    // Body
    drawRectangle(x - 10, y, x + 10, y + 50, r, g, b);

    // Arms
    drawRectangle(x - 20, y + 20, x - 10, y + 25, r, g, b);
    drawRectangle(x + 10, y + 20, x + 20, y + 25, r, g, b);

    // Legs
    drawRectangle(x - 5, y - 20, x, y, r, g, b);
    drawRectangle(x + 5, y - 20, x + 10, y, r, g, b);
}

void drawPlayers() {
    float leftStartX = 140 + position_offset;
    float rightStartX = 540 + position_offset;
    float y = 250;

    // Team 2 (Left - Blue)
    for (int i = 0; i < 4; i++) {
        drawSinglePlayer(leftStartX + i * 40, y, 0, 0, 1);
    }

    // Team 1 (Right - Red)
    for (int i = 0; i < 4; i++) {
        drawSinglePlayer(rightStartX + i * 40, y, 1, 0, 0);
    }
}

void drawRope() {
    float base_y = 225 + rope_y_offset;
    drawRectangle(100 + position_offset, base_y, 700 + position_offset, base_y + 5, 0.6, 0.4, 0.2);
}

int main(int argc, char **argv) {
    readFile("arguments.txt");
    pipe_drawer[0] = atoi(argv[1]);
    pipe_drawer[1] = atoi(argv[2]);
    glutInit(&argc, argv);
    startOpenGL();

    return 0;
}

void readFromPipe(int value) {
    if(read(pipe_drawer[0], msg_r, sizeof(msg_r)) == -1){
        perror("Read msges in Drawer Error.\n");
        exit(-8);
    } else {
        if (strcmp(msg_r, "Get Ready") == 0) {
            round_number++;

            // Reset positions at the beginning of each round
            position_offset = 0.0f;
            rope_y_offset = 0.0f;
            rope_lifted = 0;

            if (!rope_lifted) {
                rope_y_offset = 50.0f;  // Lift rope once
                rope_lifted = 1;        // Mark as lifted so it won't lift again
            }
        } else if(strcmp(msg_r, "start") == 0){
            start_time = time(NULL);
        } else if(strcmp(msg_r, "Team1") == 0){
            if (position_offset < 100) // limit movement
                position_offset += 50; // move right (toward Team1)
        } else if(strcmp(msg_r, "Team2") == 0){
            if (position_offset > -100) // limit movement
                position_offset -= 50; // move left (toward Team2)
        } else if(strcmp(msg_r, "win1") == 0){
            score_1++;
        } else if(strcmp(msg_r, "win2") == 0){
            score_2++;
        } else if(strcmp(msg_r, "Team 1 wins") == 0){
            win1++;
        } else if(strcmp(msg_r, "Team 2 wins") == 0){
            win2++;
        } else if(strcmp(msg_r, "Draw") == 0){
            draw++;
        } else if (strcmp(msg_r, "GameOver") == 0 || strcmp(msg_r, "Exit") == 0) {
            cleanUpAndExit();
        } else {
            strcpy(fall_message, msg_r);
            fallen++;
        }
    }

    if (timer > 0) {
        timer--;
    }

    // Re-schedule the timer every 1000 ms (1 second)
    glutTimerFunc(1000, readFromPipe, 0);
    glutPostRedisplay(); // Refresh OpenGL screen if needed
}

void cleanUpAndExit() {
    close(pipe_drawer[0]); // Close read end of the pipe
    close(pipe_drawer[1]); // Close write end if used
    printf("🧹 Cleanup done. Exiting game.\n");
    exit(0); // Exit the process
}


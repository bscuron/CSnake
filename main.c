#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>

#define DELAY 75000
#define SCORE_INCREMENT 5

typedef enum CELL_TYPES{
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD,
} CELL_TYPES;

typedef enum DIRECTIONS{
    NORTH,
    SOUTH,
    EAST,
    WEST,
} DIRECTIONS;

typedef struct Snake{
    int y, x;
} Snake;

typedef struct Food{
    int y, x;
} Food;

int rows, cols;
int score = 1;
int quit = 0;

struct winsize getWinsize(int fd);
void fillBoard(int* board, int value);
void set(int* board, int row, int col, int value);
int get(int* board, int row, int col);
void showBoard(int* board, Snake* snake, int* slen);
void updateBoard(int* board, Snake* snake, int* slen, int* directions, int* dlen, Food* food);
void handleKeys(int* directions, int* dlen);
void foodAte(int* board, Snake* snake, int* slen);
void setFoodLocation(int* board, Snake* snake, int* slen, Food* food);
int getRand(int min, int max);

int main(void){
    srand(time(NULL));

    struct winsize win = getWinsize(STDOUT_FILENO);
    rows = win.ws_row;
    cols = win.ws_col;
    
    int board[rows][cols];
    fillBoard((int*) board, CELL_EMPTY);

    Snake snake[rows * cols];
    Snake part = {0, 0};
    int slen = 1;
    snake[slen - 1] = part;

    Food food;
    setFoodLocation((int*) board, snake, &slen, &food);

    int directions[rows * cols];
    directions[0] = EAST;
    int dlen = 1;

    initscr();
    curs_set(0);
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    while(!quit){
        handleKeys(directions, &dlen);
        updateBoard((int*) board, snake, &slen, directions, &dlen, &food);
        showBoard((int*) board, snake, &slen);
        usleep(DELAY);
    }
    endwin();
    printf("%sFinal score: %d\n", score >= rows * cols ? "You win!, " : "", score);
    return 0;
}

void setFoodLocation(int* board, Snake* snake, int* slen, Food* food){
    int y, x;
    int cont = 0;
    while(!cont){
        y = getRand(0, rows - 1);
        x = getRand(0, cols - 1);
        cont = 1;
        for(int i = 0; i < *slen; i++){
            if(snake[i].x == x && snake[i].y == y){
                cont = 0;
            }
        }
    }
    food->x = x;
    food->y = y;
}

int getRand(int min, int max){
    return rand() % (max + 1 - min) + min;
}

void handleKeys(int* directions, int* dlen){
    int direction;
    wchar_t code = getch();
    switch(code){
        case -1:
            return;
        case KEY_UP:
        case 'w':
            if(directions[*dlen - 1] == SOUTH) return;
            direction = NORTH;
            break;
        case KEY_DOWN:
        case 's':
            if(directions[*dlen - 1] == NORTH) return;
            direction = SOUTH;
            break;
        case KEY_RIGHT:
        case 'd':
            if(directions[*dlen - 1] == WEST) return;
            direction = EAST;
            break;
        case KEY_LEFT:
        case 'a':
            if(directions[*dlen - 1] == EAST) return;
            direction = WEST;
            break;
    }
    int codeDup;
    int dup = 0;
    while((codeDup = getch()) == code) dup = 1;
    if(dup) ungetch(codeDup);
    *dlen += 1;
    directions[*dlen - 1] = direction;
}

void updateBoard(int* board, Snake* snake, int* slen, int* directions, int* dlen, Food* food){
    fillBoard(board, CELL_EMPTY);
    set(board, food->y, food->x, CELL_FOOD);
    for(int i = 0; i < *slen; i++){
        set(board, snake[i].y, snake[i].x, CELL_SNAKE);
    }

    int direction = directions[0];
    if(*dlen > 1){
        for(int i = *dlen - 2; i >= 0; i--){
            directions[i] = directions[i + 1];
        }
        *dlen -= 1;
    }

    Snake part;
    switch(direction){
        case NORTH:
            part.y = snake[0].y - 1;
            part.x = snake[0].x;
            break;
        case SOUTH:
            part.y = snake[0].y + 1;
            part.x = snake[0].x;
            break;
        case EAST:
            part.x = snake[0].x + 1;
            part.y = snake[0].y;
            break;
        case WEST:
            part.x = snake[0].x - 1;
            part.y = snake[0].y;
            break;
    }

    if(part.y < 0) part.y = rows - 1;
    else if(part.y > rows - 1) part.y = 0;
    if(part.x < 0) part.x = cols - 1;
    else if(part.x > cols - 1) part.x = 0;

    for(int i = *slen; i >= 1; i--){
        snake[i] = snake[i - 1];
        if(snake[i].x == part.x && snake[i].y == part.y){
            quit = 1;
        }
    }
    snake[0] = part;

    if(*slen < score) *slen += 1;

    if(*slen >= rows * cols){
        quit = 1;
    }

    if(snake[0].y == food->y && snake[0].x == food->x){
        score += SCORE_INCREMENT;
        setFoodLocation(board, snake, slen, food);
    }
}

struct winsize getWinsize(int fd){
    struct winsize win;
    ioctl(fd, TIOCGWINSZ, &win);
    return win;
}

void set(int* board, int row, int col, int value){
    *(board + row * cols + col) = value;
}

int get(int* board, int row, int col){
    return *(board + row * cols + col);
}

void fillBoard(int* board, int value){
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            set(board, y, x, value);
        }
    }
}

void showBoard(int* board, Snake* snake, int* slen){
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            switch(get(board, y, x)){
                case CELL_EMPTY:
                    mvprintw(y, x, "%c\n", ' ');
                    break;
                case CELL_SNAKE:
                    mvprintw(y, x, "%c\n", '#');
                    break;
                case CELL_FOOD:
                    mvprintw(y, x, "%c\n", '@');
                    break;
            }
        }
    }
    int ty = snake[0].y > rows / 2 ? 0 : rows - 1;
    mvprintw(ty, 0, "Score: %d\n", score);
    refresh();
}

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>

typedef enum CELL_TYPE{
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD,
} CELL_TYPE;

int rows, cols;

struct winsize getWinsize(int fd);
void fillBoard(int* board, int value);
void set(int* board, int row, int col, int value);
int get(int* board, int row, int col);
void showBoard(int* board);

int main(void){
    struct winsize win = getWinsize(STDOUT_FILENO);
    rows = win.ws_row;
    cols = win.ws_col;
    
    int board[rows][cols];
    fillBoard((int*) board, CELL_EMPTY);

    initscr();
    curs_set(0);
    int quit = 0;
    while(!quit){
        showBoard((int*) board);
    }
    endwin();
    return 0;
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

void showBoard(int* board){
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            switch(get(board, y, x)){
                case CELL_EMPTY:
                    mvprintw(y, x, "%c\n", ' ');
                    break;
                case CELL_SNAKE:
                    mvprintw(y, x, "%c\n", 'X');
                    break;
                case CELL_FOOD:
                    mvprintw(y, x, "%c\n", '@');
                    break;
            }
        }
    }
    refresh();
}

#include "Gomoku/protocol/pisqpipe.h"

#include <iostream>
#include <ctime>
#include "algorithm/ai.h"

const char *infotext = R"(name="Gomoku_Terminator", author="Shawn Sun", version="2.1", country="China", www="https://lengyanyu258.github.io")";

void brain_init() {
    if (width<5 || height<5) {
        pipeOut("ERROR size of the board");
        return;
    }
    if (width>MAX_BOARD || height>MAX_BOARD) {
        pipeOut("ERROR Maximal board size is %d", MAX_BOARD);
        return;
    }
    border_length = width;
    srand(start_time);
    pipeOut("OK");
}

void brain_restart() {
    int x, y;
    for (x = 0; x<width; x++) {
        for (y = 0; y<height; y++) {
            board[x][y] = 0;
        }
    }
    pipeOut("OK");
}

int isFree(int x, int y) {
    return x >= 0 && y >= 0 && x<width && y<height && board[x][y] == 0;
}

void brain_my(int x, int y) {
    if (isFree(x, y)) {
        board[x][y] = 0x1;
    } else {
        pipeOut("ERROR my move [%d,%d]", x, y);
    }
}

void brain_opponents(int x, int y) {
    if (isFree(x, y)) {
        board[x][y] = 0x2;
    } else {
        pipeOut("ERROR opponents's move [%d,%d]", x, y);
    }
}

void brain_block(int x, int y) {
    if (isFree(x, y)) {
        board[x][y] = 0x4;
    } else {
        pipeOut("ERROR winning move [%d,%d]", x, y);
    }
}

int brain_takeback(int x, int y) {
    if (x >= 0 && y >= 0 && x<width && y<height && board[x][y] != 0) {
        board[x][y] = 0;
        return 0;
    }
    return 2;
}

void brain_turn() {
    int x, y, i;

    i = -1;
    do {
        int max_key = algorithm(start_time, info_timeout_turn);
        if (max_key < 0) {
            pipeOut("ERROR the board is full.");
            return;
        }
        x = max_key >> 8;
        y = max_key & 0xff;
        i++;
        if (terminateAI) return;
    } while (!isFree(x, y));

    if (i>1) pipeOut("DEBUG %d coordinates didn't hit an empty field", i);
    do_mymove(x, y);
}

void brain_end() {
}

#ifdef DEBUG_EVAL
#ifndef ON_UNIX
#include <windows.h>

void brain_eval(int x, int y) {
    HDC dc;
    HWND wnd;
    RECT rc;
    char c;
    wnd = GetForegroundWindow();
    dc = GetDC(wnd);
    GetClientRect(wnd, &rc);
    c = (char)(board[x][y] + '0');
    TextOut(dc, rc.right - 15, 3, &c, 1);
    ReleaseDC(wnd, dc);
}
#else
void brain_eval(int x, int y) {

}
#endif
#endif
/*

//    srand(0);
unsigned int chessboard[15][15] = {
//1 2  3  4  5  6  7  8  9  10 11 12 13 14 15
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 1
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 2
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 3
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 4
{2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 5
{0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 6
{0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 7
{0, 1, 0, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0},  // 8
{0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 9
{0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 10
{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},  // 11
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 12
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 13
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 14
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}   // 15
};

int x, y, max_key = 0;
std::string str;

AI.show_chessboard(&AI.root, 225);
//    AI.root.black_ = !AI.root.black_;
//    AI.set_chessboard(chessboard);
//    AI.UCT(20);
do {
//        AI.root.black_ = !AI.root.black_;

if (AI.set_move(x, y, !AI.root.black_)) {
if (AI.root.check(y * 15 + x - 16, AI.root.chessboard_)) {
break;
}

}
} while(!AI.root.check(max_key, AI.root.chessboard_));
printf("Game Over!\n");
}
*/
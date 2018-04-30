//
// Created by lengy on 2017/12/14.
//

#ifndef GOMOKU_TERMINATOR_GOMOKU_H
#define GOMOKU_TERMINATOR_GOMOKU_H


#include "Chessboard.h"

class Gomoku {
public:
    Chessboard root;

    Gomoku();
    ~Gomoku();
    bool set_move(int x, int y, bool black);
    void set_chessboard(int cb[15][15]);
    int UCT(time_t times = 10);
    void show_chessboard(Chessboard *p, int local);
    void show_debug(Chessboard *p);
    inline void print_chessman(int c, const char *b);
};


#endif //GOMOKU_TERMINATOR_GOMOKU_H

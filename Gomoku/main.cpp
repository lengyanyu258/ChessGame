#include <iostream>
#include <sstream>
#include <ctime>
#include "Gomoku.h"

int main(int argc, char *argv[]) {
    time_t times = 5;
    if (argc == 2) {
        times = strtol(argv[1], nullptr, 10);
    }

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
    Gomoku AI;
    int x, y, max_key = 0;
    std::string str;

    AI.show_chessboard(&AI.root, 225);
//    AI.root.black_ = !AI.root.black_;
//    AI.set_chessboard(chessboard);
//    AI.UCT(20);
    do {
//        AI.root.black_ = !AI.root.black_;
        input:
        printf("Please Input(e.g.:H 8,h 8,8 8):");
        std::cin >> str;
        if (str[0] >= 'a') {
            str[0] -= 32;
        }
        if (str[0] >= 'A') {
            x = str[0] - 64;
        } else {
            std::stringstream ss(str);
            ss >> x;
        }
        std::cin >> y;
        if (AI.set_move(x, y, !AI.root.black_)) {
            if (AI.root.check(y * 15 + x - 16, AI.root.chessboard_)) {
                break;
            }
            max_key = AI.UCT(times);
        } else {
            printf("Invalid Move!\n");
            std::cin.clear();
            std::cin.sync();
            goto input;
        }
    } while(!AI.root.check(max_key, AI.root.chessboard_));
    printf("Game Over!\n");
    getchar();
    getchar();

    return 0;
}

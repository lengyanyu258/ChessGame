//
// Created by shawn on 18-6-11.
//

#include "ai.h"
#include <cstdio>

void show_debug(Chessboard *p, uint16_t local) {
    int x, y, i;
    printf("\nlocals scores:");
    for (y = 0, i = 0; y < border_length; ++y) {
        printf("\n");
        for (x = 0; x < border_length; ++x) {
            if ((x << 8 | y) == p->scores_[i]->key) {
                if (p->scores_[i]->key == local) {
                    printf("*%3d*", p->scores_[i++]->score);
                } else {
                    printf(" %3d ", p->scores_[i++]->score);
                }
            } else {
                printf("     ");
            }
        }
    }
}

int algorithm(clock_t start, clock_t left) {
    Chessboard root;
    if (!root.init_normal()) {
        return -1;
    }
#ifdef DEBUG_EVAL
    root.show(root.chessboard_, 0x8080);
#endif
    if (CLOCKS_PER_SEC != 1000) {
        left = left * CLOCKS_PER_SEC / 1000;
    }

    uint8_t score = 0;
    uint16_t max_key = 0;

    if (root.locals_area_ == border_length * border_length) {
        int o = border_length / 2;
        max_key = o << 8 | o;
        goto back;
    }

    // 根据五子棋的技巧，可以将五子棋的棋型用连珠进行分类，分类过后我们按照威力给每种棋型打分。
    for (int i = 0; i < root.locals_area_; ++i) {
        root.scores_[i]->score = static_cast<int8_t>(optimal(root.chessboard_, root.scores_[i]->key));
        if (root.scores_[i]->score == 100) {
            max_key = root.scores_[i]->key;
            goto back;
        } else if (root.scores_[i]->score > score) {
            score = static_cast<uint8_t>(root.scores_[i]->score);
            max_key = root.scores_[i]->key;
        }
        if (clock() - start >= left) {
            goto back;
        }
    }

back:
#ifdef DEBUG_EVAL
    root.chessboard_[max_key >> 8][max_key & 0xff] = static_cast<uint8_t>(root.opponents_ + 1);
    show_debug(&root, max_key);
    root.show(root.chessboard_, max_key);
#endif

    return max_key;
}

//
// Created by lengy on 2017/12/14.
//

#include <cstdlib>  // rand()
#include <cstring>  // memcpy()
#include "Chessboard.h"

#ifdef HAVE_RANDOM
#define rand random
#endif // HAVE_RANDOM

#include <iostream>
#include "algorithm/ai.h"
using namespace std;

uint8_t board[MAX_BOARD][MAX_BOARD] = {};
int border_length = MAX_BOARD;

Chessboard::Chessboard() = default;

Chessboard::~Chessboard() {
    if (locals_) {
        for (int i = 0; i < locals_area_; ++i) {
            delete locals_[i]->child;
            delete locals_[i];
        }
        delete[] locals_;
    }
    if (scores_) {
        for (int i = 0; i < locals_area_; ++i) {
            delete scores_[i]->child;
            delete scores_[i];
        }
        delete[] scores_;
    }
}

bool Chessboard::init_normal() {
    memcpy(chessboard_, board, sizeof(board));  // 拷贝当前的棋盘
    scores_ = new OptNode*[border_length * border_length];
    for (uint16_t y = 0; y < border_length; ++y) {
        for (uint16_t x = 0; x < border_length; ++x) {
            if (chessboard_[x][y] == 0) {
                scores_[locals_area_] = new OptNode;
                scores_[locals_area_++]->key = x << 8 | y;
            }
        }
    }

    return locals_area_ != 0;
}

bool Chessboard::init_uct() {
    memcpy(chessboard_, board, sizeof(board));  // 拷贝当前的棋盘
    locals_ = new UCBNode*[border_length * border_length];
    for (uint16_t y = 0; y < border_length; ++y) {
        for (uint16_t x = 0; x < border_length; ++x) {
            if (chessboard_[x][y] == 0) {
                locals_[locals_area_] = new UCBNode;
                locals_[locals_area_++]->key = x << 8 | y;
            }
        }
    }

    return locals_area_ != 0;

    /*
    // init locals_
    locals_[locals_range_++] = 7 * 15 + 7;
    locals_[locals_range_++] = 7 * 15 + 6;
    locals_[locals_range_++] = 6 * 15 + 6;
    for (int i = 6, j = 6, k = 2;; k += 2) {
        // right
        for (int right = 0; right < k; ++right) {
            locals_[locals_range_++] = j * 15 + ++i;
        }
        // down
        for (int down = 0; down < k; ++down) {
            locals_[locals_range_++] = ++j * 15 + i;
        }
        // left
        for (int left = 0; left < k; ++left) {
            locals_[locals_range_++] = j * 15 + --i;
        }
        if (--i >= 0) {
            locals_[locals_range_++] = j * 15 + i;
            // up
            for (int up = -1; up < k; ++up) {
                locals_[locals_range_++] = --j * 15 + i;
            }
        } else {
            break;
        }
    }
    */
}

bool Chessboard::Monte_Carlo(uint16_t local) {
    TinyChessboard cb(locals_area_);

    memcpy(cb.chessboard, chessboard_, sizeof(chessboard_));  // 拷贝当前的棋盘
    memcpy(cb.locals, locals_, locals_area_ * sizeof(UCBNode *));  // 拷贝当前的可落子点

    move(local, cb.chessboard, cb.locals, cb.locals_area, opponents_);
    // 判断胜负
    if (check(local, cb.chessboard)) {
        return (absolutely_win_ = true);
    }

    if (opponents_) {
        goto my_turn;
    }

    // 黑棋
opponents_turn:
    if (cb.locals_area == 0) {
        return false;
    }
    local = cb.locals[rand() % cb.locals_area]->key;
    move(local, cb.chessboard, cb.locals, cb.locals_area, true);
    // 判断胜负
    if (check(local, cb.chessboard)) {
        return opponents_;
    }

    // 白棋
my_turn:
    if (cb.locals_area == 0) {
        return false;
    }
    local = cb.locals[rand() % cb.locals_area]->key;
    move(local, cb.chessboard, cb.locals, cb.locals_area, false);
    // 判断胜负
    if (check(local, cb.chessboard)) {
        return !opponents_;
    }

    goto opponents_turn;
}

inline void Chessboard::move(uint16_t local, uint8_t (*cb)[MAX_BOARD], 
                             UCBNode **locals, uint16_t &area, bool black) {
    // 先落子
    cb[local >> 8][local & 0xff] = static_cast<uint8_t>(black + 1);

    // 更新余空对应的值
    for (int i = 0; i < area; ++i) {
        if (locals[i]->key == local) {
            /*UCBNode *t = locals[--area];
            locals[area] = locals[i];
            locals[i] = t;*/
            locals[i] = locals[--area];
            /*
            --area;
            for (int j = i; j < area; ++j) {
                locals[j] = locals[j + 1];
            }
            */
            break;
        }
    }
}

void Chessboard::update_value(bool win, int id) {
    Chessboard *p = this;

    p->locals_[id]->played = true;
    do {
        ++p->locals_[id]->count;
        ++p->total_count_;
        p->locals_[id]->value = (p->locals_[id]->value * (p->locals_[id]->count - 1) + win) / p->locals_[id]->count;
        win = !win;
        id = p->father_id_;
        p = p->father_;
    } while (p != nullptr);
}

void Chessboard::generate_child(int id) {
    Chessboard *child = locals_[id]->child = new Chessboard;
    child->father_id_ = id;
    child->father_ = this;
    child->opponents_ = !opponents_;

    // 复制棋盘与可落子点并更新余空
    child->locals_ = new UCBNode*[locals_area_ - 1];
    memcpy(child->chessboard_, chessboard_, sizeof(chessboard_));
    for (int i = 0; i < locals_area_; ++i) {
        if (i != id) {
            child->locals_[child->locals_area_] = new UCBNode;
            child->locals_[child->locals_area_]->key = locals_[i]->key;
            //child->locals_[child->locals_area_]->count = locals_[i]->count;
            ++child->locals_area_;
        } else {
            child->chessboard_[locals_[i]->key >> 8][locals_[i]->key & 0xff] = static_cast<uint8_t>(opponents_ + 1);
        }
    }

    //child->locals_area_ = (total_locals - child->locals_range_ + 1) * 8;
    //if (child->locals_area_ > child->locals_range_) {
    //    child->locals_area_ = child->locals_range_;
    //}
    //// 更新简单搜索策略
    //child->update_locals(id);
}

bool Chessboard::check(uint16_t local, uint8_t (*cb)[MAX_BOARD]) {
    int win = false;
    int x = local >> 8;
    int y = local & 0xff;
    int i, j, k, lx, ly, rx, ry;

    //横向→
    i = x < 5 ? 0 : x - 4;
    for (k = i; k <= x && k < border_length - 4; ++k) {
        win |= cb[k][y] & cb[k + 1][y] & cb[k + 2][y] & cb[k + 3][y] & cb[k + 4][y];
    }
    //纵向↓
    j = y < 5 ? 0 : y - 4;
    for (k = j; k <= y && k < border_length - 4; ++k) {
        win |= cb[x][k] & cb[x][k + 1] & cb[x][k + 2] & cb[x][k + 3] & cb[x][k + 4];
    }
    //左上到右下↘
    k = x - i < y - j ? x - i : y - j;
    lx = x - k;
    ly = y - k;
    for (k = 0; k < 5 && lx + k < border_length - 4 && ly + k < border_length - 4; ++k) {
        win |= cb[lx + k + 1][ly + k + 1] & cb[lx + k + 2][ly + k + 2] &
               cb[lx + k + 3][ly + k + 3] & cb[lx + k + 4][ly + k + 4] & cb[lx + k][ly + k];
    }
    //右上到左下↙
    i = border_length - x <= 5 ? border_length - 1 : x + 4;
    k = i - x < y - j ? i - x : y - j;
    rx = x + k;
    ry = y - k;
    for (k = 0; k < 5 && rx - k >= 4 && ry + k < border_length - 4; ++k) {
        win |= cb[rx - k - 1][ry + k + 1] & cb[rx - k - 2][ry + k + 2] &
               cb[rx - k - 3][ry + k + 3] & cb[rx - k - 4][ry + k + 4] & cb[rx - k][ry + k];
    }

    return static_cast<bool>(win);
}

inline void print_chessman(uint8_t c, const char b) {
    switch (c) {
        case 0:
            printf(" %c ", b);
            break;
        case 1:
            printf(" o ");
            break;
        case 2:
            printf(" x ");
            break;
        case 3:
            printf(" O ");
            break;
        case 4:
            printf(" X ");
            break;
        default:
            printf(" ? ");
            break;
    }
}

void Chessboard::show(uint8_t(*cb)[MAX_BOARD], uint16_t local) {
    int i, j;
    printf("\n    ");
    for (i = 0; i < border_length; ++i) {
        print_chessman(0, char(i + 'A'));
    }
    printf("\n   +");
    for (i = 0; i < border_length; ++i) {
        printf("---");
    }
    printf("+\n");

    for (j = 0; j < border_length; ++j) {
        printf("%2d |", j + 1);
        for (i = 0; i < border_length; ++i) {
            print_chessman(uint8_t(cb[i][j] + 2 * (local == (i << 8 | j))), '.');
        }
        printf("|\n");
    }

    printf("   +");
    for (i = 0; i < border_length; ++i) {
        printf("---");
    }
    printf("+\n");
}

/*
void Chessboard::update_locals(int id) {
    int locals[total_locals];
    int locals_range = locals_range_;
    int i, j, k = 3, x = id % 15, y = id / 15;
    int around_locals[9][8] = {
        {id + 1,  id + 15, id + 16},
        {id - 1,  id + 14, id + 15},
        {id - 15, id - 14, id + 1},
        {id - 16, id - 15, id - 1},
        {id - 15, id - 14, id + 1, id + 15, id + 16},
        {id - 16, id - 15, id - 1, id + 14, id + 15},
        {id - 1,  id + 1, id + 14, id + 15, id + 16},
        {id - 16, id - 15, id - 14, id - 1, id + 1},
        {id - 16, id - 15, id - 14, id - 1, id + 1, id + 14, id + 15, id + 16}
    };
    memcpy(locals, locals_, sizeof(locals_));
    if (id == 0) {
        j = 0;
    } else if (id == 14) {
        j = 1;
    } else if (id == 210) {
        j = 2;
    } else if (id == 224) {
        j = 3;
    } else {
        k = 5;
        if (x == 0) {
            j = 4;
        } else if (x == 14) {
            j = 5;
        } else if (y == 0) {
            j = 6;
        } else if (y == 14) {
            j = 7;
        } else {
            k = j = 8;
        }
    }
    locals_range_ = 0;
    for (i = 0; i < k; ++i) {
        if (arms_[around_locals[j][i]].count == 0) {
            locals_[locals_range_++] = around_locals[j][i];
            arms_[around_locals[j][i]].count = -1;
        }
    }
    for (i = 0; i < locals_range; ++i) {
        if (arms_[locals[i]].count == -1) {
            arms_[locals[i]].count = 0;
        } else {
            locals_[locals_range_++] = locals[i];
        }
    }
}
*/
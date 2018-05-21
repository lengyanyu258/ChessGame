//
// Created by lengy on 2017/12/14.
//

#include <cstring>
#include "Chessboard.h"

bool Chessboard::Monte_Carlo(int local) {
    int locals[total_locals], range;
    unsigned int chessboard[15][15];

    range = locals_range_;
    memcpy(chessboard, chessboard_, sizeof(chessboard_));   // 拷贝当前的棋盘
    memcpy(locals, locals_, sizeof(locals_));    // 拷贝当前的可落子点

    move(local, black_, chessboard, locals, range);
    // 判断胜负
    if (check(local, chessboard)) {
        absolutely_win_ = true;
        return true;
    }

    if (black_) {
        goto w;
    }

    // 黑棋
    b:
    if (range == 0) {
        return false;
    }
    local = locals[rand() % range];
    move(local, true, chessboard, locals, range);
    // 判断胜负
    if (check(local, chessboard)) {
        return black_;
    }

    // 白棋
    w:
    if (range == 0) {
        return false;
    }
    local = locals[rand() % range];
    move(local, false, chessboard, locals, range);
    // 判断胜负
    if (check(local, chessboard)) {
        return !black_;
    }

    goto b;
}

inline void Chessboard::move(int local, bool black, unsigned int (*cb)[15], int *locals, int &range) {
    // 先落子
    cb[local / 15][local % 15] = static_cast<unsigned int>(black + 1);

    // 更新余空对应的值
    for (int i = 0; i < range; ++i) {
        if (locals[i] == local) {
            --range;
            for (int j = i; j < range; ++j) {
                locals[j] = locals[j + 1];
            }
            break;
        }
    }
}

void Chessboard::update_value(bool win, int id) {
    Chessboard *p = this;
    arms_[id].played = true;
    do {
        ++p->arms_[id].count;
        ++p->total_count_;
        p->arms_[id].value = (p->arms_[id].value * (p->arms_[id].count - 1) + win) / p->arms_[id].count;
        win = !win;
        id = p->father_id_;
        p = p->father_;
    } while (p != nullptr);
}

void Chessboard::generate_child(int id) {
    Chessboard *child = arms_[id].child = new Chessboard;
    child->father_id_ = id;
    child->father_ = this;
    child->black_ = !black_;
    child->locals_range_ = locals_range_;
    // 复制棋盘与可落子点
    memcpy(child->chessboard_, chessboard_, sizeof(chessboard_));
    memcpy(child->locals_, locals_, sizeof(locals_));
    // 复制并更新余空
    child->move(id, black_, child->chessboard_, child->locals_, child->locals_range_);
    for (int i = 0; i < total_locals; ++i) {
        child->arms_[i].count = arms_[i].count;
    }
    child->locals_area_ = (total_locals - child->locals_range_ + 1) * 8;
    if (child->locals_area_ > child->locals_range_) {
        child->locals_area_ = child->locals_range_;
    }
    // 更新简单搜索策略
    child->update_locals(id);
}

Chessboard::~Chessboard() {
    for (auto &arm : arms_) {
        delete arm.child;
    }
}

bool Chessboard::check(int local, unsigned int (*cb)[15]) {
    bool win = false;
    int x = local % 15;
    int y = local / 15;
    int i, j, k, lx, ly, rx, ry;

    //横向→
    i = x < 5 ? 0 : x - 4;
    for (k = i; k <= x && k <= 10; ++k) {
        win |= cb[y][k] & cb[y][k + 1] & cb[y][k + 2] & cb[y][k + 3] & cb[y][k + 4];
    }
    //纵向↓
    j = y < 5 ? 0 : y - 4;
    for (k = j; k <= y && k <= 10; ++k) {
        win |= cb[k][x] & cb[k + 1][x] & cb[k + 2][x] & cb[k + 3][x] & cb[k + 4][x];
    }
    //左上到右下↘
    k = x - i < y - j ? x - i : y - j;
    lx = x - k;
    ly = y - k;
    for (k = 0; k < 5 && ly + k <= 10 && lx + k <= 10; ++k) {
        win |= cb[ly + k][lx + k] & cb[ly + k + 1][lx + k + 1] & cb[ly + k + 2][lx + k + 2] & cb[ly + k + 3][lx + k + 3] & cb[ly + k + 4][lx + k + 4];
    }
    //右上到左下↙
    i = 14 - x < 5 ? 14 : x + 4;
    k = i - x < y - j ? i - x : y - j;
    rx = x + k;
    ry = y - k;
    for (k = 0; k < 5 && ry + k <= 10 && rx - k >= 4; ++k) {
        win |= cb[ry + k][rx - k] & cb[ry + k + 1][rx - k - 1] & cb[ry + k + 2][rx - k - 2] & cb[ry + k + 3][rx - k - 3] & cb[ry + k + 4][rx - k - 4];
    }

    return win;
}

Chessboard::Chessboard() = default;

void Chessboard::update_locals(int id) {
    int locals[total_locals];
    int locals_range = locals_range_;
    int i, j, k = 3, x = id % 15, y = id / 15;
    int around_locals[9][8] = {
            {id + 1, id + 15, id + 16},
            {id - 1, id + 14, id + 15},
            {id - 15, id - 14, id + 1},
            {id - 16, id - 15, id - 1},
            {id - 15, id - 14, id + 1, id + 15, id + 16},
            {id - 16, id - 15, id - 1, id + 14, id + 15},
            {id - 1, id + 1, id + 14, id + 15, id + 16},
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

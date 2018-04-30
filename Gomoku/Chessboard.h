//
// Created by lengy on 2017/12/14.
//

#ifndef GOMOKU_TERMINATOR_CHESSBOARD_H
#define GOMOKU_TERMINATOR_CHESSBOARD_H


#include <cstdio>
#include <cstdlib>

#define total_locals 225

class Chessboard;

struct UCBNode
{
    bool right_choice = true;
    bool played = false;    // 是否被模拟对局过
    int count = 0; // 这台机器玩过的次数
    double value = 0;   // 该节点的收益
    Chessboard *child = nullptr;
};

class Chessboard {
public:
    bool absolutely_win_ = false;
    bool black_ = false; // 先手方
    int chessboard_[15][15] = {{0}};
    int locals_[total_locals];  // 模拟时可落子点
    int locals_range_ = 0;
    int locals_area_;
    UCBNode arms_[total_locals];  // 余空（选择臂）
    unsigned int total_count_ = 0; // 总盘数
    Chessboard *father_ = nullptr; // 父节点
    int father_id_ = 0;   // 父节点的标号

    Chessboard();
    ~Chessboard();
    void update_locals(int id);
    bool Monte_Carlo(int local);
    void update_value(bool win, int id);
    void generate_child(int id);
    bool check(int local, int cb[15][15]);
    void move(int local, bool black, int (*cb)[15], int *locals, int &range);
};


#endif //GOMOKU_TERMINATOR_CHESSBOARD_H

//
// Created by lengy on 2017/12/14.
//

#ifndef GOMOKU_TERMINATOR_CHESSBOARD_H
#define GOMOKU_TERMINATOR_CHESSBOARD_H


#include <cstdint>

#define MAX_BOARD 20

extern uint8_t board[MAX_BOARD][MAX_BOARD];
extern int border_length;

struct Chessboard;

struct OptNode {
    uint16_t key = 0xffff;
    uint8_t score = 0;
    Chessboard *child = nullptr;
};

struct UCBNode {
    uint16_t key = 0xffff;
    bool right_choice = true;
    bool played = false;    // 是否被模拟对局过
    uint32_t count = 0; // 这台机器玩过的次数
    float value = 0;   // 该节点的收益
    Chessboard *child = nullptr;
};

struct TinyChessboard {
    uint8_t chessboard[MAX_BOARD][MAX_BOARD] = {};
    UCBNode **locals = nullptr;
    uint16_t locals_area = 0;

    TinyChessboard(uint16_t locals_area_) {
        locals_area = locals_area_;
        locals = new UCBNode*[locals_area];
    }
    ~TinyChessboard() {
        delete[] locals;
    }
};

struct Chessboard {
    uint8_t chessboard_[MAX_BOARD][MAX_BOARD] = {};
    UCBNode **locals_ = nullptr;  // 模拟时可落子点，余空（选择臂）
    OptNode **scores_ = nullptr;
    uint16_t locals_area_ = 0;  // 全部搜索空间
    //uint8_t locals_range_ = 0;  // 实际搜索搜索界限，逐次放大（目的：加强前期掌控力）

    bool absolutely_win_ = false;
    bool opponents_ = false; // 先手方

    unsigned int total_count_ = 0; // 总盘数
    Chessboard *father_ = nullptr; // 父节点
    int father_id_ = 0;   // 父节点的标号

    Chessboard();
    ~Chessboard();
    bool init_uct();
    bool init_normal();

    //void update_locals(int id);
    bool Monte_Carlo(uint16_t local);
    void update_value(bool win, int id);
    void generate_child(int id);
    void move(uint16_t local, uint8_t (*cb)[MAX_BOARD], UCBNode **locals, uint16_t &range, bool black);

    bool check(uint16_t local, uint8_t (*cb)[MAX_BOARD]);
    void show(uint8_t(*cb)[MAX_BOARD], uint16_t local);
};


#endif //GOMOKU_TERMINATOR_CHESSBOARD_H

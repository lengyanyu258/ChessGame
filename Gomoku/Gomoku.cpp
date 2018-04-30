//
// Created by lengy on 2017/12/14.
//

#include <cmath>
#include <ctime>
#include "Gomoku.h"

int Gomoku::UCT(time_t times) {
    int max_key = 0;
    time_t start, stop;
    Chessboard *p;

    // UCB – The Upper Confidence Bound Algorithm，上置信算法
    double bonus;
    double max_value;
    srand(static_cast<unsigned int>(time(nullptr)));
    printf("Please wait %li seconds...\n", times);
    stop = start = time(nullptr);
    // 1.由当前局面建立根节点，生成根节点的全部子节点，分别进行模拟对局；
    for (int i = 0; i < root.locals_area_; ++i) {
        max_key = root.locals_[i];
        root.update_value(root.Monte_Carlo(max_key), max_key);
        root.generate_child(max_key);
        if (root.absolutely_win_) {
            goto back;
        }
//        p = root.arms_[max_key].child;
//        for (int j = 0; j < p->locals_area_; ++j) {
//            max_key = p->locals_[j];
//            p->update_value(p->Monte_Carlo(max_key), max_key);
//            if (p->absolutely_win_) {
//                max_key = p->father_id_;
//                p = p->father_;
//                delete p->arms_[max_key].child;
//                p->arms_[max_key].child = nullptr;
//                p->arms_[max_key].right_choice = false;
//                break;
//            }
//        }
    }
    // 2.从根节点开始，进行最佳优先搜索；
    for (p = &root; stop - start < times; stop = time(nullptr)) {
        max_value = -1;
        // 3.利用 UCB 公式计算每个子节点的 UCB 值，选择最大值的子节点；
        for (int j = 0, i; j < p->locals_area_; ++j) {
            i = p->locals_[j];
            if (p->arms_[i].count == 0) {
                max_key = i;
                break;
            }
            // UCB = X + sqrt(1.96ln(N) / T)，X 表示以前的收益，N 表示所有机器玩过的总次数，T 表示这台机器玩过的次数。
            bonus = sqrt(1.96 * log(p->total_count_) / p->arms_[i].count);
            if (max_value < bonus + p->arms_[i].value) {
                max_value = bonus + p->arms_[i].value;
                max_key = i;
            }
        }
        // 4.若此节点不是叶节点，则以此节点作为根节点，重复 2；
        if (p->arms_[max_key].child) {
            p = p->arms_[max_key].child;
        } else {
            // 5.直到遇到叶节点，如果叶节点未曾经被模拟对局过，对这个叶节点模拟对局；
            // 6.将模拟对局的收益（一般胜为 1 负为 0）按对应颜色更新该节点及各级祖先节点，同时增加该节点以上所有节点的访问次数；
            if (p->arms_[max_key].right_choice) {
                p->update_value(p->Monte_Carlo(max_key), max_key);
                // 否则为这个叶节点随机生成子节点，并进行模拟对局；
                if (p->absolutely_win_) {
                    max_key = p->father_id_;
                    p = p->father_;
                    delete p->arms_[max_key].child;
                    p->arms_[max_key].child = nullptr;
                    p->arms_[max_key].right_choice = false;
                } else {
                    if (p->arms_[max_key].played && rand() % 3 == 0) {
                        p->generate_child(max_key);
//                        p = p->arms_[max_key].child;
//                        for (int j = 0; j < p->locals_area_; ++j) {
//                            max_key = p->locals_[j];
//                            p->update_value(p->Monte_Carlo(max_key), max_key);
//                            if (p->absolutely_win_) {
//                                max_key = p->father_id_;
//                                p = p->father_;
//                                delete p->arms_[max_key].child;
//                                p->arms_[max_key].child = nullptr;
//                                p->arms_[max_key].right_choice = false;
//                                break;
//                            }
//                        }
                    }
//                    if (p->arms_[max_key].played && p->arms_[max_key].value > 0.5) {
//                        p->generate_child(max_key);
//                    }
                }
            } else {
                p->update_value(false, max_key);
            }
            // 7.回到 2，除非此轮搜索时间结束或者达到预设循环次数；
            p = &root;
        }
    }
    // 8.从当前局面的子节点中挑选平均收益最高的给出最佳着法。
    max_value = -1;
    for (int i = 0; i < root.locals_range_; ++i) {
        if (max_value < root.arms_[root.locals_[i]].value) {
            max_value = root.arms_[root.locals_[i]].value;
            max_key = root.locals_[i];
        }
    }

    back:
    root.move(max_key, root.black_, root.chessboard_, root.locals_, root.locals_range_);
    root.locals_area_ = (total_locals - root.locals_range_ + 1) * 8;
    if (root.locals_area_ > root.locals_range_) {
        root.locals_area_ = root.locals_range_;
    }
    root.update_locals(max_key);
    show_chessboard(&root, max_key);
    show_debug(&root);
//    show_chessboard(root.arms_[max_key].child, max_key);

    root.total_count_ = 0;
    for (int i = 0; i < total_locals; ++i) {
        if (root.chessboard_[i / 15][i % 15] != 0) {
            if (root.arms_[i].child) {
                root.arms_[i].count = 1;
                delete root.arms_[i].child;
                root.arms_[i].child = nullptr;
            } else {
                root.arms_[i].count = 2;
            }
        } else {
            root.arms_[i].count = 0;
        }
        root.arms_[i].played = false;
        root.arms_[i].value = 0;
    }

    return max_key;
}

Gomoku::Gomoku() {
    root.locals_area_ = 1;
    // init locals_
    root.locals_[root.locals_range_++] = 7 * 15 + 7;
    root.locals_[root.locals_range_++] = 7 * 15 + 6;
    root.locals_[root.locals_range_++] = 6 * 15 + 6;
    for (int i = 6, j = 6, k = 2; ; k += 2) {
        // right
        for (int right = 0; right < k; ++right) {
            root.locals_[root.locals_range_++] = j * 15 + ++i;
        }
        // down
        for (int down = 0; down < k; ++down) {
            root.locals_[root.locals_range_++] = ++j * 15 + i;
        }
        // left
        for (int left = 0; left < k; ++left) {
            root.locals_[root.locals_range_++] = j * 15 + --i;
        }
        if (--i >= 0) {
            root.locals_[root.locals_range_++] = j * 15 + i;
            // up
            for (int up = -1; up < k; ++up) {
                root.locals_[root.locals_range_++] = --j * 15 + i;
            }
        } else {
            break;
        }
    }
}

void Gomoku::show_chessboard(Chessboard *p, int local) {
    int i, j;
    printf("\n　ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ\n 1");
//    printf("\n  ABCDEFGHIJKLMNO\n 1");
    print_chessman(p->chessboard_[0][0] + 2 * (local == 0), "╔");
    for (i = 1; i < 14; ++i) {
        print_chessman(p->chessboard_[0][i] + 2 * (local == i), "╤");
    }
    print_chessman(p->chessboard_[0][14] + 2 * (local == 14), "╗");
    printf("\n");
    for (j = 1; j < 14; ++j) {
        printf("%2d", j + 1);
        print_chessman(p->chessboard_[j][0] + 2 * (local == 15 * j), "╟");
        for (i = 1; i < 14; ++i) {
            print_chessman(p->chessboard_[j][i] + 2 * (local == 15 * j + i), "┼");
        }
        print_chessman(p->chessboard_[j][14] + 2 * (local == 15 * j + 14), "╢");
        printf("\n");
    }
    printf("15");
    print_chessman(p->chessboard_[14][0] + 2 * (local == 15 * 14), "╚");
    for (i = 1; i < 14; ++i) {
        print_chessman(p->chessboard_[14][i] + 2 * (local == 15 * 14 + i), "╧");
    }
    print_chessman(p->chessboard_[14][14] + 2 * (local == 15 * 14 + 14), "╝");
    printf("\n");
}

bool Gomoku::set_move(int x, int y, bool black) {
    int key = y * 15 + x - 16;
    if (x < 1 || x > 15 || y < 1 || y > 15 || root.chessboard_[y - 1][x - 1] != 0) {
        return false;
    }
    root.move(key, black, root.chessboard_, root.locals_, root.locals_range_);
    ++root.arms_[key].count;
    root.locals_area_ = (total_locals - root.locals_range_ + 1) * 8;
    if (root.locals_area_ > root.locals_range_) {
        root.locals_area_ = root.locals_range_;
    }
    root.update_locals(key);
    show_chessboard(&root, key);
    return true;
}

inline void Gomoku::print_chessman(int c, const char *b) {
    switch (c) {
        case 0:
//            printf("%s", b);
            printf("　");
            break;
        case 1:
            printf("○");
            break;
        case 2:
            printf("●");
            break;
        case 3:
            printf("□");
            break;
        case 4:
            printf("■");
            break;
        default:break;
    }
}

void Gomoku::set_chessboard(int (*cb)[15]) {
    for (int j = 0; j < 15; ++j) {
        for (int i = 0; i < 15; ++i) {
            if (cb[j][i] != 0) {
                set_move(i + 1, j + 1, static_cast<bool>(cb[j][i] - 1));
            }
        }
    }
}

void Gomoku::show_debug(Chessboard *p) {
//    printf("\n收益（百分比）：\n");
//    for (i = 0; i < total_locals; ++i){
//        if (i % 15 == 0) {
//            printf("\n");
//        }
//        printf("%0.3f ", p->arms_[i].value);
//    }
//    printf("\n\n模拟次数：\n");
//    for (i = 0; i < total_locals; ++i) {
//        if (i % 15 == 0) {
//            printf("},\n{");
//        }
//        if (i % 15 == 14) {
//            printf("%d", p->arms_[i].count);
//        } else {
//            printf("%d, ", p->arms_[i].count);
//        }
//    }
//    printf("}\n");
    printf("Total Count:%d\n", p->total_count_);
}

Gomoku::~Gomoku() = default;

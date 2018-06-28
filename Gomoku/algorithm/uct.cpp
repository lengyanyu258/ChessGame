#include "ai.h"
#include <cmath>
#include <iostream>

#ifdef HAVE_RANDOM
#define rand random
#endif // HAVE_RANDOM

void show_debug(Chessboard *p, uint16_t local) {
    unsigned int x, y, i;
    printf("\nlocals values(%%):");
    for (y = 0, i = 0; y < border_length; ++y) {
        printf("\n");
        for (x = 0; x < border_length; ++x) {
            if ((x << 8 | y) == p->locals_[i]->key) {
                if (p->locals_[i]->key == local) {
                    printf("*%0.1f*", p->locals_[i++]->value * 100);
                } else {
                    printf(" %0.1f ", p->locals_[i++]->value * 100);
                }
            } else {
                printf("      ");
            }
        }
    }

    printf("\n\nrolled times:");
    for (y = 0, i = 0; y < border_length; ++y) {
        printf("\n");
        for (x = 0; x < border_length; ++x) {
            if ((x << 8 | y) == p->locals_[i]->key) {
                if (p->locals_[i]->key == local) {
                    printf("*%4d*", p->locals_[i++]->count);
                } else {
                    printf(" %4d ", p->locals_[i++]->count);
                }
            } else {
                printf("      ");
            }
        }
    }
    printf("\n\nTotal Count:%d\n", p->total_count_);
}

int algorithm(clock_t start, clock_t left) {
    Chessboard root, *p = &root;
    if (!root.init_uct()) {
        return -1;
    }
#ifdef DEBUG_EVAL
    root.show(root.chessboard_, 0x8080);
#endif
    //if (root.locals_area_ == border_length * border_length) {
    //    int o = border_length / 2;
    //    return o << 8 | o;
    //}

    int max_id = 0;
    double max_value;
    // 1.由当前局面建立根节点，生成根节点的全部子节点，分别进行模拟对局；
    for (int i = 0; i < root.locals_area_; ++i) {
        root.update_value(root.Monte_Carlo(root.locals_[i]->key), i);
        root.generate_child(i);
        if (root.absolutely_win_) {
            max_id = i;
            goto back;
        }
    }

    if (CLOCKS_PER_SEC != 1000) {
        left = left * CLOCKS_PER_SEC / 1000;
    }

    int center_pos = border_length / 2;
    int center_len = 1;
    int total_pos = border_length * border_length;
    int x, y;

    // 2.从根节点开始，进行最佳优先搜索；
    while (clock() - start < left) {
        // UCB – The Upper Confidence Bound Algorithm，上置信算法
        double bonus;
        max_id = 0;
        max_value = -1;
        // 3.利用 UCB 公式计算每个子节点的 UCB 值，选择最大值的子节点；
        for (int i = 0; i < p->locals_area_; ++i) {
            x = p->locals_[i]->key >> 8;
            y = p->locals_[i]->key & 0xff;
            center_len = sqrt(pow(abs(x - center_pos), 2) + pow(abs(y - center_pos), 2)) - 1;
            if (center_len > (total_pos - p->locals_area_) / 2) {
                continue;
            }
            //printf("x:%d, y:%d center_len:%d number:%d\n", x, y, center_len, (total_pos - p->locals_area_) / 2);

            if (p->locals_[i]->count == 0) {
                max_id = i;
                break;
            } else if (p->total_count_ > 10000 && double(p->locals_[i]->count) / double(p->total_count_) > 0.3) {
                max_id = i;
                goto back;
            }
            // UCB = X + sqrt(1.96ln(N) / T)，X 表示以前的收益，N 表示所有机器玩过的总次数，T 表示这台机器玩过的次数。
            bonus = sqrt(1.96 * log(p->total_count_) / p->locals_[i]->count);
            if (max_value < bonus + p->locals_[i]->value) {
                max_value = bonus + p->locals_[i]->value;
                max_id = i;
            }
        }
        // 4.若此节点不是叶节点，则以此节点作为根节点，重复 2；
        if (p->locals_[max_id]->child) {
            p = p->locals_[max_id]->child;
        } else {
            // 5.直到遇到叶节点，如果叶节点未曾经被模拟对局过，对这个叶节点模拟对局；
            // 6.将模拟对局的收益（一般胜为 1 负为 0）按对应颜色更新该节点及各级祖先节点，同时增加该节点以上所有节点的访问次数；
            if (p->locals_[max_id]->right_choice) {
                p->update_value(p->Monte_Carlo(p->locals_[max_id]->key), max_id);
                // 否则为这个叶节点随机生成子节点，并进行模拟对局；
                if (p->absolutely_win_) {
                    max_id = p->father_id_;
                    p = p->father_;
                    delete p->locals_[max_id]->child;
                    p->locals_[max_id]->child = nullptr;
                    p->locals_[max_id]->right_choice = false;
                } else {
                    if (p->locals_[max_id]->played && rand() % 3 == 0) {
                        p->generate_child(max_id);
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
                p->update_value(false, max_id);
            }
            // 7.回到 2，除非此轮搜索时间结束或者达到预设循环次数；
            p = &root;
        }
    }
    // 8.从当前局面的子节点中挑选平均收益最高的给出最佳着法。
    for (int i = 0, max_count = 0; i < root.locals_area_; ++i) {
        if (max_count < root.locals_[i]->count) {
            max_count = root.locals_[i]->count;
            max_id = i;
        }
    }

back:
#ifdef DEBUG_EVAL
    uint16_t max_key = root.locals_[max_id]->key;
    root.chessboard_[max_key >> 8][max_key & 0xff] = static_cast<uint8_t>(root.opponents_ + 1);
    show_debug(&root, max_key);
    root.show(root.chessboard_, max_key);
#endif

    return root.locals_[max_id]->key;
}
/*
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

void Gomoku::set_chessboard(int(*cb)[15]) {
    for (int j = 0; j < 15; ++j) {
        for (int i = 0; i < 15; ++i) {
            if (cb[j][i] != 0) {
                set_move(i + 1, j + 1, static_cast<bool>(cb[j][i] - 1));
            }
        }
    }
}
*/
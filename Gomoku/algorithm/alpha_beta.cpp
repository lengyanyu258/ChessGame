#include "ai.h"
#include <cmath>
#include <cstdio>
#include <cstring>

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

int alpha_beta(Chessboard *node, int alpha, int beta, int depth, bool opponent) {
    if (depth == 0) {
        return optimal(node->chessboard_, node->father_->scores_[node->father_id_]->key);
    }

    int new_alpha, new_beta, best;
    int x, y;
    if (opponent) {
        // opponent
        for (int i = 0; i < node->locals_area_; ++i) {
            if (beta <= alpha) {
                return beta;
            }
            x = node->scores_[i]->key >> 8;
            y = node->scores_[i]->key & 0xff;
            node->chessboard_[x][y] = 2;

            new_beta = alpha_beta(node, alpha, beta, depth - 1, false);
                
            node->chessboard_[x][y] = 0;

            if (beta > new_beta) {
                //printf("beta:%d\n", new_beta);
                beta = new_beta;
            }
        }
        best = beta;
    } else {
        // AI
        for (int i = 0; i < node->locals_area_; ++i) {
            if (beta <= alpha) {
                return alpha;
            }
            x = node->scores_[i]->key >> 8;
            y = node->scores_[i]->key & 0xff;
            node->chessboard_[x][y] = 1;

            new_alpha = alpha_beta(node, alpha, beta, depth - 1, true);

            node->chessboard_[x][y] = 0;

            if (alpha < new_alpha) {
                //printf("alpha:%d\n", new_alpha);
                alpha = new_alpha;
            }
        }
        best = alpha;
    }
    /*node->show(node->chessboard_, 0x8080);
    show_debug(node, 0x8080);*/
    //printf("best:%d", best);
    return best;
}

int algorithm(clock_t start, clock_t left) {
    Chessboard root;
    if (!root.init_normal()) {
        return -1;
    }
    //root.show(root.chessboard_, 0x8080);

    int depth = 4;
    int rated_time = 60000;
    int score = 0;
    uint16_t max_key = 0;

    if (root.locals_area_ == border_length * border_length) {
        int o = border_length / 2;
        max_key = o << 8 | o;
        goto back;
    }

    if (CLOCKS_PER_SEC != 1000) {
        left = left * CLOCKS_PER_SEC / 1000;
    }

    // estimate the search depth
    // left_time = rated_time ^ (2 ^ (depth - rated_depth))
    depth = log2f(pow(2, depth) * logf(left) / logf(rated_time));

    Chessboard *p = &root, *child;
    int x, y;
    for (int i = 0; i < p->locals_area_; ++i) {
        x = p->scores_[i]->key >> 8;
        y = p->scores_[i]->key & 0xff;

        if (p->scores_[i]->child == nullptr) {
            child = p->scores_[i]->child = new Chessboard;
            child->father_ = p;
            child->father_id_ = i;
            child->opponents_ = !p->opponents_;

            // 复制棋盘与可落子点并更新余空
            child->scores_ = new OptNode*[p->locals_area_ - 1];
            memcpy(child->chessboard_, p->chessboard_, sizeof(p->chessboard_));
            for (int j = 0; j < p->locals_area_; ++j) {
                if (j != i) {
                    child->scores_[child->locals_area_] = new OptNode;
                    child->scores_[child->locals_area_]->key = p->scores_[j]->key;
                    ++child->locals_area_;
                } else {
                    child->chessboard_[p->scores_[j]->key >> 8][p->scores_[j]->key & 0xff] = static_cast<uint8_t>(p->opponents_ + 1);
                }
            }

            root.scores_[i]->score = alpha_beta(child, -999, 999, depth, child->opponents_);

            root.scores_[i]->child = nullptr;
            delete child;

            if (root.scores_[i]->score == 100) {
                max_key = root.scores_[i]->key;
                goto back;
            } else if (root.scores_[i]->score > score) {
                score = root.scores_[i]->score;
                max_key = root.scores_[i]->key;
            }
        }
    }

back:
    /*
    root.chessboard_[max_key >> 8][max_key & 0xff] = static_cast<uint8_t>(root.opponents_ + 1);
    show_debug(&root, max_key);
    root.show(root.chessboard_, max_key);
    */

    return max_key;
}

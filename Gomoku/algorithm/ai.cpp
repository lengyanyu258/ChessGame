#include "ai.h"

int get_score(int level, bool live, int(*shape)[2]) {
    int score = 0;
    int bonus = (level - 1) * 10;
    if (live) {
        score += bonus;
        ++shape[level][1];
    } else {
        ++shape[level][0];
    }
    switch (level) {
        case 2:
            return score + 10;
        case 3:
            return score + 30;
        case 4:
            return score + 60;
        case 5:
            return 100;
        default:
            break;
    }
    return 0;
}

int optimal(uint8_t(*cb)[MAX_BOARD], uint16_t local) {
    /*
    我们可以将五子棋的连珠可以分为以下几种：

    成5：五子连珠
    活4：两边均不被拦截的四子连珠。
    死4：一边被拦截的四子连珠
    活3：两边均不被拦截的三字连珠
    死3：一边被拦截的三字连珠
    活2：两边均不被拦截的二子连珠
    死2：一边被拦截的二子连珠
    单子：四周无相连棋子
    */
    uint8_t color;
    bool live;
    int shape[6][2] = {};   // level 2-5, live false-true
    int score[2] = {}, level, l;
    int x = local >> 8;
    int y = local & 0xff;
    int i, j, k, lx, ly, rx, ry;

    //    std::cout << x << ' ' << y << std::endl;
    for (int black = 0; black < 2; ++black) {
        cb[x][y] = static_cast<uint8_t>(black + 1);

        /*
        * 成5：100分
        * 活4：90分
        * 死4：60分
        * 活3：50分
        * 死3：30分
        * 活2：20分
        * 死2：10分
        * 单子：0分
        */
        for (level = 5; level >= 2; --level) {
            //横向→
            i = x < level ? 0 : x - level + 1;
            for (k = i; k <= x && k < border_length - level + 1; ++k) {
                if (cb[k][y] == 0) {
                    continue;
                }
                color = cb[k][y];
                for (l = 1; level > l; ++l) {
                    if (color != cb[k + l][y]) {
                        break;
                    }
                }
                if (level == l) {
                    if (level == 5) {
                        return 100;
                    }
                    live = false;
                    if (i - 1 >= 0 && cb[i - 1][y] == 0 && k + l < border_length - level + 1 && cb[k + l][y] == 0) {
                        // 活level
                        live = true;
                    }
                    l = get_score(level, live, shape);
                    if (l > score[black]) {
                        score[black] = l;
                    }
                }
            }
            //纵向↓
            j = y < level ? 0 : y - level + 1;
            for (k = j; k <= y && k < border_length - level + 1; ++k) {
                if (cb[x][k] == 0) {
                    continue;
                }
                color = cb[x][k];
                for (l = 1; level > l; ++l) {
                    if (color != cb[x][k + l]) {
                        break;
                    }
                }
                if (level == l) {
                    if (level == 5) {
                        return 100;
                    }
                    live = false;
                    if (j - 1 >= 0 && cb[x][j - 1] == 0 && k + l < border_length - level + 1 && cb[x][k + l] == 0) {
                        // 活level
                        live = true;
                    }
                    l = get_score(level, live, shape);
                    if (l > score[black]) {
                        score[black] = l;
                    }
                }
            }
            //左上到右下↘
            k = x - i < y - j ? x - i : y - j;
            lx = x - k;
            ly = y - k;
            for (k = 0; k < level && lx + k < border_length - level + 1 && ly + k < border_length - level + 1; ++k) {
                if (cb[lx + k][ly + k] == 0) {
                    continue;
                }
                color = cb[lx + k][ly + k];
                for (l = 1; level > l; ++l) {
                    if (color != cb[lx + k + l][ly + k + l]) {
                        break;
                    }
                }
                if (level == l) {
                    if (level == 5) {
                        return 100;
                    }
                    live = false;
                    if (lx + k + l < border_length - level + 1 && ly + k + l < border_length - level + 1 &&
                        lx - 1 >= 0 && ly - 1 >= 0 && cb[lx - 1][ly - 1] == 0 && cb[lx + k + l][ly + k + l] == 0) {
                        // 活level
                        live = true;
                    }
                    l = get_score(level, live, shape);
                    if (l > score[black]) {
                        score[black] = l;
                    }
                }
            }
            //右上到左下↙
            i = border_length - x <= level ? border_length - 1 : x + level - 1;
            k = i - x < y - j ? i - x : y - j;
            rx = x + k;
            ry = y - k;
            for (k = 0; k < level && rx - k >= level - 1 && ry + k < border_length - level + 1; ++k) {
                if (cb[rx - k][ry + k] == 0) {
                    continue;
                }
                color = cb[rx - k][ry + k];
                for (l = 1; level > l; ++l) {
                    if (color != cb[rx - k - l][ry + k + l]) {
                        break;
                    }
                }
                if (level == l) {
                    if (level == 5) {
                        return 100;
                    }
                    live = false;
                    if (rx - k + 1 < border_length - level + 1 && ry + k + l < border_length - level + 1 &&
                        rx - k - l >= level - 1 && ry + k - 1 >= 0 &&
                        cb[rx - k + 1][ry + k - 1] == 0 && cb[rx - k - l][ry + k + l] == 0) {
                        // 活level
                        live = true;
                    }
                    l = get_score(level, live, shape);
                    if (l > score[black]) {
                        score[black] = l;
                    }
                }
            }
        }
        /*
        * 双死4、死4活3：90分
        * 双活3：80分
        * 死3活3：70分
        * 双活2：40分
        */
        if (shape[4][0] >= 2 || (shape[4][0] > 0 && shape[3][1] > 0)) {
            l = 90;
            if (l > score[black]) {
                score[black] = l;
            }
        } else if (shape[3][1] >= 2) {
            l = 80;
            if (l > score[black]) {
                score[black] = l;
            }
        } else if (shape[3][0] > 0 && shape[3][1] > 0) {
            l = 70;
            if (l > score[black]) {
                score[black] = l;
            }
        } else if (shape[2][1] >= 2) {
            l = 40;
            if (l > score[black]) {
                score[black] = l;
            }
        }
    }
    cb[x][y] = 0;
    return score[0] > score[1] ? score[0] : score[1];
}

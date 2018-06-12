#pragma once

#include <ctime>
#include "Gomoku/Chessboard.h"

int get_score(int level, bool live, int(*shape)[2]);
int optimal(uint8_t(*cb)[MAX_BOARD], uint16_t local);
int algorithm(clock_t start_time, clock_t time_left);

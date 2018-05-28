#pragma once

#include <ctime>
#include "Gomoku/Chessboard.h"

void show_chessboard(uint8_t(*cb)[MAX_BOARD], uint16_t local);
int algorithm(clock_t start_time, clock_t time_left);
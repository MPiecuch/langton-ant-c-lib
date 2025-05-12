/*
MIT License

Copyright (c) 2025 Mateusz Piecuch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ant.h"

bool compare_worlds(ant_world *w1, ant_world *w2) {
    if (w1->num_ants == w2->num_ants &&
        memcmp(w1->ants, w2->ants, w1->num_ants * sizeof(ant)) == 0 &&
        memcmp(&w1->board.board_size, &w1->board.board_size, sizeof(ant_vec)) == 0 &&
        memcmp(w1->board.board, w2->board.board, w1->board.board_size.row * w1->board.board_size.col) == 0)
        return true;
    return false;
}

ant_world* read_world(FILE *file) {
    int board_rows, board_cols, num_ants;
    fscanf(file, "%d %d %d", &board_rows,
                             &board_cols,
                             &num_ants);
    ant_world *world = ant_init((ant_vec){board_rows, board_cols});
    if (!world) {
        printf("MEMORY ERROR\n");
        return NULL;
    }
    ant_vec ant_pos;
    ant_direction ant_dir;
    for (int ant = 0; ant < num_ants; ant++) {
        fscanf(file, "%d %d %d", &ant_pos.row, &ant_pos.col, &ant_dir);
        ant_add_ant(world, ant_pos, ant_dir);
    }
    char buf[128];
    for (int r = 0; r < board_rows; r++) {
        fscanf(file, "%s", buf);
        for (int c = 0; c < board_cols; c++)
            if (buf[c] == 'B')
                ant_set_board_cell(world, (ant_vec){r, c}, ANT_BOARD_CELL_BLACK);
    }
    return world;
}

void print_world(ant_world *w) {
    printf("NUM ANTS: %d\n", w->num_ants);
    for (int i = 0; i < w->num_ants; i++)
        printf("ANT[%d] ROW: %d COL: %d DIRECTION: %d\n", i,
                                                          w->ants[i].pos.row,
                                                          w->ants[i].pos.col,
                                                          w->ants[i].dir);
    printf("BORAD ROWS: %d COLS: %d\n", w->board.board_size.row,
                                        w->board.board_size.col);
    for (int r = 0; r < w->board.board_size.row; r++) {
        for (int c = 0; c < w->board.board_size.col; c++) {
            if (w->board.board[r * w->board.board_size.col + c] == ANT_BOARD_CELL_WHITE) putchar('W');
            if (w->board.board[r * w->board.board_size.col + c] == ANT_BOARD_CELL_BLACK) putchar('B');
        }
        putchar('\n');
    }
}

void process_test(FILE *input_file, FILE *output_file, int test_num) {
    ant_world *input_world, *output_world;
    input_world = read_world(input_file);
    if (!input_world) {
        return;
    }
    ant_step(input_world);
    output_world = read_world(output_file);
    if (compare_worlds(input_world, output_world))
        printf("TEST: %d [OK]\n", test_num);
    else {
        printf("TEST: %d [ERROR]\n", test_num);
        printf("GENERATED OUTPUT:\n");
        print_world(input_world);
        printf("CORRECT OUTPUT\n");
        print_world(output_world);
    }
    ant_free(input_world);
    input_world = NULL;
    ant_free(output_world);
    output_world = NULL;
}

int main(void) {
    printf("TESTING LANGTON'S ANT VERSION: %s\n", ant_str_version());
    for (int test_num = 1;; test_num++) {
        char input_filename[128];
        char output_filename[128];
        sprintf(input_filename, "in/test%d", test_num);
        sprintf(output_filename, "out/test%d", test_num);
        FILE *input_file = fopen(input_filename, "r");
        FILE *output_file = fopen(output_filename, "r");
        if (!input_file) break;
        if (!output_file) break;
        process_test(input_file, output_file, test_num);
    }
    return 0;
}
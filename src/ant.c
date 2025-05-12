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
#include <stdlib.h>
#include <stdbool.h>

#include "ant.h"

/*
===============
ant_next_position

Moves ant by one position according
to ant's direction.
===============
*/
static void
ant_next_position (ant *ant)
{
    ant_vec dir_to_vec[] =
    {
        // {row, col}
        // UP, RIGHT, DOWN, LEFT
        {-1,  0},
        { 0,  1},
        { 1,  0},
        { 0, -1}
    };
    ant->pos.row += dir_to_vec[ant->dir].row;
    ant->pos.col += dir_to_vec[ant->dir].col;
}

/*
===============
ant_realloc

This function expands board, and adjusts
its content depending on direction of expansion (expand_dir).
Position of 'old' board in 'new' board is stored
in board_offset.
===============
*/
static int
ant_realloc (ant_board       *board,
             ant_vec         *board_offset,
             ant_direction   expand_dir)
{
    /*
     * Below is the result of realloc from
     * board and memory perspective
     *
     * b,B  - board cells
     * ?    - newly allocated memory (trash values)
     *
     *     BOARD            BOARD
     *     BEFORE           AFTER
     *     +---+            +---+
     *     |bbb|            |bbb|
     *     |BBB|            |BBB|
     *     +---+            |???|
     *                      |???|
     *                      +---+
     *
     *     MEMORY           MEMORY
     *     BEFORE           AFTER
     *
     *     bbbBBB           bbbBBB??????
     *     ^
     *     *board
     *
     */
    char *tmp = realloc (board->board, 2 * board->board_size.row * board->board_size.col);
    if (tmp == NULL)
        return -1;
    board->board = tmp;

    if (expand_dir == ANT_DIR_UP)
    {
        /*
         *     BOARD            BOARD
         *     BEFORE           AFTER
         *     +---+            +---+
         *     |bbb|            |000|
         *     |BBB|            |000|
         *     |???|            |bbb|
         *     |???|            |BBB|
         *     +---+            +---+
         *
         *     MEMORY           MEMORY
         *     BEFORE           AFTER
         *
         *     bbbBBB??????     000000bbbBBB
         *     ^                ^
         *     *board           *board
         */
        memcpy (board->board + board->board_size.row * board->board_size.col,
                board->board, board->board_size.row * board->board_size.col);
        memset (board->board, 0, board->board_size.row * board->board_size.col);
        board_offset->row += board->board_size.row;
        board->board_size.row *= 2;
    }
    else if (expand_dir == ANT_DIR_DOWN)
    {
        /*
         *     BOARD            BOARD
         *     BEFORE           AFTER
         *     +---+            +---+
         *     |bbb|            |bbb|
         *     |BBB|            |BBB|
         *     |???|            |000|
         *     |???|            |000|
         *     +---+            +---+
         *
         *     MEMORY           MEMORY
         *     BEFORE           AFTER
         *
         *     bbbBBB??????     bbbBBB000000
         *     ^                ^
         *     *board           *board
         */
        memset (board->board + board->board_size.row * board->board_size.col,
                0, board->board_size.row * board->board_size.col);
        board->board_size.row *= 2;
    }
    else if (expand_dir == ANT_DIR_LEFT)
    {
        /*
         *     BOARD            BOARD
         *     BEFORE           AFTER
         *     +---+            +------+
         *     |bbb|            |000bbb|
         *     |BBB|            |000BBB|
         *     |???|            +------+
         *     |???|            
         *     +---+            
         *
         *     MEMORY           MEMORY
         *     BEFORE           AFTER
         *
         *     bbbBBB??????     000bbb000BBB
         *     ^                ^
         *     *board           *board
         */
        for (int r = board->board_size.row - 1; r >= 0; r--)
            memcpy (board->board + (2 * r + 1) * board->board_size.col,
                    board->board + r * board->board_size.col,
                    board->board_size.col);
        for (int r = board->board_size.row - 1; r >= 0; r--)
            memset (board->board + 2 * r * board->board_size.col,
                    0, board->board_size.col);
        board_offset->col += board->board_size.col;
        board->board_size.col *= 2;
    }
    else if (expand_dir == ANT_DIR_RIGHT)
    {
        /*
         *     BOARD            BOARD
         *     BEFORE           AFTER
         *     +---+            +------+
         *     |bbb|            |bbb000|
         *     |BBB|            |BBB000|
         *     |???|            +------+
         *     |???|            
         *     +---+            
         *
         *     MEMORY           MEMORY
         *     BEFORE           AFTER
         *
         *     bbbBBB??????     bbb000BBB000
         *     ^                ^
         *     *board           *board
         */
        for (int r = board->board_size.row - 1; r >= 0; r--)
            memcpy (board->board + (2 * r) * board->board_size.col,
                    board->board + r * board->board_size.col,
                    board->board_size.col);
        for (int r = board->board_size.row - 1; r >= 0; r--)
            memset (board->board + (2 * r + 1) * board->board_size.col,
                    0, board->board_size.col);
        board->board_size.col *= 2;
    }
    return 0;
}

/*
===============
ant_init

Create ant_world with size of board_size
and no ant.
===============
*/
ant_world*
ant_init (ant_vec board_size)
{
    ant_world *world = malloc (sizeof(ant_world));
    world->board.board = (char*)malloc (board_size.row * board_size.col);
    if (!world->board.board)
        return 0;
    memset (world->board.board, ANT_BOARD_CELL_WHITE, board_size.row * board_size.col);
    world->board.board_size = board_size;
    world->num_ants = 0;
    world->ants = NULL;

    return world;
}

/*
===============
ant_add_ant

Add ant with ant_pos and ant_dir to world.
===============
*/

void
ant_add_ant (ant_world *world, ant_vec ant_pos, ant_direction ant_dir)
{
    world->num_ants++;
    world->ants = realloc (world->ants, world->num_ants * sizeof(ant));
    world->ants[world->num_ants - 1].pos = ant_pos;
    world->ants[world->num_ants - 1].dir = ant_dir;
}

/*
===============
ant_free

Release resources.
===============
*/

void
ant_free (ant_world *world)
{
    free (world->board.board);
    world->board.board = 0;
    free (world->ants);
    world->num_ants = 0;
    world->ants = NULL;
    free (world);
}

/*
===============
ant_set_board_cell

Set board cell to selected color.
===============
*/

void
ant_set_board_cell (ant_world *world, ant_vec cell, ant_board_cell cell_color)
{
    world->board.board[cell.row * world->board.board_size.col + cell.col] = cell_color;
}

/*
===============
ant_step

Make a single step for each ant in world.
Expand board if needed.
===============
*/

void
ant_step (ant_world *world)
{
    for (int i = 0; i < world->num_ants; i++)
    {
        ant_vec board_offset = {0};
        int linear_idx = world->ants[i].pos.row * world->board.board_size.col + world->ants[i].pos.col;
        world->board.board[linear_idx] = 1 - world->board.board[linear_idx];
        ant_next_position (&world->ants[i]);

        bool expand = true;
        ant_direction expand_dir;
        if (world->ants[i].pos.row < 0) expand_dir = ANT_DIR_UP;
        else if (world->ants[i].pos.row >= world->board.board_size.row) expand_dir = ANT_DIR_DOWN;
        else if (world->ants[i].pos.col < 0) expand_dir = ANT_DIR_LEFT;
        else if (world->ants[i].pos.col >= world->board.board_size.col) expand_dir = ANT_DIR_RIGHT;
        else expand = false;
        if (expand)
        {
            ant_realloc (&world->board, &board_offset, expand_dir);
            for (int j = 0; j < world->num_ants; j++)
            {
                if (expand_dir == ANT_DIR_UP) world->ants[j].pos.row += board_offset.row;
                if (expand_dir == ANT_DIR_LEFT) world->ants[j].pos.col += board_offset.col;
            }
        }

        linear_idx = world->ants[i].pos.row * world->board.board_size.col + world->ants[i].pos.col;
        if (world->board.board[linear_idx] == ANT_BOARD_CELL_BLACK)
            world->ants[i].dir = (world->ants[i].dir + 3) % 4;
        else if(world->board.board[linear_idx] == ANT_BOARD_CELL_WHITE)
            world->ants[i].dir = (world->ants[i].dir + 1) % 4;
    }
}

/*
===============
ant_version

Returns version as int i.e. v 1.0 is equal to 10
===============
*/
int
ant_version ()
{
    return ANT_VER_MAJOR * 10 + ANT_VER_MINOR;
}

/*
===============
ant_str_version

Returns version as null terminated string i.e. 'ver. 1.0'
===============
*/
char *
ant_str_version ()
{
    static char ver_buf[16];
    sprintf(ver_buf, "ver. %d.%d", ANT_VER_MAJOR, ANT_VER_MINOR);

    return ver_buf;
}
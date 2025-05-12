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

#ifndef ANT_H
#define ANT_H

#define ANT_VER_MAJOR 1
#define ANT_VER_MINOR 0

typedef enum
{
    ANT_DIR_UP,
    ANT_DIR_RIGHT,
    ANT_DIR_DOWN,
    ANT_DIR_LEFT
} ant_direction;

typedef enum
{
    ANT_BOARD_CELL_WHITE,
    ANT_BOARD_CELL_BLACK
} ant_board_cell;

typedef struct
{
    int row, col;
} ant_vec;

typedef struct
{
    ant_vec pos;
    ant_direction dir;
} ant;

typedef struct
{
    char *board;
    ant_vec board_size;
} ant_board;

typedef struct
{
    ant_board board;
    int num_ants;
    ant *ants;
} ant_world;

/*
===============
ant_init

Create ant_world with size of board_size
and no ant.
===============
*/
ant_world* ant_init (ant_vec board_size);

/*
===============
ant_add_ant

Add ant with ant_pos and ant_dir to world.
===============
*/
void ant_add_ant (ant_world *world,
                  ant_vec ant_pos,
                  ant_direction ant_dir);


/*
===============
ant_set_board_cell

Set board cell to selected color.
===============
*/
void ant_set_board_cell (ant_world *world,
                         ant_vec cell,
                         ant_board_cell cell_color);


/*
===============
ant_step

Make a single step for each ant in world.
Expand board if needed.
===============
*/
void ant_step (ant_world *world);

/*
===============
ant_free

Release resources.
===============
*/
void ant_free (ant_world *world);

/*
===============
ant_version

Returns version as int i.e. v 1.0 is equal to 10
===============
*/
int ant_version ();

/*
===============
ant_str_version

Returns version as null terminated string i.e. 'ver. 1.0'
===============
*/
char* ant_str_version ();

#endif //ANT_H
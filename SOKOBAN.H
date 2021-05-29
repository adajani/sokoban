/************************************************************************
* Copyright (C) 2021 by Ahmad Dajani                                    *
*                                                                       *
* This file is part of MY SOKOBAN.                                      *
*                                                                       *
* MY SOKOBAN is free software: you can redistribute it and/or modify it *
* under the terms of the GNU Lesser General Public License as published *
* by the Free Software Foundation, either version 3 of the License, or  *
* (at your option) any later version.                                   *
*                                                                       *
* MY SOKOBAN is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY* without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU Lesser General Public License for more details.                   *
*                                                                       *
* You should have received a copy of the GNU Lesser General Public      *
* License along with MY SOKOBAN.                                        *
* If not, see <http://www.gnu.org/licenses/>.                           *
************************************************************************/

/*@file SOKOBAN.H
* @author Ahmad Dajani <eng.adajani@gmail.com>
* @date 24 May 2021
* @brief Sokoban header file game for DOS
*/
#ifndef __SOKOBAN_H
    #define __SOKOBAN_H

    #define CRLF            '\n'
    #define WALL            '#'
    #define STREET          ' '
    #define BLOCK           '$'
    #define TARGET          '.'
    #define HERO            '@'
    #define CENTER          -1
    #define BLIND_COLOR     7
    #define KEY_ESC         1
    #define KEY_UP          72
    #define KEY_DOWN        80
    #define KEY_RIGHT       77
    #define KEY_LEFT        75
    #define KEY_ENTER       28
    #define INPUT_STATUS    0x03da
    #define VRETRACE        0x08
    #define SCREEN_WIDTH    320
    #define SCREEN_HEIGHT   200
    #define DATA_HEIGHT     86
    #define DATA_WIDTH      307
    #define ARROW_CHAR      37
    #define TARGET_CHAR     38
    #define BLOCK_CHAR      39
    #define WALL_CHAR       40
    #define HERO_CHAR       41
    #define STREET_CHAR     42
    #define HEADER_CHAR     43
    #define EMPTY_CHAR      44
    #define MAX_TILE        73
    #define TILE_WIDTH      16
    #define LOGO_HEIGHT     2
    #define LOGO_WIDTH      14
    #define LOGO_INDEX      45
    #define MAX_MENU_ENTRY  3
    #define ARROW_X_SHIFT   70
    #define ABOUT_MAX_LINE  12
    #define MAX_LEVEL       5
    #define MAX_MOVE        2000

    typedef enum {
        false = 0,
        true = 1
    } bool;

    typedef enum {
        OFF = 3,
        ON = 0X13
    } screen_mode;

    typedef enum {
        PALETTE_NOT_FOUND = 0,
        IMAGE_NOT_FOUND = 1,
        MEMORY_ERROR,FILE_NOT_FOUND = 2
    } error_code;

    typedef struct{
        int x;
        int y;
    } target_t;

    void setpal(unsigned char col, unsigned char r, unsigned char g, unsigned char b);
    void error(error_code err);
    void screen(screen_mode mode);
    void plot_pixel(int x, int y, unsigned char col);
    unsigned char get_pixel(int x, int y);
    void load_palette(char *fileName);
    void put_tile(char *img, int x, int y);
    void alloc_mem();
    void load_data();
    void put_text(char *text,int x,int y);
    void load_map(int n);
    void show_header();
    unsigned char read_key();
    bool check_win();
    void init_game();
    void clean_screen(int tile_index);
    void main_menu();
    void wait_retrace();
    void show_messege(char *txt,int background_tile,int y);
    void wait_until_keypress(int key);
#endif
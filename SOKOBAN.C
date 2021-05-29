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

/*@file SOKOBAN.C
* @author Ahmad Dajani <eng.adajani@gmail.com>
* @date 24 May 2021
* @brief Sokoban game for DOS
*/

#pragma inline
#if __TURBOC__ < 0X0300
    #error "This game need borland compiler ver 3 at least"
#endif
#include <dos.h>
#include <stdio.h>
#include "sokoban.h"

static char *about[]={
    "sokoban game",
    "by ahmad dajani",
    "copy right",
    "2021",
    "",
    "how to play",
    "use arrow keys to",
    "move mushroom and",
    "place blocks over",
    "question mark",
    "",
    "press esc to exit"
};

unsigned char far *VGA_MEM=(unsigned char far*)0xA0000000L;
char *string_buffer; /*general usage string buffer*/
unsigned char **tile;
unsigned char **map;
unsigned int map_y, map_x, level, moves = 0,
             total_blocks, hero_x, hero_y;
target_t *target=NULL;

void main(void){
    int old_hero_x, old_hero_y, dir, lookup, target_index;
    int old_block_x, old_block_y, block_x, block_y;
    int cur_x, cur_y, tile_switch;

    alloc_mem();
    load_data();
    main_menu();

    while(true) {
        if(!kbhit()) continue;

        old_hero_x = hero_x ;
        old_hero_y = hero_y ;

        switch( dir=read_key() ){
            case KEY_ESC:  main_menu(); continue;
            case KEY_UP:   lookup = map[hero_y-1][hero_x];  break;
            case KEY_DOWN: lookup = map[hero_y+1][hero_x];  break;
            case KEY_LEFT: lookup = map[hero_y][hero_x-1];  break;
            case KEY_RIGHT:lookup = map[hero_y][hero_x+1]; break;
            default: continue;
        }

        if(lookup==WALL) continue;

        if(lookup==BLOCK){
            switch(dir){
                case KEY_UP   : lookup = map[hero_y-2][hero_x]; break;
                case KEY_DOWN : lookup = map[hero_y+2][hero_x]; break;
                case KEY_LEFT : lookup = map[hero_y][hero_x-2]; break;
                case KEY_RIGHT: lookup = map[hero_y][hero_x+2]; break;
            }

            if(lookup == BLOCK || lookup == WALL) continue;

            old_block_x = block_x = hero_x;
            old_block_y = block_y = hero_y;

            switch(dir){
                case KEY_UP:
                        map[hero_y-1][hero_x]=STREET;
                        map[hero_y-2][hero_x]=BLOCK;
                        block_y -=2;
                        old_block_y --;
                        break;

                case KEY_DOWN:
                        map[hero_y+1][hero_x]=STREET;
                        map[hero_y+2][hero_x]=BLOCK;
                        block_y +=2;
                        old_block_y ++;
                        break;

                case KEY_LEFT:
                        map[hero_y][hero_x-1]=STREET;
                        map[hero_y][hero_x-2]=BLOCK;
                        block_x -=2;
                        old_block_x --;
                        break;

                case KEY_RIGHT:
                        map[hero_y][hero_x+1]=STREET;
                        map[hero_y][hero_x+2]=BLOCK;
                        block_x +=2;
                        old_block_x ++;
                        break;

            }

            /*darw moving block*/
            put_tile(tile[STREET_CHAR], old_block_x * TILE_WIDTH, TILE_WIDTH + (old_block_y * TILE_WIDTH));
            put_tile(tile[BLOCK_CHAR], block_x * TILE_WIDTH, TILE_WIDTH +(block_y * TILE_WIDTH));
        }

        switch(dir){
            case KEY_UP:   hero_y--; break;
            case KEY_DOWN: hero_y++; break;
            case KEY_LEFT: hero_x--; break;
            case KEY_RIGHT:hero_x++; break;
        }
        moves++;

        if(moves > MAX_MOVE ){
            show_messege("wasted",EMPTY_CHAR,CENTER);
            wait_until_keypress(KEY_ENTER);
            main_menu();
        }

        /*draw hero and target layer*/
        put_tile(tile[STREET_CHAR], old_hero_x * TILE_WIDTH, TILE_WIDTH + (old_hero_y * TILE_WIDTH));

        for(target_index=0; target_index<total_blocks; target_index++) {
            cur_x = target[target_index].x;
            cur_y = target[target_index].y;

            if(map[cur_y][cur_x] == BLOCK) {
                tile_switch = BLOCK_CHAR;
            } 
            else {
                tile_switch = TARGET_CHAR;
            }

            put_tile(tile[tile_switch], cur_x * TILE_WIDTH, TILE_WIDTH + (cur_y * TILE_WIDTH));
        }

        put_tile(tile[HERO_CHAR], hero_x * TILE_WIDTH, TILE_WIDTH + (hero_y * TILE_WIDTH));

        if( check_win() ){
            if(level<MAX_LEVEL){
                sprintf(string_buffer,"level%d complete",level);
            }
            else{
                strcpy(string_buffer,"game over");
            }

            show_messege(string_buffer,EMPTY_CHAR,CENTER);
            wait_until_keypress(KEY_ENTER);

            if(level<MAX_LEVEL){
                clean_screen(STREET_CHAR);
                moves=0;
                level++;
                load_map(level);
                init_game();
                continue;
            }
            else{
                main_menu();
            }
        }

        show_header();
        wait_retrace();
    }
}

/**************************************************************************/
void setpal(unsigned char col, unsigned char r,
            unsigned char g, unsigned char b){
    outportb(0x3C8, col);
    outportb(0x3C9, r);
    outportb(0x3C9, g);
    outportb(0x3C9, b);
}

void error(error_code err){
    static char *messege[]={
        "Pallete not found",
        "Image not found",
        "Memory error",
        "File not found"
    };

    screen(OFF);
    printf("Error: %s.\n", messege[err]);
    exit(-err);
}

void screen(screen_mode mode){
    asm{ mov ax, mode
         int 0x10
    }
}

void plot_pixel(int x, int y, unsigned char col){
    if(x>=0 && x<SCREEN_WIDTH && y>=0 && y<SCREEN_HEIGHT) {
        VGA_MEM[y * SCREEN_WIDTH + x] = col;
    }
}

unsigned char get_pixel(int x, int y){
    if(x>=0 && x<SCREEN_WIDTH && y>=0 && y<SCREEN_HEIGHT) {
        return VGA_MEM[ y*SCREEN_WIDTH + x ];
    }

     return -1;
}

void load_palette(char *fileName){
    unsigned int index;
    unsigned char r,g,b;
    FILE *f;

    if((f=fopen(fileName,"rb"))==NULL) {
        error(PALETTE_NOT_FOUND);
    }

    for(index=0; index<256; index++){
        fread(&r, sizeof(unsigned char), 1, f);
        fread(&g, sizeof(unsigned char), 1, f);
        fread(&b, sizeof(unsigned char), 1, f);
        setpal(index, r>>2, g>>2, b>>2);
    }

    fclose(f);
}

void put_tile(char *img, int x, int y){
    int index,xx,yy;
    unsigned char pixel;

    index=0;
    for(yy=y; yy<y + TILE_WIDTH; yy++){
        for(xx=x; xx<x + TILE_WIDTH; xx++){
            pixel = img[index++];
            if(pixel != BLIND_COLOR) {
                plot_pixel(xx,yy, pixel );
            }
        }
    }
}

void alloc_mem(){
    int y;

    tile=(unsigned char**)malloc(sizeof(unsigned int*) * MAX_TILE);
    if(tile==NULL) {
        error(MEMORY_ERROR);
    }

    for(y=0; y<MAX_TILE; y++){
        tile[y]= (unsigned char*)malloc(sizeof(unsigned char) * TILE_WIDTH << 4);
        if(tile[y]==NULL) {
            error(MEMORY_ERROR);
        }
    }

    string_buffer = (char*)malloc(80);
    if(string_buffer==NULL) {
        error(MEMORY_ERROR);
    }
}

void load_data(){
    FILE *f;
    unsigned int index, tile_index, x, y, xx, yy;
    unsigned clr;

    screen(ON);

    /*use zero pallete to make blind view of loading data*/
    load_palette("data\\zero.act");

    if((f=fopen("data\\game.raw","rb"))==NULL) {
        error(IMAGE_NOT_FOUND);
    }

    /*DRAW data image*/
    for(y=0; y<DATA_HEIGHT; y++){
        for(x=0; x<DATA_WIDTH; x++){
            fread(&clr,sizeof(unsigned char), 1, f);
            plot_pixel(x, y, clr);
        }
    }

    fclose(f);

    /*put data into array*/
    tile_index=0;
    for(y=1; y<DATA_HEIGHT; y += TILE_WIDTH + 1){
        for(x=1; x<DATA_WIDTH; x += TILE_WIDTH + 1){
            index=0;
            for(yy=y; yy<y + TILE_WIDTH; yy++){
                for(xx=x; xx<x + TILE_WIDTH; xx++){
                    tile[tile_index][index++] = get_pixel(xx, yy);
                }
            }
            ++tile_index;
            if(tile_index > MAX_TILE-1) goto done;
        }
    }
done:
    screen(ON);
    load_palette("data\\game.act");
}

void put_text(char *text, int x, int y){
    int index;
    while(*text){
        if(*text>='0' && *text<='9'){
            index = *text - '0';
        }
        else if (*text>='a' && *text<='z'){
            index = 11 + (*text-'a');
        }
        else if( *text==' '){
            index= 10;
        }

        put_tile( tile[index],x,y);
        x += TILE_WIDTH;
        text++;
    }
}

void load_map(int n){
    static char fileName[30];
    unsigned char c;
    int index, x, y, target_index=0;
    FILE *f;

    sprintf(fileName,"data\\level%d.txt",n);

    if((f=fopen(fileName,"r"))==NULL) {
        error(FILE_NOT_FOUND);
    }

    fscanf(f,"%d,%d,%d", &map_y, &map_x, &total_blocks);

    /*remove CRLF right after numbers*/
    fread(&c,sizeof(unsigned char), 1, f);

    /*release target struct if exsist*/
    if(target != NULL) {
        free(target);
    }

    /*create targets struct*/
    target=(target_t*)malloc(sizeof(target_t)*total_blocks);
    if(target == NULL) {
        error(MEMORY_ERROR);
    }

    /*release memory if exsist*/
    if(map!=NULL && map_y){
        for(y=0; y<map_y; y++){
            free(map[y]);
        }
        free(map);
    }

    /*create dynamic array*/
    map=(unsigned char**)malloc(sizeof(unsigned int*)*map_y);
    if(map==NULL) error(MEMORY_ERROR);
    for(y=0; y<map_y; y++){
        map[y]=(unsigned char*)malloc(sizeof(unsigned char)*map_x);
        if(map[y]==NULL) {
            error(MEMORY_ERROR);
        }
    }

    x = y = index = 0;
    while( !feof(f) ){
        fread(&c, sizeof(unsigned char), 1, f);

        switch(c){
            case CRLF:
                    ++y;
                    x=0;
                    continue;

            case TARGET:
                    index=38;
                    c=STREET;
                    target[target_index].x = x;
                    target[target_index].y = y;
                    target_index++;
                    break;

            case BLOCK:
                    index=BLOCK_CHAR;
                    break;

            case WALL:
                    index=WALL_CHAR;
                    break;

            case HERO:
                    hero_x = x;
                    hero_y = y;
                    c=STREET;/*remove hero from map*/
                    index=STREET_CHAR;
                    break;

            case STREET:
                    index=STREET_CHAR;
                    break;

            default:
                    index=WALL_CHAR;
                    c=STREET; /*any thing else is wall*/
        }

        map[y][x]=c;
        put_tile(tile[index], x * TILE_WIDTH,TILE_WIDTH/*skip one line for header*/+(y*TILE_WIDTH));
        x++;
    }

    fclose(f);
}

void show_header(){
    char header[80];
    sprintf(header, "map%d move%d", level,moves);
    show_messege(header, HEADER_CHAR, 0);
}

unsigned char read_key(){
    asm { mov ah,0
          int 0x16
    }
    return _AH;
}

bool check_win(){
    int x,y,index,count=0;

    for(y=0; y<map_y; y++){
        for(x=0; x<map_x; x++){
            if( map[y][x] == BLOCK ) {
                for(index=0; index<total_blocks; index++){
                    if( target[index].x == x && target[index].y == y ) {
                        count++;
                    }
                }
            }
        }
    }
    if( count == total_blocks ) {
        return true;
    }

    return false;
}

void init_game(){
    show_header();
    put_tile(tile[HERO_CHAR], hero_x * TILE_WIDTH, TILE_WIDTH + (hero_y * TILE_WIDTH));
}

void clean_screen(int tile_index){
    register int x,y;

    for(y=0; y<13; y++) {
        for(x=0; x<20; x++) {
            put_tile(tile[tile_index], x * TILE_WIDTH, y * TILE_WIDTH);
        }
    }
}

void main_menu(){
    bool select=false;
    int x,y,clr,index=0;
    int old_y,center_x,center_y;
    static char *menu[]={"new game", "about", "exit"};

again:
    clean_screen(EMPTY_CHAR);

    /*draw logo*/
    center_x = (((SCREEN_WIDTH/TILE_WIDTH) - LOGO_WIDTH)/2)*TILE_WIDTH;
    for(y=0; y<LOGO_HEIGHT; y++){
        for(x=0; x<LOGO_WIDTH; x++) {
            put_tile(tile[LOGO_INDEX+index], center_x + (x*TILE_WIDTH) , y*TILE_WIDTH );
            index++;
        }
    }

    /*print menu at center of screen*/
    center_y= (SCREEN_HEIGHT - (MAX_MENU_ENTRY*TILE_WIDTH)) / 2;
    for(y=0; y<MAX_MENU_ENTRY; y++) {
        center_x = (SCREEN_WIDTH - (strlen(menu[y])*TILE_WIDTH))/2 ;
        put_text( menu[y] , center_x , center_y );
        center_y += TILE_WIDTH ;
    }

    index=0;
    center_y -= MAX_MENU_ENTRY*TILE_WIDTH;
    y = old_y = center_y+(index*TILE_WIDTH) ;

    while(!select){
        old_y = center_y+(index*TILE_WIDTH);

        put_tile( tile[ARROW_CHAR] , ARROW_X_SHIFT , y );

        if(!kbhit()) continue;

        switch(read_key()){
            case KEY_UP:
                    if(index > 0 ) {
                        index--;
                    }
                    break;

            case KEY_DOWN:
                    if(index < MAX_MENU_ENTRY-1) {
                        index++;
                    }
                    break;

            case KEY_ENTER: 
                    select=true;
                    break;

        }
        y = center_y+(index*TILE_WIDTH) ;
        put_tile( tile[EMPTY_CHAR] , ARROW_X_SHIFT , old_y );
    }

    if(index==1) {
        clean_screen(EMPTY_CHAR);
        for(y=0; y<ABOUT_MAX_LINE; y++){
            show_messege( about[y], EMPTY_CHAR, y*TILE_WIDTH);
        }
        wait_until_keypress(KEY_ESC);
        select=false;
        index=0;
        goto again;
    }
    if(index==2){
        screen(OFF);
        exit(0);
    }

    clean_screen(STREET_CHAR);

    /* init game */
    load_map(level=1);
    moves=0;
    init_game();
}

void wait_retrace(){
    /* wait until done with vertical retrace */
    while  ((inp(INPUT_STATUS) & VRETRACE)){;}
    /* wait until done refreshing */
    while (!(inp(INPUT_STATUS) & VRETRACE)){;}
}

void show_messege(char *txt,int background_tile,int y){
    int x,center_x, center_y;

    center_x=(SCREEN_WIDTH - (strlen(txt)*TILE_WIDTH))/2;

    if( y == CENTER ) {
        center_y=(SCREEN_HEIGHT - TILE_WIDTH)/2;
    }
    else {
        center_y=y;
    }

    for(x=0; x<20; x++) {
        put_tile(tile[background_tile] , x * TILE_WIDTH , center_y );
    }

    put_text(txt, center_x, center_y);
}

void wait_until_keypress(int key){
    while(read_key()!=key);
}

/*
 *  For Raspberry Pi pico using 64x64 matrix LED
 * 
 *   Copyright (C) 2023-2025 by Y.Ishioka.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***********/
#define  IMAGE_BUFFER        image_buffer
extern  unsigned char  IMAGE_BUFFER[];
/***********/

void  delay( int ms );
void  clear_image_buffer( unsigned char  *buff );

#include "app_param.h"

#define ENABLE_UP    (1 << 3)
#define ENABLE_RIGHT (1 << 2)
#define ENABLE_DOWN  (1 << 1)
#define ENABLE_LEFT  (1)

#define POS_1UP      (-1)
#define POS_1RIGHT   (+1)
#define POS_1DOWN    (+1)
#define POS_1LEFT    (-1)
#define POS_2UP      (-2)
#define POS_2RIGHT   (+2)
#define POS_2DOWN    (+2)
#define POS_2LEFT    (-2)

#define MAZE_WIDTH   (DEF_IMG_WIDTH - 3)
#define MAZE_LENGTH  (DEF_DISP_HIGH - 3)

#define COLOR_BLOCK    0x1f1f1f
#define COLOR_FOOTMK   0x00007f
#define COLOR_RESULT   0xff0000
#define COLOR_CLEAR    0x000000

#define  STEP_TIME      10
#define  SLIDE_TIME     30
#define  DELAY_TIME     500
#define  NEXT_TIME      500


int  maze_stack_maxall ;
int  maze_stack_max ;
int  maze_stack_tmp ;

char  maze_block[ MAZE_WIDTH*MAZE_LENGTH ] ;
int  maze_width;
int  maze_height;

int  comp_flag ;
int  search_count ;
int  dir[ 4 ] ;
char  maze_ana_buff[ MAZE_LENGTH+2 ][ MAZE_WIDTH+2 ] ;


void  disp_maze_slide( void )
{
  int  x;
  int  y;
  unsigned char  *img_pnt;

  for( y=0 ; y<DEF_DISP_HIGH ; y++ ) {
    img_pnt = IMAGE_BUFFER + DEF_IMG_WIDTH * DEF_IMG_PIXEL * y;
    for( x=0 ; x<DEF_IMG_WIDTH -1 ; x++ ) {
      *img_pnt = *(img_pnt+DEF_IMG_PIXEL);
      img_pnt++;
      *img_pnt = *(img_pnt+DEF_IMG_PIXEL);
      img_pnt++;
      *img_pnt = *(img_pnt+DEF_IMG_PIXEL);
      img_pnt++;
    }
    *(img_pnt++) = 0x00;
    *(img_pnt++) = 0x00;
    *(img_pnt++) = 0x00;
  }
}


void  matled_pset( int x, int y, unsigned int color )
{
    unsigned char  *img_pnt;
    unsigned char  pix_r = (unsigned char)(color & 0xff);
    unsigned char  pix_g = (unsigned char)((color >> 8) & 0xff);
    unsigned char  pix_b = (unsigned char)((color >> 16) & 0xff);

    img_pnt = IMAGE_BUFFER
              + (DEF_IMG_WIDTH * DEF_IMG_PIXEL) * y
              + DEF_IMG_PIXEL * x;

    *img_pnt     = pix_r;
    *(img_pnt+1) = pix_g;
    *(img_pnt+2) = pix_b;
}


char  get_block( int x, int y )
{
  return  maze_block[ maze_width * y + x ];
}


void  set_block(int x, int y, int num)
{
  maze_block[ maze_width * y + x ] = (char)num;
}


int  check_dir( int x, int y )
{
  int  count = 0;
  int  dir;
  int  dir_flag = 0;

  /* UP */
  if( y >= 2 ){
    if( get_block(x, y - 2) - get_block(x, y) != 0 ){
      count++;
      dir_flag |= ENABLE_UP;
    }
  }

  /* RIGHT */
  if( x < maze_width - 2 ){
    if( get_block(x + 2, y) - get_block(x, y) != 0 ){
      count++;
      dir_flag |= ENABLE_RIGHT;
    }
  }

  /* DOWN */
  if( y < maze_height - 2 ){
    if( get_block(x, y + 2) - get_block(x, y) != 0 ){
      count++;
      dir_flag |= ENABLE_DOWN;
    }
  }

  /* LEFT */
  if( x >= 2 ){
    if( get_block(x - 2, y) - get_block(x, y) != 0 ){
      count++;
      dir_flag |= ENABLE_LEFT;
    }
  }

  if( count == 0 ){
    return  0;
  }

  dir = rand() % count;

  if( dir_flag & ENABLE_UP ){
    if( dir == 0 ){
      return  ENABLE_UP;
    }
    dir--;
  }

  if( dir_flag & ENABLE_RIGHT ){
    if( dir == 0 ){
      return  ENABLE_RIGHT;
    }
    dir--;
  }

  if( dir_flag & ENABLE_DOWN ){
    if( dir == 0 ){
      return  ENABLE_DOWN;
    }
    dir--;
  }

  if( dir_flag & ENABLE_LEFT ){
    if( dir == 0 ){
      return  ENABLE_LEFT;
    }
  }

  return  -1;
}


int  get_random_block(int *x, int *y)
{
  int i, j;
  int free_block_count = 0;
  int selected_block;
  int count = 0;

  for(i = 0; i < maze_height; i += 2){
    for(j = 0; j < maze_width; j += 2){
      if(get_block(j,i) == 0){
        free_block_count++;
      }
    }
  }

  if(free_block_count != 0){
      selected_block = rand() % free_block_count + 1;
  }
  else{
    return 0;
  }

  for(i = 0; i < maze_height; i += 2){
    for(j = 0; j < maze_width; j += 2){
      if(get_block(j, i) == 0){
        count++;
      }
      if(count == selected_block){
        *x = j;
        *y = i;
        i = maze_height;
        j = maze_width;
      }
    }
  }

  return 1;
}


void  clear_block( int num )
{
  int i, j;

  for( i=0; i<maze_height ; i++ ){
    for( j=0; j<maze_width ; j++ ){
      if( get_block( j, i ) == num ){
        set_block( j, i, 0 );
      }
    }
  }
}


void  clear_block_data( int width, int height )
{
    int  i;

    for( i=0; i<height*width ; i++ ){
        maze_block[ i ] = 0 ;
    }
}


int  create_maze( int width, int height )
{
  int  pos_x, pos_y;
  int  pos_tmp;
  int  dir;
  int  index ;

  maze_width = width;
  maze_height = height;
  index = 0 ;

  while( 1 ){

    if( get_random_block( &pos_x, &pos_y ) ){
      index++;
      set_block( pos_x, pos_y, index );
    } else {
      break;
    }

    while( 1 ){
      dir = check_dir( pos_x, pos_y );

      if( dir == 0 ){
        if( index != 1 ){
          clear_block( index );
        }
        break;

      } else if( dir == ENABLE_UP ){
        set_block( pos_x, pos_y+POS_1UP, index );
        pos_tmp = pos_y+POS_2UP;
        if( get_block( pos_x, pos_tmp ) != 0 ){
          break;
        }
        set_block( pos_x, pos_tmp, index );
        pos_y = pos_tmp;

      } else if( dir == ENABLE_RIGHT ){
        set_block( pos_x+POS_1RIGHT, pos_y, index );
        pos_tmp = pos_x+POS_2RIGHT;
        if( get_block( pos_tmp, pos_y ) != 0 ){
          break;
        }
        set_block( pos_tmp, pos_y, index );
        pos_x = pos_tmp;

      } else if( dir == ENABLE_DOWN ){
        set_block( pos_x, pos_y+POS_1DOWN, index );
        pos_tmp = pos_y+POS_2DOWN;
        if( get_block( pos_x, pos_tmp ) != 0 ){
          break;
        }
        set_block( pos_x, pos_tmp, index );
        pos_y = pos_tmp;

      } else if( dir == ENABLE_LEFT ){
        set_block( pos_x+POS_1LEFT, pos_y, index );
        pos_tmp = pos_x+POS_2LEFT;
        if( get_block( pos_tmp, pos_y ) != 0 ){
          break;
        }
        set_block( pos_tmp, pos_y, index );
        pos_x = pos_tmp;
      }
    }
  }

  return 1;
}


void printf_maze( int mode )
{
  int  x;
  int  y;
  int  color = COLOR_BLOCK;

  /* set 1st column */
  if( mode == 0 ) {
    matled_pset( MAZE_WIDTH +1, 0, color );
    matled_pset( MAZE_WIDTH +1, 1, COLOR_RESULT );
    for( y=2 ; y<=MAZE_LENGTH +1 ; y++ ) {
      matled_pset( MAZE_WIDTH +1, y, color );
    }
  } else {
    for( y=0 ; y<=MAZE_LENGTH -1 ; y++ ) {
      matled_pset( MAZE_WIDTH +1, y, color );
    }
    matled_pset( MAZE_WIDTH +1, MAZE_LENGTH, COLOR_RESULT );
    matled_pset( MAZE_WIDTH +1, MAZE_LENGTH +1, color );
  }
  delay( SLIDE_TIME );
  disp_maze_slide();

  for( x=0 ; x<MAZE_WIDTH ; x++ ) {
    matled_pset( MAZE_WIDTH +1, 0, color );
    for( y=0 ; y<MAZE_LENGTH ; y++ ) {
      if( get_block( x, y ) == 0) {
        matled_pset( MAZE_WIDTH +1, (y+1), color );
      }
    }
    matled_pset( MAZE_WIDTH +1, MAZE_LENGTH +1, color );
    delay( SLIDE_TIME );
    disp_maze_slide();
  }

  /* set last column */
  if( mode == 0 ) {
    for( y=0 ; y<=MAZE_LENGTH -1 ; y++ ) {
      matled_pset( MAZE_WIDTH +1, y, color );
    }
    matled_pset( MAZE_WIDTH +1, MAZE_LENGTH +1, color );
  } else {
    matled_pset( MAZE_WIDTH +1, 0, color );
    for( y=2 ; y<=MAZE_LENGTH +1 ; y++ ) {
      matled_pset( MAZE_WIDTH +1, y, color );
    }
  }
  delay( SLIDE_TIME );
}


void search_exit( char maze[], int pos )
{
    int  x;

    maze_ana_buff[pos/(MAZE_WIDTH+2)][pos%(MAZE_WIDTH+2)] = 3;
    matled_pset( (pos%(MAZE_WIDTH+2)), (pos/(MAZE_WIDTH+2)), COLOR_RESULT );
    search_count++ ;
    maze_stack_tmp++ ;
    if( maze_stack_tmp > maze_stack_max ) {
        maze_stack_max = maze_stack_tmp ;
    }
    delay( STEP_TIME ) ;

    for( x = 0; x < 4; x++ ){
        if( maze[ pos+dir[x] ]==0 ) {
            search_exit( maze, pos+dir[x] );
            if( comp_flag != 0 ) {
                break ;
            }
            maze_ana_buff[(pos+dir[x])/(MAZE_WIDTH+2)][(pos+dir[x])%(MAZE_WIDTH+2)] = 4;  /* set footmark */
            matled_pset( ((pos+dir[x])%(MAZE_WIDTH+2)), ((pos+dir[x])/(MAZE_WIDTH+2)), COLOR_FOOTMK );
            delay( STEP_TIME ) ;
        } else if( maze[pos+dir[x]]==2 ){
            comp_flag = 1 ;
            /* set exit mark */
            matled_pset( ((pos+dir[x])%(MAZE_WIDTH+2)), ((pos+dir[x])/(MAZE_WIDTH+2)), COLOR_RESULT );
            break ;
        }
    }
    maze_stack_tmp-- ;
}


void  maze_ana( int mode )
{
    int  x, y ;

    /* clear buffer */
    memset( maze_ana_buff, 0x00, sizeof(maze_ana_buff) ) ;

    maze_stack_tmp = 0 ;
    maze_stack_max = 0 ;

    for( x=0 ; x<MAZE_WIDTH+2 ; x++ ) {
        maze_ana_buff[0][x] = 1 ;
        maze_ana_buff[MAZE_LENGTH+1][x] = 1 ;
    }
    for( y=1 ; y<MAZE_LENGTH+1 ; y++ ) {
        maze_ana_buff[y][0] = 1 ;
        maze_ana_buff[y][MAZE_WIDTH+1] = 1 ;
    }

    for( y=0 ; y<MAZE_LENGTH ; y++ ) {
        for( x=0 ; x<MAZE_WIDTH ; x++ ) {
            if( maze_block[maze_width * y + x] == 0 ) {
                maze_ana_buff[y+1][x+1] = 1 ;
            }
        }
    }

    dir[0] = 1 ;               /* right */
    dir[1] = MAZE_WIDTH+2 ;    /* down */
    dir[2] = -1 ;              /* left */
    dir[3] = -(MAZE_WIDTH+2) ; /* up */
    comp_flag = 0 ;
    search_count = 0 ;

    if( mode == 0 ) {
      maze_ana_buff[1][0] = 3 ;
      maze_ana_buff[MAZE_LENGTH][MAZE_WIDTH + 1] = 2 ;

      /* set start mark */
      matled_pset( 0, 1, COLOR_RESULT );

      search_exit( maze_ana_buff[0], (MAZE_WIDTH + 2) + 1 ) ;

    } else {
      maze_ana_buff[MAZE_LENGTH][0] = 3 ;
      maze_ana_buff[1][MAZE_WIDTH + 1] = 2 ;

      /* set start mark */
      matled_pset( 0, MAZE_LENGTH, COLOR_RESULT );

      search_exit( maze_ana_buff[0], (MAZE_WIDTH + 2) * MAZE_LENGTH + 1 ) ;
    }


    if( maze_stack_max > maze_stack_maxall ) {
        maze_stack_maxall = maze_stack_max ;
    }
}


void  maze_main( void )
{
    maze_stack_maxall = 0;
    clear_image_buffer( (unsigned char *)IMAGE_BUFFER );

    while( 1 ) {
      clear_block_data( MAZE_WIDTH, MAZE_LENGTH );
      create_maze( MAZE_WIDTH, MAZE_LENGTH );
      printf_maze( 0 );
      delay( DELAY_TIME );
      maze_ana( 0 );
      delay( NEXT_TIME ) ;

      clear_block_data( MAZE_WIDTH, MAZE_LENGTH );
      create_maze( MAZE_WIDTH, MAZE_LENGTH );
      printf_maze( 1 );
      delay( DELAY_TIME );
      maze_ana( 1 );
      delay( NEXT_TIME ) ;
    }
}


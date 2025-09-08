/*
 * Copyright (C) 2022-2025 by Y.Ishioka
 */

/***********/
extern  const unsigned char  yoji_data[];
static  unsigned char  *yoji_data_temp = (unsigned char  *)yoji_data;
#define  MODE_HEAD           "Yoji:"
#define  INIT_COLOR          0
#define  IMAGE_BUFFER        image_buffer
extern  unsigned char  IMAGE_BUFFER[];
/***********/

#include "task_yoji_base.c"


void  yoji_main_1( void )
{
    yoji_main();
}

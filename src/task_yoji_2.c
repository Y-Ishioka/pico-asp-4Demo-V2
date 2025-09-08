/*
 * Copyright (C) 2022-2025 by Y.Ishioka
 */

/***********/
#if 1
extern  const unsigned char  eki_data[];
static  unsigned char  *yoji_data_temp = (unsigned char  *)eki_data;
#define  MODE_HEAD           "EKI: "
#else
extern  const unsigned char  yoji_data[];
static  unsigned char  *yoji_data_temp = (unsigned char  *)yoji_data;
#define  MODE_HEAD           "Yoj2:"
#endif
#define  INIT_COLOR          3
#define  IMAGE_BUFFER        image_buffer
extern  unsigned char  IMAGE_BUFFER[];
/***********/

#include "task_yoji_base.c"


void  yoji_main_2( void )
{
    yoji_main();
}

/*
 * Copyright (C) 2022-2025 by Y.Ishioka
 */

/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2012 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <string.h>
#include <stdlib.h>

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "kernel_cfg.h"
#include "sample1.h"

#include "app_param.h"


void  pico_gpio_led_set( int on );
unsigned int  pico_dev_read_adc0( void );
int  pico_dev_adc_init( void );
int  pico_dev_read_dip( void );
int  pico_dev_dip_init( void );
int  pico_gpio_select_btn( void );
int  pico_gpio_restart_btn( void );
int  pico_dev_btn_init( void );
int  pico_dev_chk_spi_miso( void );

unsigned int  pico_dev_rand_read( void );

int hub75_test_init( void );
int hub75_test_loop( void );

void  yoji_main_1( void );
void  yoji_main_2( void );
void  niji_main( void );
void  maze_main( void );
void  niji_cyclic( void );


unsigned int disp_color_code[] = {
    0xff0000, 0x00ff00, 0x1010ff, 0xffff00,
    0x00ffff, 0xff00ff, 0xffffff, 0x000000
};


unsigned char  image_buffer[ DEF_IMAGEBUFF ];

unsigned int  yoji_rand_seed;
int  disp_exec_mode;

ID  active_task;


void delay( int ms )
{
  tslp_tsk( ms );
}


void  clear_image_buffer( void )
{
    memset( image_buffer, 0x00, sizeof(image_buffer) );
}


void task2(intptr_t exinf)
{
    syslog(LOG_NOTICE, "# TASK1: DIP-SW val : %d", pico_dev_read_dip() );
    if( disp_exec_mode == 0 ) {
        yoji_main_1();
    } else {
        yoji_main_2();
    }
}


void task3(intptr_t exinf)
{
    niji_main();
}


void task4(intptr_t exinf)
{
    while( 1 ) {
        maze_main();
    }
}


void task1(intptr_t exinf)
{
    int  rand_wait = 0;
    int  mode;
    int  tmp;

    syslog(LOG_NOTICE, "# TASK2: DIP-SW val : %d", pico_dev_read_dip() );

    mode = pico_dev_read_dip();
    if( mode == 3 ) {
        active_task = TASK4;
    } else if( mode == 2 ) {
        active_task = TASK3;
    } else if( mode == 1 ) {
        active_task = TASK2;
        disp_exec_mode = 1;  /* yoji-ekimei */
    } else {
        active_task = TASK2;
        disp_exec_mode = 0;  /* yoji-jukugo */
    }
    act_tsk( active_task );

    while( true ) {
        tslp_tsk( 5*100 );
        rand_wait++;
        if( rand_wait >= 120 ) {
            yoji_rand_seed = pico_dev_rand_read();
            while( yoji_rand_seed == 0 ) {
                yoji_rand_seed = rand() & 0xffff;
            }
            srand( yoji_rand_seed );
            rand_wait = 0;
        }

        tmp = pico_dev_read_dip();
        if( mode != tmp ) {
            /* terminate scenario task */
            ter_tsk( active_task );

            clear_image_buffer();
            mode = tmp;
            if( mode == 3 ) {
                active_task = TASK4;
            } else if( mode == 2 ) {
                active_task = TASK3;
            } else if( mode == 1 ) {
                active_task = TASK2;
                disp_exec_mode = 1;  /* yoji-ekimei */
            } else {
                active_task = TASK2;
                disp_exec_mode = 0;  /* yoji-jukugo */
            }
            act_tsk( active_task );
        }
    }
}


void cyclic_handler(intptr_t exinf)
{
    if( active_task != TASK3 ) {
        return;
    }

    niji_cyclic();
}


void main_task(intptr_t exinf)
{
    ER  ret;

    (void)hub75_test_init();
    (void)pico_dev_adc_init();
    (void)pico_dev_btn_init();
    (void)pico_dev_dip_init();

    if( pico_gpio_select_btn() != 0 ) {
        syslog(LOG_NOTICE, "# MODE button status : Continuous Mode" );
    } else {
        syslog(LOG_NOTICE, "# Mode button status : Stop Mode" );
    }

    if( pico_gpio_restart_btn() != 0 ) {
        syslog(LOG_NOTICE, "# NEXT button status : ON" );
    } else {
        syslog(LOG_NOTICE, "# Mode button status : OFF" );
    }

    yoji_rand_seed = pico_dev_rand_read();
    syslog(LOG_NOTICE, "# pico_dev_rand_read() : 0x%x", yoji_rand_seed );
    srand( yoji_rand_seed );

    syslog(LOG_NOTICE, "# MAIN: DIP-SW val : %d", pico_dev_read_dip() );

    ret = act_tsk(TASK1);
    syslog(LOG_NOTICE, "# act_tsk(TASK2) : %d", ret );

    ret = sta_cyc(CYCHDR1);
    syslog(LOG_NOTICE, "# sta_cyc(CYCHDR1) : %d", ret );

    while( 1 ) {
        (void)hub75_test_loop();
        tslp_tsk( 1 );
    }
}


/*********************************************************/
void tex_routine(TEXPTN texptn, intptr_t exinf)
{
}


#ifdef CPUEXC1
void
cpuexc_handler(void *p_excinf)
{
}
#endif /* CPUEXC1 */
/*********************************************************/


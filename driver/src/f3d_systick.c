/* f3d_systick.c ---
 * 
 *   Author: Roy Sorce 
 *   Partner: Sarah Sha
 *   Date Created: 4/1/18 
 *   Last Modified by: Roy Sorce 
 *   Date Last Modified: 4/15/18
 *   Assignment: Lab10
 *   Part of: C335
 */  

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */

#include <f3d_systick.h>
#include <f3d_led.h> 
#include <f3d_user_btn.h>
#include <f3d_uart.h>

volatile int systick_flag = 0;
int led = 0;

//static uint32_t TimingDelay;

void f3d_systick_init(void) {
  SysTick_Config(SystemCoreClock/100);
}

void SysTick_Handler(void) {
  //LED Slow Motion
  
  if(user_btn_read()){
    SysTick_Config(SystemCoreClock/10);
  } else{
    SysTick_Config(SystemCoreClock/100);
  }
  /*
  f3d_led_off(led);
  led = (led + 1) % 9;
  f3d_led_on(led);
  */
}

/* f3d_systick.c ends here */

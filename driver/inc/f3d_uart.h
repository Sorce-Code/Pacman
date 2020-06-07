/* f3d_uart.h ---
 * 
 *   Author: Roy Sorce 
 *   Partner: Sarah Sha
 *   Date Created: 4/1/18 
 *   Last Modified by: Roy Sorce 
 *   Date Last Modified: 4/5/18
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

void f3d_uart_init(void);
int putchar(int);
int getchar(void);
void USART1_IRQHandler(void); //added for interrupt
void flush_uart(void);

/* f3d_uart.h ends here */

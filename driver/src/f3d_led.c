/************************
 *f3d_led.c - contains intializations/functions for the leds
 ************************/
/* Roy Sorce : rsorce
Lab Partner- Sarah Sha : slsha
*/

#include <stm32f30x.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <f3d_led.h>


//intializes the port and pins for the leds on the board
void f3d_led_init(void) {
  
  //initialize pins
  GPIO_InitTypeDef pinAll; 
  GPIO_StructInit(&pinAll);
  pinAll.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_8;
  //update settings
  pinAll.GPIO_Mode = GPIO_Mode_OUT;
  pinAll.GPIO_OType = GPIO_OType_PP;
  pinAll.GPIO_Speed = GPIO_Speed_50MHz;
  pinAll.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  //active internal clock of port E
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  
  //connect port E to all pins  
  GPIO_Init(GPIOE, &pinAll);
}

/*Turns on the appropriate led as specified by the parameter.*/
void f3d_led_on(int led) {
  switch(led){
    case 0:
      GPIOE->BSRR = GPIO_Pin_9;
      break;
    case 1:
      GPIOE->BSRR = GPIO_Pin_10;
      break;
    case 2:
      GPIOE->BSRR = GPIO_Pin_11;
      break;
    case 3:
      GPIOE->BSRR = GPIO_Pin_12;
      break;
    case 4:
      GPIOE->BSRR = GPIO_Pin_13;
      break;
    case 5:
      GPIOE->BSRR = GPIO_Pin_14;
      break;
    case 6:
      GPIOE->BSRR = GPIO_Pin_15;
      break;
    case 7:
      GPIOE->BSRR = GPIO_Pin_8;
  }
}

/*Turns off the appropriate led as specified by the parameter*/ 
void f3d_led_off(int led) {
   switch(led){
    case 0:
      GPIOE->BRR = GPIO_Pin_9;
      break;
    case 1:
      GPIOE->BRR = GPIO_Pin_10;
      break;
    case 2:
      GPIOE->BRR = GPIO_Pin_11;
      break;
    case 3:
      GPIOE->BRR = GPIO_Pin_12;
      break;
    case 4:
      GPIOE->BRR = GPIO_Pin_13;
      break;
    case 5:
      GPIOE->BRR = GPIO_Pin_14;
      break;
    case 6:
      GPIOE->BRR = GPIO_Pin_15;
      break;
    case 7:
      GPIOE->BRR = GPIO_Pin_8;
      break;
  }
} 

/*Turns on all LEDs*/
void f3d_led_all_on(void) {
  GPIOE->BSRR = GPIO_Pin_9;
  GPIOE->BSRR = GPIO_Pin_10;
  GPIOE->BSRR = GPIO_Pin_11;
  GPIOE->BSRR = GPIO_Pin_12;
  GPIOE->BSRR = GPIO_Pin_13;
  GPIOE->BSRR = GPIO_Pin_14;
  GPIOE->BSRR = GPIO_Pin_15;
  GPIOE->BSRR = GPIO_Pin_8;
} 

/*Turns off all LEDs*/
void f3d_led_all_off(void) {
  GPIOE->BRR = GPIO_Pin_9;
  GPIOE->BRR = GPIO_Pin_10;
  GPIOE->BRR = GPIO_Pin_11;
  GPIOE->BRR = GPIO_Pin_12;
  GPIOE->BRR = GPIO_Pin_13;
  GPIOE->BRR = GPIO_Pin_14;
  GPIOE->BRR = GPIO_Pin_15;
  GPIOE->BRR = GPIO_Pin_8;
} 

/* led.c ends here */


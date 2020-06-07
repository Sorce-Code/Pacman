/**********************************
 *f3d_user_btn.c 
 *contains the init and read functions for the User Button
 *********************************/

/* Roy Sorce: rsorce
Lab Partner- Sarah Sha: slsha
*/

#include <stm32f30x.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <f3d_user_btn.h>


/*Initialization of the UserButton*/
void f3d_user_btn_init(void){
  
  //initialize pin settings
  GPIO_InitTypeDef pinUser;
  GPIO_StructInit(&pinUser);
  pinUser.GPIO_Pin = GPIO_Pin_0;
  pinUser.GPIO_Mode = GPIO_Mode_IN;
  pinUser.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  //Connect pin to port?
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &pinUser);
}

/*reads the User Button*/
int user_btn_read(void){
  if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1){
    return 1;
  } else{
    return 0;
  }
}

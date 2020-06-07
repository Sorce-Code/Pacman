/* main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: Roy Sorce
 * Partner: Sarah Sha
 * C335 Final Project
 * 4/26/18
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
/* Code: */

#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <ff.h>
#include <diskio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include <fcntl.h>
#include "level.h"

#define TIMER 20000
#define AUDIOBUFSIZE 128

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;

void playSong(int);
char *rowToSong(int);


/****GAME FUNCTIONS */
void homeScreen(void);
void playLevel(int);
void levelSetup(int);
void updateDirection_pacman(void);
void updateDirection_blue(void);
void updateDirection_green(void);
void updateDirection_red(void);
void updateGame(int, int, int);
int wallCheck(void);
int pacmanSafe(int);
void resetPacman(int);
void resetCandies(void);
void winScreen(void);
void populateLevelArray(void);
void populateCandyArray(void);
void printDirection(int, int);
void drawPacman(int, int, int);


uint16_t figure_PACMAN[64] = {WHITE, WHITE, WHITE, YELLOW, YELLOW, WHITE, WHITE, WHITE, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, WHITE, YELLOW, YELLOW, YELLOW, YELLOW, WHITE, WHITE, WHITE, WHITE, WHITE, YELLOW, YELLOW, WHITE, WHITE, WHITE};

uint16_t figure_WHITE[64] = {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE};


uint16_t colorRow[8];

/*** PACMAN AND GHOST(S) VARIABLES*/ 
int pac_curX, pac_curY; //pacman's current (x,y) location
int pac_prevX, pac_prevY; //pacman's previous (x,y) location
int pac_dirX, pac_dirY; //pacman's cuurent x, y heading
int pac_lives; //pacman's lives currently remaining (set to 3 in levelSetup())

int blue_curX, blue_curY; //blue ghost's current (x,y) location
int blue_prevX, blue_prevY; //blue ghost's previous (x,y) location
int blue_dirX, blue_dirY; //blue ghost's current x, y heading

int green_curX, green_curY; //green ghost's current (x,y) location
int green_prevX, green_prevY; //green ghost's previous (x,y) location
int green_dirX, green_dirY; //green ghost's current x, y heading

int red_curX, red_curY; //red ghost's current (x,y) location
int red_prevX, red_prevY; //red ghost's previous (x,y) location
int red_dirX, red_dirY; //red ghost's current x, y heading


/****MAP VARIABLES***/
int totalBorders;
COORD game_map[200];

/*****CANDY VARIABLES**/
int totalCandies, totalCandies_collected;
CANDY candies[64];


/* GAME DECLARATIONS ABOVE*/


/* BMP IMAGE STUFF */
// BMP File Structs
struct bmpfile_magic magic; // unsigned char[2]
struct bmpfile_header header; // file size, creator(s), and offset
BITMAPINFOHEADER info; // header size, dimensions, etc..
struct bmppixel rgb; // 24bit rgb colors
uint16_t rgb_row[128]; //stores a row of rgb colors 

//BMP Helper Functions
uint16_t colorConverter(struct bmppixel); //converts rgb from 24-16 bits
void readPicture(void);
/* BMP IMAGE STUFF*/

nunchuk_t nun; //holds nunchuk data after read

FATFS Fatfs;		/* File system object */
FIL fid;		/* File object */
BYTE Buff[512];		/* File read buffer */
int ret;

FIL Fil;

FRESULT rc;			/* Result code */
DIR dir;			/* Directory object */
FILINFO fno;			/* File information object */
UINT bw, br;
unsigned int retval;
int bytesread;


struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;      
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  if (ret != sizeof(struct ckhd))
    exit(-1);
  if (ckID && (ckID != hd->ckID))
    exit(-1);
}

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

int main(void) { 

  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  f3d_uart_init();
  delay(200);
  f3d_lcd_init();
  delay(200);
  f3d_timer2_init();
  delay(200);
  f3d_dac_init();
  delay(200);
  f3d_delay_init();
  delay(200);
  f3d_rtc_init();
  delay(200);
  f3d_systick_init();
  delay(200);
  f3d_i2c1_init();
  delay(200);
  f3d_nunchuk_init();
  delay(200);
  f3d_led_init();
  delay(200);
  
  printf("Reset\n");
  
  f_mount(0, &Fatfs);/* Register volume work area */
  delay(300);

  homeScreen();
}

void homeScreen(void){
  f3d_lcd_fillScreen(BLUE);
  readPicture();
  f3d_lcd_drawString(10, 20, "Press 'C' to start", BLACK, WHITE);
  
  while(1){
    f3d_nunchuk_read(&nun);
    if(nun.c == 1){
      break;
    }
  }
  playLevel(0);
}


void playLevel(int level){
  
  levelSetup(level);
  
  int iter = 0; //iteration count for keeping track of which pacman sprite to draw
  
  //loop until dead or all levels complete
  while(pac_lives && level != 3){
    
    f3d_nunchuk_read(&nun);
    
    //update headings for pacman and ghost(s)
    updateDirection_pacman();
    updateDirection_blue();
    if(level > 0){
      updateDirection_green();
      if(level > 1){
	updateDirection_red();
      }
    }
    
    //check for first movement of pacman to start game
    if(!(abs(pac_dirX)) && !(abs(pac_dirY)) && iter == 0){ 
      continue;
    }
    
    /*(START)UPDATE PACMAN AND GHOST(S) LOCATIONS*/
    if(pac_dirX != 0 || pac_dirY != 0){ //only update pacman if moving to new location
      //updates pacman
      updateGame(0, iter, level);
      iter++;
    } else{
      delay(10); //too much delay
    }
    //updates blue ghost
    updateGame(1, iter, level);
    
    if(level > 0){
      //updates green ghost
      updateGame(2, iter, level);
      if(level > 1){
	//update red ghost
	updateGame(3, iter, level);
      } else { //delay for 1 extra ghost processing time
	//delay();
      }
    } else { //delay for 2 extra ghost's processing time
      //delay();
    }
    /*(END)UPDATE PACMAN AND GHOST(S) LOCATIONS*/
    
    //update candies
    updateGame(4, iter, level);
    
    //has pacman eaten all candies on this level?
    if(totalCandies_collected == totalCandies){
      level++;
      //playSong(1);
      resetPacman(level);
      iter = 0;
      resetCandies();
      continue;
    }
    
    //check if pacman is safe from ghost(s)
    if(!pacmanSafe(level)){
      
      //decrease lives
      pac_lives--;
      
      //reset positions 
      resetPacman(level);
      
      //reset iteration count
      iter = 0;
      
      //playSong(0);
    }
    
  } // end of game loop
  
  // pacman won all levels(3) or lost all lives
  if(level == 3){ //game has been won: celebrate
    winScreen();
  } else {
    homeScreen(); //return to homescreen after loss
  }
}


//performs all necessary per-frame-updates to the specified character including:
//                                                                      -erasing old characters
//                                                                      -updating coordinates
//                                                                      -drawing new characters
//NOTE: iter is currently unused and is intended to be implemented only if we decide to draw pacman instead of the lettr 'o' as pacman
//0: updates pacman
//1: updates blue
//2: updates green
//3: updates red
//4: updates candies
void updateGame(int character, int iter, int level){
  switch(character){
    int i;
  case 0: //pacman
    //f3d_lcd_drawChar(pac_prevX, pac_prevY, 'o', WHITE, WHITE); //overwrite old pacman
    drawPacman(1, 0, iter);
    //update coordinates
    pac_prevX = pac_curX;
    pac_prevY = pac_curY;
    pac_curX += pac_dirX;
    pac_curY += pac_dirY;
    
    //f3d_lcd_drawChar(pac_curX, pac_curY, 'o', YELLOW, BLACK); //draw new pacman
    drawPacman(0, 0, iter);
    break;
  case 1: //blue ghost
    f3d_lcd_drawChar(blue_prevX, blue_prevY, 'o', WHITE, WHITE); //overwrite old blue ghost
    
    //update coordinates
    blue_prevX = blue_curX;
    blue_prevY = blue_curY;
    blue_curX += blue_dirX;
    blue_curY += blue_dirY;
    
    f3d_lcd_drawChar(blue_curX, blue_curY, 'o', BLUE, WHITE); //draw new blue ghost
    break;
  case 2: //green ghost
    f3d_lcd_drawChar(green_prevX, green_prevY, 'o', WHITE, WHITE); //overwrite old green ghost
    
    //update coordinates
    green_prevX = green_curX;
    green_prevY = green_curY;
    green_curX += green_dirX; 
    green_curY += green_dirY;
    
    f3d_lcd_drawChar(green_curX, green_curY, 'o', GREEN, WHITE); //draw new green ghost
    break;
  case 3: //red ghost
    f3d_lcd_drawChar(red_prevX, red_prevY, 'o', WHITE, WHITE); //overwrite old red ghost
    
    //update coordinates
    red_prevX = red_curX;
    red_prevY = red_curY;
    red_curX += red_dirX; 
    red_curY += red_dirY;
    
    f3d_lcd_drawChar(red_curX, red_curY, 'o', RED, WHITE); //draw new red ghost
    break;
  case 4: //candies
    for(i = 0; i < totalCandies; i++){ //redraw all uneaten candies
      if(candies[i].eaten){ //candy has already been eaten; skip this iteration
	continue;
      }
      if(abs(pac_curX - candies[i].c.x) < 8 && abs(pac_curY - candies[i].c.y) < 8){ //pacman eats a candy
	candies[i].eaten = 1;
        f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', WHITE, WHITE);
	totalCandies_collected++;
	continue;
      }
      if(abs(blue_curX - candies[i].c.x) < 8 && abs(blue_curY - candies[i].c.y) < 8){ //blue ghost eats a candy (just redraw it)
	f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', WHITE, WHITE);
	f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', YELLOW, YELLOW); 
      }
      if(level > 0){
	 if(abs(green_curX - candies[i].c.x) < 8 && abs(green_curY - candies[i].c.y) < 8){ //green ghost eats a candy (just redraw it)
	   f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', WHITE, WHITE);
	   f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', YELLOW, YELLOW); 
	 }
	 if(level > 1){
	   if(abs(red_curX - candies[i].c.x) < 8 && abs(red_curY - candies[i].c.y) < 8){ //red ghost eats a candy (just redraw it)
	     f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', WHITE, WHITE);
	     f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', YELLOW, YELLOW);
	   }
	 }
      }      
    }
    break;
  default: //nothing..
    break;
  }
}

//changes pacman's directional velocity based on last nunchuk interaction
//nunchuk joystick directions are set for: LANDSCAPE orientation
void updateDirection_pacman(void){
 if(nun.jx == 0){ //move left
    pac_dirX = 0;
    pac_dirY = -1;
  } else if(nun.jx == 255){ //move right
    pac_dirX = 0;
    pac_dirY = 1;
  } else if(nun.jy == 255){ //move up
    pac_dirX = 1;
    pac_dirY = 0;
  } else if(nun.jy == 0){ //move down 
    pac_dirX = -1;
    pac_dirY = 0;
  }
  //check if pacman will hit a wall and update heading if collision will occur 
  if(!wallCheck()){  
    pac_dirX = 0;
    pac_dirY = 0;
  }
}

void updateDirection_blue(void){
  if(blue_curX == 64 && blue_curY == 80){ //start moving out of box
    blue_dirX = 1;
  } else if((blue_dirX == 1 || blue_curY == 104) && blue_curX == 80){ //reached edge of box; start moving left around box
    blue_dirX = 0;
    blue_dirY = -1;
  } else if(blue_curX == 80 && blue_curY == 48) {//start moving down around box
    blue_dirX = -1;
    blue_dirY = 0;
  } else if(blue_curX == 40 && blue_curY == 48){//start moving right around box
    blue_dirX = 0;
    blue_dirY = 1;
  } else if(blue_curX == 40 && blue_curY == 104){//start moving up around box
    blue_dirX = 1;
    blue_dirY = 0;
  } else{
    //continue as is with same heading
  }
}

void updateDirection_green(void){
  if (green_curX > 115  && green_curY == 122){   //move right 
    green_dirX = 1;
    green_dirY = 0;
  }
  else if (green_curX == 115 && green_curY < 120){ //move down 
    green_dirX = 0;
    green_dirY = 1;
  }  
  else if (green_curX > 10 && green_curY == 120){ //move left 
    green_dirX = -1;
    green_dirY = 0;
  }
  else if (green_curX == 10 && green_curY > 10) { //move up 
    green_dirX = 0;
    green_dirY = -1;
  }
  else if (green_curX < 108 && green_curY == 10) { //move right 
    green_dirX = 1;
    green_dirY = 0;
  }
  else if (green_curX == 108 && green_curY < 138) { //move down 
    green_dirX = 0;
    green_dirY = 1;
  }
  else if (green_curX < 89  && green_curY == 80){ //move right (up out of box)
    green_dirX = 1;
    green_dirY = 0;
  }
  else if (green_curX == 89 && green_curY < 122){ //move down after out of box
    green_dirX = 0;
    green_dirY = 1;
  }
}


void updateDirection_red(void){
  if (red_curX < 108 && red_curY == 145){ //move right (up)
    red_dirX = 1;
    red_dirY = 0;
  }
  else if (red_curX == 10 && red_curY < 145){ //move down (right)
    red_dirX = 0;
    red_dirY = 1;
  }  
  else if (red_curX < 88  && red_curY == 80){ //move right (up out of box)
    red_dirX = 1;
    red_dirY = 0;
  }
  else if (red_curX == 88 && red_curY > 39){ //move up (left)
    red_dirX = 0;
    red_dirY = -1;
  }
  else if (red_curX > 10  && red_curY == 39){   //move left (down)
    red_dirX = -1;
    red_dirY = 0;
  }
  else if (red_curX == 108 && red_curY > 80) { //move up (left)
    red_dirX = 0;
    red_dirY = -1;
  }
}


// check's pacmans current location and heading to determine if it will hit a wall
//return 1 if pacman is clear of walls; 0 if intersection will occur
int wallCheck(void){
  int i;
  for(i = 0; i < totalBorders; i++){
    if(abs((pac_curX + pac_dirX) - game_map[i].x) < 8 && abs((pac_curY + pac_dirY) - game_map[i].y) < 8){
      return 0;
    }  else{
      //continue looping 
    }
  }
  return 1;
}


//checks pacman's location with respect to the ghosts
//returns 0 if pacman is too close (signifying death)
//returns 1 is pacman is safe from ghosts
int pacmanSafe(int level){
  if(abs(pac_curX - blue_curX) < 5 && abs(pac_curY - blue_curY) < 5){ //all levels
    return 0;
  } 
  if(level > 0){ //level 1 and above
    if(abs(pac_curX - green_curX) < 5 && abs(pac_curY - green_curY) < 5){
      return 0;
    }
    if(level > 1){ //level 2 and above
      if(abs(pac_curX - red_curX) < 5 && abs(pac_curY - red_curY) < 5){ //change this ghosts color
	 return 0;
      }
      if(level > 2){  //level 3 and above
	if(0){ //change if statement
	  return 0;
	}
      }
    }
  }
  return 1;
}


//draws map and sets up characters for gameplay 
void levelSetup(int level){
  printf("Setting up level %d\n", level);
  f3d_lcd_fillScreen(WHITE);
  
  //start pacman with 3 lives
  pac_lives = 3;
  
  //set starting coordinates and draw characters
  resetPacman(level);

  //create level map
  populateLevelArray();
  
  //create candies on map
  populateCandyArray();
  
  //playSong(1); //level song
  printf("Start playing\n");
}


//reset all current characters on board to starting positions
void resetPacman(int level){
  
  //LEDS tell user how many lives are left
  f3d_led_all_off();
  int i;
  for(i = 0; i < pac_lives; i++){
    f3d_led_on(i);
  }

  /*****************PACMAN*/
  //f3d_lcd_drawChar(pac_curX, pac_curY, 'o', WHITE, WHITE); //overwrite pacman
  drawPacman(0, 1, 0);
  //reset pacman coordinates
  pac_curX = 40;
  pac_prevX = 40;
  pac_curY = 80;
  pac_prevY = 80;
  pac_dirX = 0;
  pac_dirY = 0;
  
  //f3d_lcd_drawChar(pac_curX, pac_curY, 'o', YELLOW, BLACK); //draw pacman
  drawPacman(0, 0, 0);
  
  /*****************BLUE*/
  f3d_lcd_drawChar(blue_curX, blue_curY, 'o', WHITE, WHITE); //overwrite ghost blue
  
  //reset blue coordinates
  blue_curX = 64;
  blue_prevX = 64;
  blue_curY = 80;
  blue_prevY = 80;
  blue_dirX = 0;
  blue_dirY = 0;
  
  f3d_lcd_drawChar(blue_curX, blue_curY, 'o', BLUE, WHITE); //draw blue ghost
  
  if(level > 0){
    /****************GREEN*/
    f3d_lcd_drawChar(green_curX, green_curY, 'o', WHITE, WHITE); //overwrite green ghost
    
    //reset green coordinates
    green_curX = 64;
    green_prevX = 64;
    green_curY = 80;
    green_prevY = 80;
    green_dirX = 0;
    green_dirY = 0;
    
    f3d_lcd_drawChar(green_curX, green_curY, 'o', GREEN, WHITE); //draw green ghost
    
    if(level > 1){
      /***************RED*/
      f3d_lcd_drawChar(red_curX, red_curY, 'o', WHITE, WHITE); //overwrite red ghost
    
      //reset green coordinates
      red_curX = 64;
      red_prevX = 64;
      red_curY = 80;
      red_prevY = 80;
      red_dirX = 0;
      red_dirY = 0;
    
      f3d_lcd_drawChar(red_curX, red_curY, 'o', RED, WHITE); //draw red ghost
    }
  }
}

void resetCandies(void){
  int i;
  for(i = 0; i < totalCandies; i++){
    candies[i].eaten = 0;
    f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', YELLOW, YELLOW); 
   }
   totalCandies_collected = 0;
 }

 void winScreen(void){
   f3d_lcd_fillScreen(BLUE);
   f3d_lcd_drawString(40, 120, "YOU WON!!!!", RED, BLUE);

   while(1){
     f3d_nunchuk_read(&nun);
     if (nun.c == 1){
       break;
     }
   }
   homeScreen();
 }

 //creates the game map and populates an array with all blocked off coordinates
 void populateLevelArray(){
   int i;
   totalBorders = 0;
   //populate left and right frame
   //level1[0:15] holds left frame coords
   //level1[16:31] holds right frame coords
   for(i = 0; i < 128; i+=8){
     //left border (top of lcd)
     COORD c;
     c.x = i;
     c.y = 0;
     game_map[totalBorders] = c;
     totalBorders++;
     //right border (bottom of lcd)
     COORD d;
     d.x = i;
     d.y = 152;
     game_map[totalBorders] = d;
     totalBorders++;
   }


   //populate top and bottom frame
   //level1[32:49] holds top frame coords
   //level1[50:67] holds bottom frame coords
   for(i = 8; i < 152; i+=8){
     //bottom border (left of lcd)
     COORD c;
     c.x = 0;
     c.y = i;
     game_map[totalBorders] = c;
     totalBorders++;
     //top border (right of lcd)
     COORD d;
     d.x = 120;
     d.y = i;
     game_map[totalBorders] = d;
     totalBorders++;
   }


   //populate board interior
   //game_map[68-75] holds outer right wing
   //game_map[76-83] holds outer left wing
   for(i = 24 ; i < 104; i+=8){
     
     if(i == 56 || i == 64){ //creates a gap in the outer right/left wing
       continue;
     }

     COORD c;
     c.x = i;
     c.y = 132;
     game_map[totalBorders] = c;
     totalBorders++;
     
     COORD d;
     d.x = i;
     d.y = 20;
     game_map[totalBorders] = d;
     totalBorders++;
   }
  
  //populate board interior
  //game_map[84-91] holds bottom outer wing
  //game_map[92-99] holds uppder outer wing
  for(i = 48; i < 112; i+=8) {
    COORD c;
    c.x = 20;
    c.y = i;
    game_map[totalBorders] = c;
    totalBorders++;
    
    COORD d;
    d.x = 100;
    d.y = i;
    game_map[totalBorders] = d;
    totalBorders++;
  }
  
  //populate board interior
  //game_map[100-] holds top and bottom of box
  for(i = 56; i < 104; i+=8){
    COORD c;
    c.x = 48;
    c.y = i;
    game_map[totalBorders] = c;
    totalBorders++;
    
    if(i == 72 || i == 80){ //create gap for ghosts to escape from
      continue;
    }
    
    COORD d;
    d.x = 72;
    d.y = i;
    game_map[totalBorders] = d;
    totalBorders++;
  }
  
  //populate board interior
  //game_map[x-y] holds right and left of box
  for(i = 56; i < 72; i+=8){
    COORD c;
    c.x = i;
    c.y = 56;
    game_map[totalBorders] = c;
    totalBorders++;
    
    COORD d;
    d.x = i;
    d.y = 96;
    game_map[totalBorders] = d;
    totalBorders++;
  }
  
  printf("Starting to draw border\n");
  
  //draw all borders
  for(i = 0; i < totalBorders; i++){ 
    f3d_lcd_drawChar(game_map[i].x, game_map[i].y, 'x', BLACK, WHITE);
  }
  
  printf("border draw complete: %d totalBorders\n", totalBorders);
  return;
}

void populateCandyArray(void){
  int i;
  totalCandies = 0;
  totalCandies_collected = 0;
  for(i = 8; i < 144; i+=10){
    //make candies
    
    COORD a;
    a.x = 8;
    a.y = i;
    
    CANDY can1;
    can1.c = a;
    can1.eaten = 0;
    
    candies[totalCandies] = can1;
    totalCandies++;
    
    COORD b;
    b.x = 112;
    b.y = i;
    
    CANDY can2;
    can2.c = b;
    can2.eaten = 0;
    
    candies[totalCandies] = can2;
    totalCandies++;
  }
  
  for(i = 16; i < 112; i+=10){
    //make candies
    
    COORD a;
    a.x = i;
    a.y = 8;
    
    CANDY can1;
    can1.c = a;
    can1.eaten = 0;
    
    candies[totalCandies] = can1;
    totalCandies++;
    
    COORD b;
    b.x = i;
    b.y = 144;
    
    CANDY can2;
    can2.c = b;
    can2.eaten = 0;
    
    candies[totalCandies] = can2;
    totalCandies++;
  }
  
  for(i = 8; i < 144; i +=10){
    if(i >= 56 && i <= 104){
      continue;
    }
    COORD a;
    a.x = 64;
    a.y = i;
    
    CANDY can1;
    can1.c = a;
    can1.eaten = 0;
    
    candies[totalCandies] = can1;
    totalCandies++;
  }

  for(i = 0; i < totalCandies; i++){ 
    f3d_lcd_drawChar(candies[i].c.x, candies[i].c.y, 'r', YELLOW, YELLOW);
  }
}


void printDirection(int x, int y){
  if(x != 0) {
    switch(x){
    case 1:
      printf("RIGHT\n");
      return;
    case -1:
      printf("LEFT\n");
      return;
    default:
      printf("Not moving\n");//shouldnt enter
      return;
    }
  }
  if(y != 0){
    switch(y){
    case 1:
      printf("DOWN\n");
      return;
    case -1:
      printf("UP\n");
      return;
    default:
      printf("Not moving\n"); //shouldnt enter
      return;
    }
  }
}

void drawPacman(int overwrite, int reset, int iter){
  int i, j;
  if(overwrite){
    f3d_lcd_setAddrWindow(pac_prevX, pac_prevY, pac_prevX + 7, pac_prevY + 7, MADCTLGRAPHICS);
    //add switch case
    f3d_lcd_pushColor(figure_WHITE, 64);
  } else if(reset){
    f3d_lcd_setAddrWindow(pac_curX, pac_curY, pac_curX + 7, pac_curY + 7, MADCTLGRAPHICS);
    //add switch case
    f3d_lcd_pushColor(figure_WHITE, 64);
  } else {//else draw regular pacman
  f3d_lcd_setAddrWindow(pac_curX, pac_curY, pac_curX + 7, pac_curY + 7, MADCTLGRAPHICS);
  //add switch case
  f3d_lcd_pushColor(figure_PACMAN, 64);
  }
}



void readPicture(void){
  char *pic = "PACMAN.BMP";
  //open file
  rc = f_open(&Fil, "PACMAN.BMP", FA_READ);
  if(rc) {
    printf("line: %i\n", __LINE__);
    die(rc);
  }
  
   /*read in magic(start)*/
  rc = f_read(&Fil, (void *) &magic, sizeof(magic), &br);
  if(rc) {
    printf("line: %i\n", __LINE__);
    die(rc);
    return;
  }
  delay(200);
  printf("Magic %c%c\n", magic.magic[0], magic.magic[1]);
  /*read in magic (end)*/
  
  /*read in header (start)*/
  rc = f_read(&Fil, (void *) &header, sizeof(header), &br);
  if(rc) {
    printf("line: %i\n", __LINE__);
    die(rc);
  }
  printf("file size %d offset %d\n", header.filesz, header.bmp_offset);
  delay(200);
  /*read in header (end)*/

  /*read in file info (start)*/
  rc = f_read(&Fil, (void *) &info, sizeof(info), &br); 
  if(rc) {
    printf("line: %i\n", __LINE__);
    die(rc);
  }
  delay(200);
  printf("width: %d height: %d bitspp: %d\n", info.width, info.height, info.bitspp);
  /*read in file info (end)*/

  /*read in pixel data (start)*/
  uint16_t color;
  int i, j;
  f3d_lcd_setAddrWindow(0, 0, 128, 160, MADCTLGRAPHICS);
  for(i = 0; i < 160; i++){
    for(j= 0; j < 128; j++){
      rc = f_read(&Fil, (void *) &rgb, sizeof(rgb), &br);
      if(rc) {
	printf("line: %i\n", __LINE__);
	die(rc);
      }
      color = colorConverter(rgb);
      //printf("%hu\n", color);
      rgb_row[j] = color;
    } //end of inner for loop
    
    //set address window and push color array (rgb_row)
    //f3d_lcd_setAddrWindow(0, i, 128, i+1, MADCTLGRAPHICS);
    f3d_lcd_pushColor(rgb_row, 128);
  } //end of outer for loop
  /*read in pixel data (end)*/
  
  //close file
  rc = f_close(&Fil);
}


uint16_t colorConverter(struct bmppixel col){
  uint16_t r, g, b, result;
  
  r = (col.r >> 3);
  g = (col.g >> 2) << 5;
  b = (col.b >> 3) << 11;

  result = r | g | b;
  return result;
}


//returns the corresponding song name to the given index row
char *rowToSong(int row){
  char *song;
  switch(row){
  case 0:
    song = "THERMO.WAV";
    break;
  default:
    song = "LEVEL.WAV";
    break;
  }
  return song;
}


void playSong(int choice){
  //retreive wav file name
  char *song = rowToSong(choice);
  printf("starting song..\n");
  rc = f_open(&fid, song, FA_READ);
  if (!rc) {
    printf("file succesfully opened\n");
    struct ckhd hd;
    uint32_t  waveid;
    struct fmtck fck;
    
    readckhd(&fid, &hd, 'FFIR');
    
    f_read(&fid, &waveid, sizeof(waveid), &ret);
    if ((ret != sizeof(waveid)) || (waveid != 'EVAW'))
      return;
    
    readckhd(&fid, &hd, ' tmf');
    
    f_read(&fid, &fck, sizeof(fck), &ret);
    
    // skip over extra info
    
    if (hd.cksize != 16) {
      printf("extra header info %d\n", hd.cksize - 16);
      f_lseek(&fid, hd.cksize - 16);
    }
    
    printf("audio format 0x%x\n", fck.wFormatTag);
    printf("channels %d\n", fck.nChannels);
    printf("sample rate %d\n", fck.nSamplesPerSec);
    printf("data rate %d\n", fck.nAvgBytesPerSec);
    printf("block alignment %d\n", fck.nBlockAlign);
    printf("bits per sample %d\n", fck.wBitsPerSample);
    
    // now skip all non-data chunks !
    
    while(1){
      readckhd(&fid, &hd, 0);
      if (hd.ckID == 'atad')
	break;
      f_lseek(&fid, hd.cksize);
    }
    
    printf("Samples %d\n", hd.cksize);
    
    
    f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
    hd.cksize -= ret;
    audioplayerStart();
    while (hd.cksize) {
      int next = hd.cksize > AUDIOBUFSIZE/2 ? AUDIOBUFSIZE/2 : hd.cksize;
      if (audioplayerHalf) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(Audiobuf, AUDIOBUFSIZE/2);
	f_read(&fid, Audiobuf, next, &ret);
	hd.cksize -= ret;
	audioplayerHalf = 0;
      }
      if (audioplayerWhole) {
	if (next < AUDIOBUFSIZE/2)
	  bzero(&Audiobuf[AUDIOBUFSIZE/2], AUDIOBUFSIZE/2);
	f_read(&fid, &Audiobuf[AUDIOBUFSIZE/2], next, &ret);
	hd.cksize -= ret;
	audioplayerWhole = 0;
      }
    }
    audioplayerStop();
    printf("audio stopped\n");
    return;
  } else{
    die(rc);
  }
  
  printf("\nClose the file.\n"); 
  rc = f_close(&fid);
  
  if (rc) die(rc);
  while (1);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
#endif


/* main.c ends here */

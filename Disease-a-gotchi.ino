#include <TFT_eSPI.h>
#include "title_scrn.h"
#include "goop_baby.h"
#include "Splinter_idle.h"
//#include "death_screen.h"
//#include "game_hud.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite splasscreen = TFT_eSprite(&tft); // Sprite for meter reading
TFT_eSprite bckgrnd = TFT_eSprite(&tft); // Sprite for meter reading
TFT_eSprite foreground = TFT_eSprite(&tft); // Sprite for meter reading
TFT_eSprite sicko = TFT_eSprite(&tft); // Sprite for meter reading
TFT_eSprite frame_render = TFT_eSprite(&tft); // Sprite for meter reading

// Some animated elements to make the screen less stale during random events
TFT_eSprite element_animations[4] = {TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft),TFT_eSprite(&tft)};

// Set the "framerate"
int clck=0;
int clck_draw=1000000;

// Run the game while you aren't dead or "finished"
bool waitStart=true;
bool gameRun=false;

// Menu buttons before starting
int startMeter1=0;
int startMeter2=0;
int startMeter_height=211;
int startMeter_height2=220;
int goop_baby_cycle=0;

// Set a starting position for the menu options, might change this at some point to become a hand instead that has more freedom of movement
int menu_pos=0;

// Two toggels for the buttons to prevent spamming of buttons when held down
int debounce=0;
int debounce2=0;

// Declare some color schemes, these will have to change lateron
static const uint16_t bckgrndclr=0x00C6;//darkblue R=0, G=26, B=49
static const uint16_t frntclr=0xB03C;//pink/purple R=181, G=4, B=225
static const uint16_t buttonclr=0xED5F; //R=237,G=169,B=254
static const uint16_t sick_green=0x3FE4; //R=237,G=169,B=254
static const uint16_t sick_green2=0x4503; //R=68, G=162, B=30
// Set some initial values
int level=1;
int bar_health=50;
int bar_sleep=50;
int bar_food=50;
int bar_happy=0;
int bar_smart=0;
int bar_gainz=0;

int bar_errection=0;
int bar_blueball=0;

//
int count=0;
int pos=0;
bool stats_switch=true;

void setup() {
  // Set the input buttons
  pinMode(35,INPUT_PULLUP);
  pinMode(0,INPUT_PULLUP);

  // initialize the screen
  tft.init();
  tft.setSwapBytes(true);
  tft.pushImage(0,0,135,240,title_scrn);
  
  // Create an empty sprite to render the background on
  bckgrnd.createSprite(135, 240);
  bckgrnd.setSwapBytes(true);
  bckgrnd.fillSprite(TFT_WHITE);
  
  // Create an empty sprite to render the foreground on
  foreground.createSprite(137, 25);
  foreground.setSwapBytes(true);
  foreground.fillSprite(TFT_WHITE);
  
  // Create an empty sprite to render the character on
  sicko.createSprite(71, 64);
  sicko.setSwapBytes(true);
  sicko.fillSprite(TFT_WHITE);

  // Create an empty sprite to serve as the render frame to push to the tft
  frame_render.createSprite(135, 240);
  frame_render.setSwapBytes(true);
  frame_render.fillSprite(TFT_WHITE);

}

void setup_game(){
  tft.fillScreen(bckgrndclr);
  
  tft.fillCircle(10,265,80,frntclr);
  tft.fillCircle(125,265,80,frntclr);
  tft.setTextColor(bckgrndclr, frntclr);
  tft.drawString("Eat",20,193,2);
  tft.drawString("Rub",20,207,2);
  tft.drawString("Feed",17,220,2);
  tft.drawString("Slap",92,193,2);
  tft.drawString("Fun",94,207,2);
  tft.drawString("Sleep",90,220,2);

  tft.drawString("HE",6,5);
  tft.drawString("SL",6,15);
  tft.drawString("ER",71,5);
  tft.drawString("BB",71,15);
  tft.drawRect(21,5,42,8,frntclr);
  tft.drawRect(21,15,42,8,frntclr);
  tft.drawRect(88,5,42,8,frntclr);
  tft.drawRect(88,15,42,8,frntclr);
  statsSetup();
}

// This function is meant as a routine for the start menu
void waitForStart(){
  // if pressed left button, increase size of left ball, till 18
  if(digitalRead(35)==0){
    if(startMeter1 <=18){
      startMeter1+=1;
    }
  }else{
    if(startMeter1 >=1){
      startMeter1-=1;
    }
  }
  // if pressed left button, increase size of left ball, till 18
  if(digitalRead(0)==0){
    if(startMeter2 <=18){
      startMeter2+=1;
    }
  }else{
    if(startMeter2 >=1){
      startMeter2-=1;
    }
  }
  // if pressed long enough, skip start menu
  if(startMeter1>=18 && startMeter2>18){
    waitStart=false;
    gameRun=true;
    setup_game();
    return;
  }
  frame_render.pushImage(0,0,135,240,title_scrn);
  frame_render.pushImage(54,111,27,36,goop_baby[goop_baby_cycle]);
  frame_render.fillRect(1,220-(startMeter2),27,1+(startMeter2),sick_green2);
  frame_render.fillRect(108,220-(startMeter1),27,1+(startMeter1),sick_green2);
  frame_render.fillEllipse(13, 218-(startMeter2), 13, 7,  sick_green);
  frame_render.fillEllipse(121, 218-(startMeter1), 13, 7, sick_green);
  //frame_render.fillCircle(1,211,20,sick_green);
  //frame_render.fillCircle(108,211,20,sick_green);
  if(goop_baby_cycle<=6){
    goop_baby_cycle+=1;
  }else{
    goop_baby_cycle=0;
  }
}

void menuChangeDarken(uint16_t varx,uint16_t vary){
  tft.setTextColor(varx,vary);
  if(pos==1){
    tft.drawString("Eat",20,193,2);
  }else if(pos==2){
    tft.drawString("Slap",92,193,2);
  }else if(pos==3){
    tft.drawString("Rub",20,207,2);
  }else if(pos==4){
    tft.drawString("Fun",94,207,2);
  }else if(pos==5){
    tft.drawString("Feed",17,220,2);
  }else if(pos==6){
    tft.drawString("Sleep",90,220,2);
  }
}

void statsSetup(){
  tft.fillRect(22,6,40,6,bckgrndclr);
  tft.fillRect(22,16,40,6,bckgrndclr);
  tft.fillRect(89,6,40,6,bckgrndclr);
  tft.fillRect(89,16,40,6,bckgrndclr);
  tft.fillRect(22,6,bar_health,6,TFT_WHITE);
  tft.fillRect(22,16,bar_sleep,6,TFT_WHITE);
  tft.fillRect(89,6,bar_errection,6,TFT_WHITE);
  tft.fillRect(89,16,bar_blueball,6,TFT_WHITE);
}

void leveIncreaser(){
  if(pos==1){
    bar_health++;
  }else if(pos==2){
    bar_health+=-20;
    bar_errection+=10;
  }else if(pos==3){
    bar_blueball++;
  }else if(pos==4){
    bar_errection++;
  }else if(pos==5){
    bar_blueball+=10;
  }else if(pos==6){
    bar_sleep++;
  }
}

void menuChange(){
  if(digitalRead(35)==0 && menu_pos<6 && digitalRead(0)==1){
    if(debounce==0){
      debounce=1;
      menuChangeDarken(bckgrndclr,frntclr);
      menu_pos+=1;
      menuChangeDarken(buttonclr,frntclr);
    }
  }else debounce=0;
  if(digitalRead(0)==0 && menu_pos>1 && digitalRead(35)==1){
    if(debounce2==0){
      debounce2=1;
      menuChangeDarken(bckgrndclr,frntclr);
      menu_pos-=1;
      menuChangeDarken(buttonclr,frntclr);
    }
  }else debounce2=0;
  if(digitalRead(0)==0 && digitalRead(35)==0){
    if(debounce2==0 && debounce==0){
      debounce=1;
      debounce2=1;
      leveIncreaser();
      statsSetup();
      menuChangeDarken(0xFFFF,frntclr);
    }
  }else{
    debounce=0;
    debounce2=0;
  }
}




void draw_sicko(){
  if(count<=6){
    count+=1;
  }else if(count>=1){
    count-=1;
  }
  sicko.pushImage(0,0,71,64,Splinter[count]);
  sicko.pushSprite(30,150);
}



void loop() {
  while(waitStart==true){
    if(clck>=clck_draw){
      waitForStart();
      frame_render.pushSprite(0,0);
      clck=0;
    }
    clck+=1;
  }while(gameRun==true){
    //tft.fillScreen(TFT_BLACK);
    if(clck>=clck_draw){
      if(stats_switch=true){
        statsSetup();
        stats_switch=false;
      }
      menuChange();
      draw_sicko();
      clck=0;
    }
    clck+=1;
  }
  
}
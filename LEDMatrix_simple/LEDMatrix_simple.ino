/*
 Quick LedMatrix example by Pawel A. Hernik
 
 ARDUINO PINOUT:
 Pin 13 - DataIn
 Pin 12 - CLK
 Pin 11 - LOAD/CS
 
 DIGISPARK ATTINY85 PINOUT:
 Pin 2 - DataIn
 Pin 1 - CLK
 Pin 0 - LOAD/CS
*/

#define NUM_MAX 4

#include "Arduino.h"

#if defined( __AVR_ATtinyX5__ )
#define DIN_PIN 2
#define CS_PIN  1
#define CLK_PIN 0
#else
#define DIN_PIN 13
#define CS_PIN  12
#define CLK_PIN 11
#endif

#include "max7219.h"
#include "fonts.h"

int dx=0;
int dy=0;
unsigned long cnt = 0;
unsigned long start;

// ----------------------------------
void setup() 
{
  Serial.begin (9600);
  initMAX7219();
  sendCmdAll(CMD_SHUTDOWN,1);
  sendCmdAll(CMD_INTENSITY,0);
//  clr();
//  showDigit(2, 0, dig6x8);
//  showDigit(3, 8, dig6x8);
//  showDigit(4, 16, dig6x8);
//  showDigit(5, 24, dig6x8);
//  refreshAll();
//  delay(2000);
//  cnt = random(0,9999) * (long)random(0,9999);
  cnt = 12345678;
  start = millis();
}

// ----------------------------------

byte dig[8]={0};
byte digold[8]={0};
byte digtrans[8]={0};
byte digpos4[4]={1,9,17,25};
byte digpos6[6]={1,6,11,16,21,26};
byte digpos8[8]={0,4,8,12,16,20,24,28};
byte del=0;
byte num;
byte *digpos;
const uint8_t *fnt;
int mode=0;

void loop()
{
  clr();
  showDigit(3, 24,  monster);
  refreshAll();
  clr();
  delay(500);
  showDigit(3, 16,  monster);
  refreshAll();
  clr();
  delay(500);
  showDigit(3, 8, monster);
  refreshAll();
  clr();
  delay(500);
  showDigit(3, 0, monster);
  refreshAll();
  delay(500);
  clr();
  /*if( millis() - start > 6000 && del == 0) {
    start = millis();
    mode++;
    if(mode==4) mode=0;
  }
  if(mode==0) {
  fnt = dig6x8;
  digpos = digpos4;
  num = 4;
  counter();
  } else
  if(mode==1) {
  fnt = dig4x8;
  digpos = digpos6;
  num = 6;
  counter();
  } else
  if(mode==2) {
  fnt = dig3x8;
  digpos = digpos8;
  num = 8;
  counter();
  } else printStringWithShift(" Scroll test: 0123456789 abcdefghijk ABCDEFGHIJK ", 32);*/
  printStringWithShift("JOYEUX NOEL - Oh! Oh! Oh! - JOYEUSES FETES  ", 64);
}


void counter()
{
  int i;
  if(del==0) {
    long v = cnt;
    for(i=0; i<num; i++) digold[i] = dig[i];
    for(i=0; i<num; i++) {
      dig[num-1-i] = v % 10;
      v /= 10;
    }
    del=12;
    cnt++;
    for(i=0; i<num; i++)  digtrans[i] = (dig[i]==digold[i]) ? 0 : 10;
  } else
    del--;
  
  clr();
  for(i=0; i<num; i++) {
    if(digtrans[i]==0) {
      dy=0;
      showDigit(dig[i], digpos[i], fnt);
    } else {
      dy=10-digtrans[i];
      showDigit(digold[i], digpos[i], fnt);
      dy=-digtrans[i];
      showDigit(dig[i], digpos[i], fnt);
      digtrans[i]--;
    }
  }
  refreshAll();
  delay(40);
}

// ----------------------------------

void showDigit(char ch, int col, const uint8_t *data)
{
  if(dy<-8 | dy>8) return;
  int len = pgm_read_byte(data);
  int w = pgm_read_byte(data + 1 + ch * len);
  col += dx;
  for (int i = 0; i < w; i++)
    if(col+i>=0 && col+i<32) {
      byte v = pgm_read_byte(data + 1 + ch * len + 1 + i);
      if(!dy) scr[col + i] = v; else scr[col + i] |= dy>0 ? v>>dy : v<<-dy;
    }
}

void setCol(int col, byte v)
{
  if(dy<-8 | dy>8) return;
  col += dx;
  if(col>=0 && col<32)
    if(!dy) scr[col] = v; else scr[col] |= dy>0 ? v>>dy : v<<-dy;
}

int showChar(char ch, const uint8_t *data)
{
  int len = pgm_read_byte(data);
  int i,w = pgm_read_byte(data + 1 + ch * len);
  for (i = 0; i < w; i++)
    scr[NUM_MAX*8 + i] = pgm_read_byte(data + 1 + ch * len + 1 + i);
  scr[NUM_MAX*8 + i] = 0;
  return w;
}

int dualChar = 0;

unsigned char convertPolish(unsigned char _c)
{
  unsigned char c = _c;
  if(c==196 || c==197 || c==195) {
    dualChar = c;
    return 0;
  }
  if(dualChar) {
    switch(_c) {
      case 133: c = 1+'~'; break; // 'ą'
      case 135: c = 2+'~'; break; // 'ć'
      case 153: c = 3+'~'; break; // 'ę'
      case 130: c = 4+'~'; break; // 'ł'
      case 132: c = dualChar==197 ? 5+'~' : 10+'~'; break; // 'ń' and 'Ą'
      case 179: c = 6+'~'; break; // 'ó'
      case 155: c = 7+'~'; break; // 'ś'
      case 186: c = 8+'~'; break; // 'ź'
      case 188: c = 9+'~'; break; // 'ż'
      //case 132: c = 10+'~'; break; // 'Ą'
      case 134: c = 11+'~'; break; // 'Ć'
      case 152: c = 12+'~'; break; // 'Ę'
      case 129: c = 13+'~'; break; // 'Ł'
      case 131: c = 14+'~'; break; // 'Ń'
      case 147: c = 15+'~'; break; // 'Ó'
      case 154: c = 16+'~'; break; // 'Ś'
      case 185: c = 17+'~'; break; // 'Ź'
      case 187: c = 18+'~'; break; // 'Ż'
      default:  break;
    }
    dualChar = 0;
    return c;
  }    
  switch(_c) {
    case 185: c = 1+'~'; break;
    case 230: c = 2+'~'; break;
    case 234: c = 3+'~'; break;
    case 179: c = 4+'~'; break;
    case 241: c = 5+'~'; break;
    case 243: c = 6+'~'; break;
    case 156: c = 7+'~'; break;
    case 159: c = 8+'~'; break;
    case 191: c = 9+'~'; break;
    case 165: c = 10+'~'; break;
    case 198: c = 11+'~'; break;
    case 202: c = 12+'~'; break;
    case 163: c = 13+'~'; break;
    case 209: c = 14+'~'; break;
    case 211: c = 15+'~'; break;
    case 140: c = 16+'~'; break;
    case 143: c = 17+'~'; break;
    case 175: c = 18+'~'; break;
    default:  break;
  }
  return c;
}

// ---------------------------------------------
void printCharWithShift(unsigned char c, int shift_speed) {
  c = convertPolish(c);
  if (c < ' ' || c > '~'+22) return;
  c -= 32;
  int w = showChar(c, font);
  for (int i=0; i<w+1; i++) {
    delay(shift_speed);
    scrollLeft();
    //refreshAll();
    refreshAll_New();
  }
}

// ---------------------------------------------
void printStringWithShift(char* s, int shift_speed){
  while (*s) {
    printCharWithShift(*s, shift_speed);
    s++;
  }
}

void printString(char* s){
  while (*s) {
    printChar(*s);
    s++;
  }
}

void printChar(unsigned char c){
  c = convertPolish(c);
  if (c < ' ' || c > '~'+22) return;
  c -= 32;
  int w = showChar(c, font);
  scrollLeft();
  refreshAll();
  /*for (int i=0; i<w+1; i++) {
    delay(shift_speed);
    scrollLeft();
    refreshAll();
  }*/
}


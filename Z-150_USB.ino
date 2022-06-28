#include <Keyboard.h>
#include "Remote.h"
#include "codes.h"

#define XTCLK 7
#define XTDATA 8
#define XTRESET 6

bool numLockOn;
bool leftShiftDown;
bool rightShiftDown;
bool altDown;

void setup() {
  Keyboard.begin();
  Keyboard.releaseAll();
  numLockOn = false;
  leftShiftDown = false;
  rightShiftDown = false;
  altDown = false;
  pinMode(2, INPUT_PULLUP);
  pinMode(XTCLK,INPUT_PULLUP);
  pinMode(XTDATA, INPUT);
  pinMode(XTRESET, OUTPUT);
  pinMode(LED_BUILTIN_TX,INPUT); //setting these to input stops the onboard LEDs from flashing with every keypress
  pinMode(LED_BUILTIN_RX,INPUT);
  
  attachInterrupt(digitalPinToInterrupt(XTCLK),handleXT,FALLING);
  
  digitalWrite(XTRESET, LOW); //hard reset board
  delay(50);
  digitalWrite(XTRESET,HIGH);
}

void loop() {}

unsigned char scancode;
unsigned char last = 0;
int clkcount = 0;

const char numpad_switch_table[][2] = {
  {KEY_UP_ARROW,KEY_KP_8},    //8 -> up
  {KEY_F11,KEY_KP_MINUS},     //numpad minus -> F11
  {KEY_LEFT_ARROW,KEY_KP_4},  //4 -> left
  {KEY_RIGHT_ARROW,KEY_KP_6}, //6 -> right
  {KEY_F12,KEY_KP_PLUS},      //numpad plus -> F12
  {KEY_DOWN_ARROW,KEY_KP_2}   //2 -> down
};

int findInTable(unsigned char code, bool numLockOn){
  for(int i = 0; i < 6; i++){
    if(numpad_switch_table[i][1] == code){
      return numpad_switch_table[i][numLockOn];
    }
  }
  return code;
}

void processCode(){
    if(scancode == 0x2A){ leftShiftDown = true;} //track if left shift is being held
    else if(scancode == 0xAA){ leftShiftDown = false;}

    if(scancode == 0x36){ rightShiftDown = true;} //track if right shift is being held
    else if(scancode == 0xB6){ rightShiftDown = false;}
    
    if(scancode == 0x38){ altDown = true;} //track if alt is being held, this board only has one alt key
    else if(scancode == 0xB8){ altDown = false;}
    
    if(scancode == 0x45){numLockOn = !numLockOn;} //track state of num lock

    if((leftShiftDown || rightShiftDown) && altDown && !numLockOn){ //If shift and alt are held, and numlock is not on
      Keyboard.release(KEY_LEFT_SHIFT); //release all buttons, VLC recognizes 'shift+alt+next' as different from just 'next' 
      Keyboard.release(KEY_RIGHT_SHIFT);
      Keyboard.release(KEY_LEFT_ALT);
      switch(scancode){
        case 0x48: //numpad up -> volume up
          Remote.increase();
          Remote.clear();
          break;
        case 0x4B: //numpad left -> previous
          Remote.previous(); 
          Remote.clear();
          break;
        case 0x4C: //numpad middle -> play/pause
          Remote.playpause(); 
          Remote.clear();
          break;
        case 0x4D: //numpad right -> next
          Remote.next(); 
          Remote.clear();
          break;
        case 0x50: //numpad down -> volume down
          Remote.decrease(); 
          Remote.clear();
          break;
        default:
          if(scancode&0x80){
            Keyboard.release(findInTable(usbcodes[scancode&~0x80],numLockOn));
          }else{
            Keyboard.press(findInTable(usbcodes[scancode],numLockOn));
          }
      }
      if(leftShiftDown){Keyboard.press(KEY_LEFT_SHIFT);} //re-press all buttons to avoid desync
      if(rightShiftDown){Keyboard.press(KEY_RIGHT_SHIFT);}
      Keyboard.press(KEY_LEFT_ALT);
    }else{
      if(scancode == last){ //block repeated codes, fixes some weird behavior with held keys 
        scancode = 0;       //OS (or at least windows) automatically holds key until released, repeated "press" commands are unnecessary 
        clkcount = 0;       //only in else block, to allow volume keys to be held
        return;
      }
      if(scancode&0x80){
        Keyboard.release(findInTable(usbcodes[scancode&~0x80],numLockOn));
      }else{
        Keyboard.press(findInTable(usbcodes[scancode],numLockOn));
      }
    }
}

void handleXT(){
  switch(clkcount){
  case 0: //ignore start bit
    clkcount++;
    break;
  case 8:
    scancode += digitalRead(XTDATA) << 7;
    processCode();
    last = scancode;
    scancode = 0;
    clkcount = 0;
    break;
  default:
    scancode += digitalRead(XTDATA) << (clkcount-1); //add bit
    clkcount++;
    break;
  }
}

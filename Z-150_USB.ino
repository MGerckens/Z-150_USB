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

void handleXT(){
  
  switch(clkcount){
  case 0: //ignore start bit
    clkcount++;
    break;
  case 8:
    scancode += digitalRead(XTDATA) << 7;
    
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
          if(scancode&0x80){Keyboard.release(usbcodes[scancode&~0x80]);}
          else{Keyboard.press(usbcodes[scancode]);}
      }
      if(leftShiftDown){Keyboard.press(KEY_LEFT_SHIFT);} //re-press all buttons to avoid desync
      if(rightShiftDown){Keyboard.press(KEY_RIGHT_SHIFT);}
      Keyboard.press(KEY_LEFT_ALT);
      
    }else{
      if(scancode == last){ //block repeated codes, fixes some weird behavior with held keys (only in else block, to allow volume keys to be held)
        scancode = 0;
        clkcount = 0;
        return;
      }
      switch(scancode){
        case 0x4A: //numpad '-' -> F11
          Keyboard.press(KEY_F11);
          break;
        case 0xCA:
          Keyboard.release(KEY_F11);
          break;
        case 0x4E: //numpad '+' -> F12
          Keyboard.press(KEY_F12);
          break;
        case 0xCE:
          Keyboard.release(KEY_F12);
          break;
        case 0x37: //'*' -> windows key
          Keyboard.press(KEY_LEFT_GUI);
          break;
        case 0xB7:
          Keyboard.release(KEY_RIGHT_GUI);
          break;
        default:
          if(scancode&0x80){Keyboard.release(usbcodes[scancode&~0x80]);}
          else{Keyboard.press(usbcodes[scancode]);}
      }
    }
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

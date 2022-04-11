#include <Keyboard.h>

//yellow to +5, orange to gnd
#define XTCLK 7 //black
#define XTDATA 8 //brown
#define XTRESET 6 //red
//PIN 9 ON MY PRO MICRO IS BROKEN :pain:

volatile int clkcount = 0;

void setup() {
  Keyboard.begin();
  Keyboard.releaseAll();
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

volatile unsigned char scancode;
volatile unsigned char last = 0;

void handleXT(){
  
  switch(clkcount){
  case 0:
    clkcount++;
    break;
  case 8:
    scancode += digitalRead(XTDATA) << 7;
    if(scancode == last){ //block repeated codes, fixes some weird behavior with held keys
      scancode = 0;
      clkcount = 0;
      return;
    }
    switch(scancode){
      case 0x37: //map "*" to windows key
        Keyboard.press(KEY_LEFT_GUI);
        break;
      case 0xB7:
        Keyboard.release(KEY_LEFT_GUI);
        break;
      default:
        if(scancode&0x80){Keyboard.release(usbcodes[scancode&~0x80]);}
        else{Keyboard.press(usbcodes[scancode]);}
    }
    last = scancode;
    scancode = 0;
    clkcount = 0;
    break;
  default:
    scancode += digitalRead(XTDATA) << (clkcount-1);
    clkcount++;
    break;
  }
}

#include <Keyboard.h>

const char usbcodes[]{
  '0',
  KEY_ESC,
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  '0',
  '-',
  '=',
  KEY_BACKSPACE,
  KEY_TAB,
   'q',
   'w',
   'e',
   'r',
   't',
   'y',
   'u',
   'i',
   'o',
   'p',
   '[',
   ']',
   KEY_RETURN,
   KEY_LEFT_CTRL,
   'a',
   's',
   'd',
   'f',
   'g',
   'h',
   'j',
   'k',
   'l',
  ';',
  '\'',
  '`',
  KEY_LEFT_SHIFT,
  '\\',
  'z',
  'x',
  'c',
  'v',
  'b',
  'n',
  'm',
  ',',
  '.',
  '/',
  KEY_RIGHT_SHIFT,
  '*',
  KEY_LEFT_ALT,
  ' ',
  KEY_CAPS_LOCK,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  219,
  207,
  231,
  232,
  233,
  222,
  228,
  229,
  230,
  223,
  225,
  226,
  227,
  234,
  235  
};

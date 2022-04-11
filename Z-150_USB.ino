#include <Keyboard.h>

#define XTCLK 7 
#define XTDATA 8 
#define XTRESET 6 

volatile int clkcount = 0;

void setup() {
  Keyboard.begin();
  Keyboard.releaseAll();
  pinMode(XTCLK,INPUT_PULLUP); //Z-150 requires pullup on clk line
  pinMode(XTDATA, INPUT);
  pinMode(XTRESET, OUTPUT);
  
  pinMode(LED_BUILTIN_TX,INPUT); //setting these to input stops the onboard LEDs from flashing with every keypress
  pinMode(LED_BUILTIN_RX,INPUT);
  
  attachInterrupt(digitalPinToInterrupt(XTCLK),handleXT,FALLING);
  
  digitalWrite(XTRESET, LOW); //hard reset keyboard on boot
  delay(50);
  digitalWrite(XTRESET,HIGH);
}

void loop() {}

volatile unsigned char scancode;
volatile unsigned char last = 0;

void handleXT(){
  
  switch(clkcount){
  case 0: //skip start bit
    clkcount++;
    break;
  case 8: //finish and process code
    scancode += digitalRead(XTDATA) << 7; //add last bit
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
    scancode += digitalRead(XTDATA) << (clkcount-1); //add bit
    clkcount++;
    break;
  }
}

const char usbcodes[]{ //maps XT code (set 1) to USB code
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

/*
  ButtonMouseControl

  For Leonardo and Due boards only.

  Controls the mouse from five pushbuttons on an Arduino Leonardo, Micro or Due.

  Hardware:
  - five pushbuttons attached to D2, D3, D4, D5, D6

  The mouse movement is always relative. This sketch reads four pushbuttons,
  and uses them to set the movement of the mouse.

  WARNING: When you use the Mouse.move() command, the Arduino takes over your
  mouse! Make sure you have control before you use the mouse commands.

  created 15 Mar 2012
  modified 27 Mar 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ButtonMouseControl
*/

#include <Mouse.h>
#include <Keyboard.h>

// set pin numbers for the five buttons:




void setup() {
  // initialize the buttons' inputs:
  // initialize mouse control:
  Mouse.begin();
  Serial1.begin(115200);
}
char dur=0,zoomlen=0,noPress=0;//0:左右，1：上下
char lastSameFlag='\0';
int moveDelta=0;
unsigned long getmillis=0;
unsigned long pressmillis=0;
unsigned long funmillis=0;
unsigned long lastScroll=0;
void loop() {
  // read the buttons:
  // if X or Y is non-zero, move:
  
  // a delay so the mouse doesn't move too fast:
  while(Serial1.available()){
    char rcv=Serial1.read();
    getmillis=millis();
    if(rcv!='{' && '{' == lastSameFlag) checkzoom();
    if(rcv=='{' && rcv!= lastSameFlag){
      lastSameFlag='{';
      noPress=1;
      lastScroll=millis();
    }
    if(rcv=='}' && rcv!= lastSameFlag){
      if(Mouse.isPressed(MOUSE_LEFT)){
        Mouse.release(MOUSE_LEFT);
      }
      else{
        Mouse.press(MOUSE_LEFT);
      }
      moveDelta=0;
      noPress=1;
      lastSameFlag='}';
    }
    if(rcv=='&' && rcv!= lastSameFlag){
      moveDelta=0;
      lastSameFlag='&';
    }
    if(rcv=='<' && rcv!= lastSameFlag){
      dur=!dur;
      moveDelta=0;
      lastSameFlag='<';
      funmillis=millis();
    }
    if(rcv=='<' && rcv== lastSameFlag && millis()-funmillis>550){
      dur=2;
      moveDelta=0;
      funmillis=0;
    }
    if(rcv=='^'){
      moveDelta--;
      lastSameFlag='^';
    }
    if(rcv=='>'){
      moveDelta++;
      lastSameFlag='>';
    }
    if(!noPress){
    if(rcv==':' && rcv!= lastSameFlag){
      lastSameFlag=':';
      pressmillis=millis();
    }
    if(rcv!=':' && lastSameFlag== ':'){
      if (millis()-pressmillis<550) {
        Mouse.press(MOUSE_LEFT);
        delay(5);
        Mouse.release(MOUSE_LEFT);
      }
      if (millis()-pressmillis>=550 &&millis()-pressmillis<3000) {
        Mouse.press(MOUSE_RIGHT);
        delay(5);
        Mouse.release(MOUSE_RIGHT);
      }
      moveDelta=0;
      lastSameFlag='\0';
    }
    }
  }
  if(millis()-getmillis>130) {
    moveDelta=0;
    noPress=0;
    if(lastSameFlag== ':'){
      if (millis()-pressmillis<550) {
        Mouse.press(MOUSE_LEFT);
        delay(5);
        Mouse.release(MOUSE_LEFT);
      }
      if (millis()-pressmillis>=550 &&millis()-pressmillis<3000) {
        Mouse.press(MOUSE_RIGHT);
        delay(5);
        Mouse.release(MOUSE_RIGHT);
      }
      lastSameFlag='\0';
    }
    if('{' == lastSameFlag) checkzoom();
  }
  if(moveDelta!=0) {
    if(dur==2) {
      int moveDelta2=-1;
      if(moveDelta<0) moveDelta2=1;
      Mouse.move(0,0,moveDelta2);
      delay(300);
      //lastScroll=millis
      while(Serial1.available()) Serial1.read();
    }
    else if(dur==1) Mouse.move(0,moveDelta/2,0);
    else Mouse.move(moveDelta/2,0,0);
  }
  delay(10);
}
void checkzoom(){
      if (millis()-lastScroll<550) {
        Keyboard.begin();
        Keyboard.press(KEY_LEFT_GUI);
        if(zoomlen) Keyboard.press(KEY_ESC);
        else Keyboard.press('=');
        delay(5);
        Keyboard.releaseAll();
        Keyboard.end();
        zoomlen=!zoomlen;
        moveDelta=0;
        noPress=1;
      }
      if (millis()-lastScroll>=550 &&millis()-lastScroll<3000) {
        if(Mouse.isPressed(MOUSE_LEFT)){
          Mouse.release(MOUSE_LEFT);
          delay(990);
        }
        Mouse.press(MOUSE_LEFT);
        delay(10);
        Mouse.release(MOUSE_LEFT);
        Keyboard.begin();
        delay(3000);
        Keyboard.print("your pc password\n");
        Keyboard.releaseAll();
        Keyboard.end();
        moveDelta=0;
        noPress=1;
      }
      lastSameFlag='\0';
    }

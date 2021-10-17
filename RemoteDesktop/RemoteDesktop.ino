//Createen Studios 
//M5Stack Remote Desktop
//版本1.0.1
//add support for M5 faces.

//改编自原作者 お父ちゃん 的原项目 ESP32 ScreenShotReceiver 
//功能是传输电脑截图，并利用UDP进行简易鼠标控制。此项目还需要依赖
//MouseServer.ino -ESP8266接收端
//MouseControl.ino -Arduino Leonardo 鼠标控制器
// 

/*----------------------------------------------------------------------------/
  ESP32 ScreenShotReceiver  

Original Source:  
 https://github.com/lovyan03/ESP32_ScreenShotReceiver/  

Licence:  
 [MIT](https://github.com/lovyan03/ESP32_ScreenShotReceiver/blob/master/LICENSE)  

Author:  
 [lovyan03](https://twitter.com/lovyan03)  

/----------------------------------------------------------------------------*/

#include <esp_wifi.h>
//#include <TFT_eSPI.h>
#include <M5Stack.h>
#if defined(ARDUINO_M5Stack_Core_ESP32) || defined(ARDUINO_M5STACK_FIRE)
#include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater/
#endif

#include "src/TCPReceiver.h"
#define LISTEN_PORT 3333
#define TRANSFER_PORT 5026
#define KEYBOARD_I2C_ADDR     0X08
#define KEYBOARD_INT          5
static LGFX lcd;
static TCPReceiver recv;
static WiFiUDP udp;
static uint32_t remote_port;
static IPAddress remote_ip;
static  uint8_t just_Pressed=1;
uint8_t getFacesVal();

void setup(void)
{
  Serial.begin(115200);
  //Serial.flush();

#if defined ( __M5STACKUPDATER_H )
  M5.begin();
  pinMode(BUTTON_A_PIN ,INPUT_PULLUP);
  pinMode(BUTTON_B_PIN ,INPUT_PULLUP);
  pinMode(BUTTON_C_PIN ,INPUT_PULLUP);
  #ifdef __M5STACKUPDATER_H
    if(digitalRead(BUTTON_A_PIN) == 0) {
       Serial.println("Will Load menu binary");
       updateFromFS(SD);
       ESP.restart();
    }
  #endif
#endif

  lcd.begin();
  lcd.setColorDepth(24);
  lcd.setRotation(0);
  if (lcd.width() < lcd.height())
    lcd.setRotation(1);

  lcd.setFont(&fonts::Font2);

  Serial.println("WiFi begin.");
  lcd.println("WiFi begin.");
  // 記憶しているAPへ接続試行
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin();

  // 接続できるまで10秒待機
  for (int i = 0; WiFi.status() != WL_CONNECTED && i < 100; i++) { delay(100); }

  // 接続できない場合はSmartConfigを起動
  // https://itunes.apple.com/app/id1071176700
  // https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("SmartConfig start.");
    lcd.println("SmartConfig start.");
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.beginSmartConfig();

    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
    WiFi.stopSmartConfig();
    WiFi.mode(WIFI_MODE_STA);
  }

  Serial.println(String("IP:") + WiFi.localIP().toString());
  lcd.println(WiFi.localIP().toString());
    if(digitalRead(BUTTON_C_PIN) == 0) {
      just_Pressed=255;
    }
    else{
  lcd.println("Starting UDP...");

  //启动udp服务，然后等待接收到主机的信息
  udp.begin(LISTEN_PORT);
  while(1){
    while(!udp.parsePacket()) yield();
    char incomingPacket[64];  // 存储Udp客户端发过来的数据
    int len = udp.read(incomingPacket, 63);
    if(len) incomingPacket[len]=0;
    if(strcmp(incomingPacket,"Createen Remote Desktop Control Server")==0) break;
    
    yield();
  }
  lcd.println("UDP set done. Remote IP:");
  remote_ip=udp.remoteIP();
  lcd.println(remote_ip.toString());
  remote_port=udp.remotePort();
  lcd.println("Remote Port:");
  lcd.println(remote_port);
  udp.stop();
  udp.begin(TRANSFER_PORT);
    }
  recv.setup( &lcd );
}

void loop(void)
{
  uint8_t faces_ch=0;
  recv.loop();
  if(just_Pressed!=255){
    just_Pressed=1;
  if(!digitalRead(BUTTON_A_PIN)) {
    if(!digitalRead(BUTTON_C_PIN)) UDPprint('\xad'); // raw <
    else if(!digitalRead(BUTTON_B_PIN)) UDPprint('\xae'); // raw '{'
    else UDPprint('\xaf'); // raw '^'
    just_Pressed=0;
  }
  if(just_Pressed && !digitalRead(BUTTON_B_PIN)) {
    if(!digitalRead(BUTTON_C_PIN)) UDPprint('\xb0'); // raw '}'
    else UDPprint('\xb1'); // raw ':'
    just_Pressed =2;
  }
  if(just_Pressed==1 && !digitalRead(BUTTON_C_PIN)) UDPprint('\xb2'); // raw '>'
  }
  else if((faces_ch=getFacesVal())!=0){
    UDPprint(faces_ch);
  }
}
void UDPprint(char c){
  static uint32_t cmillis=0;
  if(millis()-cmillis<20) return;
  udp.beginPacket(remote_ip,remote_port);
  udp.write(c);
  udp.endPacket();
  cmillis=millis();
}

uint8_t getFacesVal(){
  if(digitalRead(KEYBOARD_INT) == LOW) {
  Wire.requestFrom(KEYBOARD_I2C_ADDR, 1);  // request 1 byte from keyboard
  while (!(Wire.available())) yield();
  return Wire.read();                  // receive a byte as character
  }
  else return 0;
}


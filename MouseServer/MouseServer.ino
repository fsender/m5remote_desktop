//通过UDP收发数据,并把收到的数据
//Using library ESPAsyncUDP.h : https://github.com/me-no-dev/ESPAsyncUDP
#include <ESP8266WiFi.h>
#include "ESPAsyncUDP.h"

const char* ssid = "your wifi account";//wifi账号
const char* password = "your wifi password";//wifi密码
#define LISTEN_PORT 3333

AsyncUDP udp;
char  replyPacket[] = "Createen Remote Desktop Control Server";  // 应答信息
uint8_t broadcasted=0;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if(millis()>20000) ESP.restart();
  }
  Serial.println(" connected");

  //启动udp监听服务
  if(udp.listen(LISTEN_PORT)){
    Serial.print("UDP Listening on IP ");
    Serial.print(WiFi.localIP());
    Serial.print(" , UDP Port ");
    Serial.println(LISTEN_PORT);
        udp.onPacket([](AsyncUDPPacket packet) {
  //解析udp数据包
  int packetSize = packet.length();
  if (packetSize)
  {
    // 收到udp数据包
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    // 读取udp数据包
    const char * dt = (const char *)packet.data();
    if (dt[0]=='\xad'){
      Serial.write('<');
    }
    else if(dt[0]=='\xb2'){
      Serial.write('>');
    }
    else if(dt[0]=='\xaf'){
      Serial.write('^');
    }
    else if(dt[0]=='\xb1'){
      Serial.write(':');
    }
    else if(dt[0]=='\xae'){
      Serial.write('{');
    }
    else if(dt[0]=='\xb0' ){
      Serial.write('}');
    }
    else if (dt[0]=='\''){
      Serial.write('\'');
      Serial.write(dt[1]);
    }
    //向串口调试器打印信息
    //Serial.printf("UDP packet contents %d chars: %s\n", len, incomingPacket);
    //往udp 远端发送应答信息
  }
  
        });
  }
  else ESP.restart();
  //打印本地ip地址，udp client端会使用到
}
void loop()
{
  if(millis()%1000<100 && broadcasted==0){
    udp.broadcast(replyPacket);
    broadcasted=1;
  }
  if(millis()%1000>100 && millis()%1000>200){
    broadcasted=0;
  }
}

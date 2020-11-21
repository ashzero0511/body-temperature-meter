#include <Wire.h> //NFC 통신 모듈
#include <SPI.h>
#include <Adafruit_PN532.h> 
#define PN532_IRQ   (14)
#define PN532_RESET (12)
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
uint32_t cardid;

#include <ESP8266WiFi.h> //와이파이 
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <MFRC522.h>
const char* ssid = "PCU_C_2.4G";
const char* password = "";
char server[] = "leejy0978.dothome.co.kr"; //172.17.3.218
WiFiClient client;

#include <Adafruit_MLX90614.h> //적외선 온도센서
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float temp;
/////////////////////////////////////////////////////////////////////////

void setup(void) {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password); //와이파이 연결
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println(WiFi.localIP());
  
  while (!Serial) delay(10); //NFC연결
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    //Serial.print("Didn't find PN53x board");
    while (1);
  }
  //Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  //Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  //Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  nfc.SAMConfig();
  //Serial.println("Waiting for an ISO14443A Card ...");

  mlx.begin(); //적외선온도센서 연결
 }

//////////////////////////////////////////////////////////////////////////////////////
 
void loop() {
  uint8_t success; //nfc 태그 확인
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    //Serial.println("Found an ISO14443A card");
    //Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    //Serial.print("  UID Value: ");
    //nfc.PrintHex(uid, uidLength);
    if (uidLength == 4)
    {
      cardid = uid[0];
      cardid <<= 8;
      cardid |= uid[1];
      cardid <<= 8;
      cardid |= uid[2];  
      cardid <<= 8;
      cardid |= uid[3]; 
      //Serial.print("cardid = #");
      //Serial.println(cardid);
      //Serial.println("");
     
      temp = floor((mlx.readObjectTempC()+4.5)*10)/10;
      //Serial.println(temp);
      while(temp<32.0){ temp = floor((mlx.readObjectTempC()+4.5)*10)/10; } //온도가 32보다 높을때까지 무한반복
      Serial.print(String(temp)); // tx로 송신
      if(temp<38.0){ Sending_To_php(); } // DB에 값 입력
      delay(6000);//NFC 태그 확인 딜레이
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////

void Sending_To_php() //PHP의 GET 메소드 이용하여 DB에 값 입력
{
  if (client.connect(server, 80)) {
    //Serial.println("connected");
    //Serial.print("GET /project/insert.php?");
    //Serial.print("nfc=");
    //Serial.println(cardid);
    //Serial.print("&temp=");
    //Serial.println(temp);
    
    client.print("GET /project/insert.php?");
    client.print("nfc=");
    client.print(cardid);
    client.print("&temp=");
    client.print(temp);
    client.print(" ");      
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: leejy0978.dothome.co.kr");
    client.println("Content-Type: application/x-www-from-urlencoded");
    client.println("Connection: close");
    client.println();
    client.println();
  }
  else {
    //Serial.println("connection failed");
  }
}
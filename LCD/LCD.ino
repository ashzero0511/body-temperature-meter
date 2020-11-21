#include <U8glib.h>
#include <Wire.h>
#include <Arduino.h>


String str="";

U8GLIB_ST7920_128X64_1X u8g(5, 3, 4);  // SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17

void rev(void)
{
  if(str.length()>=5){
    str = "";
  }
  if(Serial.available()>0)
    {
      char c=Serial.read();
      if(c>=46&&c<=57){
        str+=c;
        Serial.println(str);
      }
      
    }
    delay(1);
}

void draw(void) {
  
  u8g.setFont(u8g_font_unifont);
  
  float temp;
  temp=str.toFloat();
  String tmp = "";
  tmp = str.substring(0,4);
  
  u8g.drawStr( 0, 25, "YourTEMP:" );
  u8g.setPrintPos( 73, 25 );
  u8g.print(tmp);
  u8g.drawStr( 105, 25, "\xb0""C" );
  if(temp>37)
  {
    u8g.drawStr( 50, 48, "Warning");
  }
  else if(temp>30)
  {
    u8g.drawStr( 50, 48, "Pass");
  }
  else
  {
    u8g.drawStr( 50, 48, "Repeat");
  }
}

void setup(void) {
  Serial.begin(115200);
  u8g.begin();
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
}

void loop(void) {
  rev();
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  if(str.length()==5)
  {
    delay(3000);
  }
}
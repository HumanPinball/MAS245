#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>


namespace carrier
{
  namespace pin
  {
    constexpr uint8_t oledDcPower{6};
    constexpr uint8_t oledCs{10};
    constexpr uint8_t oledReset{5};
  }

  namespace oled
  {
    constexpr uint8_t screenWidth{128};
    constexpr uint8_t screenHeight{64};
  }
}

namespace 
{
  CAN_message_t testmelding;
  CAN_message_t mottamelding;
  CAN_message_t retunermelding;

  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
  

  Adafruit_SSD1306 display( 
    carrier::oled::screenWidth,
    carrier::oled::screenHeight,
    &SPI,
    carrier::pin::oledDcPower,
    carrier::pin::oledReset,
    carrier::pin::oledCs);
}

int returnertID;
int returnertLen = 0;
int returnertBuf[7] = {};


void mottaMelding(const CAN_message_t &mottamelding);

void setup()
{
  Serial.begin(9600);
  can0.begin();
  can0.setBaudRate(250000);

  Serial.print(F("float size in bytes: "));
  Serial.println(sizeof(float));


  display.begin(SSD1306_SWITCHCAPVCC); //Gir 3.3V til skjerm
  display.clearDisplay();

  // delay(1000);
  // Serial.println(F("DISPLAY CAN-BUS DETAILS AND IMU Z-VALUE"));
  // display.setTextSize(0);
  // delay(1000);
  // display.clearDisplay();

  display.drawRoundRect(0,0,128,64,3,SSD1306_WHITE);

  for(int i = 0; i < 128; i++) {
    // Beregn y-verdien basert på sinuskurven
    float y = 8 + 16 * sin(2 * PI * i /254);

    // Tegn punktet på skjermen
    display.drawPixel(i, y, SSD1306_WHITE);
  }

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 4);
  display.println(F("MAS245 - Gruppe 12"));
  delay(500);
  display.println("");

  

  can0.enableFIFO();
  can0.enableFIFOInterrupt();
  can0.onReceive(mottaMelding);
  can0.onTransmit(mottaMelding);
}

void statistikk(Adafruit_SSD1306 display)
{
  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,32);
  display.println(F("Can-statistikk"));

}



void sendMelding(CAN_message_t &testmelding)
{
  testmelding.id = 0x001;
  testmelding.len = 3;
  testmelding.buf[0] = 6;
  testmelding.buf[1] = 7;
  testmelding.buf[2] = 89;
  
  Serial.print(testmelding.id);
  Serial.print(testmelding.buf[0]);
  Serial.print(testmelding.buf[1]);
  Serial.println(testmelding.buf[2]);

  can0.write(testmelding);

}

void mottaMelding(const CAN_message_t &mottamelding) // ogsÃ¥ kjent som canSniff
{

  returnertID = mottamelding.id;
  returnertLen = mottamelding.len;
  for (int i = 0; i < mottamelding.len; i++)
    {
      returnertBuf[i] = mottamelding.buf[i];
    }

  Serial.print(mottamelding.id, HEX);
  Serial.print(mottamelding.len);
  Serial.print(mottamelding.buf[0], HEX);
  Serial.print(mottamelding.buf[1], HEX);
  Serial.println(mottamelding.buf[2], HEX);

  

}

void retunerMelding(CAN_message_t &retunermelding)
{
  retunermelding.id = returnertID;
  retunermelding.len = returnertLen;
  for (int i = 0; i < returnertLen; i++)
  {
    retunermelding.buf[i] = returnertBuf[i];
  }

  // Serial.print(returnertID);


  can0.write(retunermelding);
}


  void loop()
  {
    sendMelding(testmelding);
    retunerMelding(retunermelding);
    delay(1000);


    // husk display.display

    display.statistikk(true);
    display.display();
  }



  // SSD1306_BLACK
  // SSD1306_WHITE
  
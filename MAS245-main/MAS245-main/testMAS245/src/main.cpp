#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

int receivedID;
int receivedLen = 0;
int receivedBuf[7] = {};

int receivedMessageCount = 0;
int sentMessageCount = 0;

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
  CAN_message_t sendmsg;
  CAN_message_t receivemsg;
  CAN_message_t returnmsg;

  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
  

  Adafruit_SSD1306 display( 
    carrier::oled::screenWidth,
    carrier::oled::screenHeight,
    &SPI,
    carrier::pin::oledDcPower,
    carrier::pin::oledReset,
    carrier::pin::oledCs);
}

void receiveMsg(const CAN_message_t &receivemsg);

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

 
  can0.enableFIFO();
  can0.enableFIFOInterrupt();
  can0.onReceive(receiveMsg);
  can0.onTransmit(receiveMsg);
}



void displayStatistikk()
{
  display.drawRoundRect(0,0,128,64,3,SSD1306_WHITE);

  for(int i = 0; i < 128; i++) {
    // Beregn y-verdien basert på sinuskurven
    float y = 10 + 8 * sin(2 * PI *i /256);

    // Tegn punktet på skjermen
    display.drawPixel(i, y, SSD1306_WHITE);
  }

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 4);
  display.println(F("MAS245 - Gruppe 12"));
  delay(500);
  display.println("");

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(8,22);
  display.println(F("CAN-statistikk"));

  display.setCursor(10,28);
  display.println("------------------");

  display.setCursor(10,30);


}


void sendM(CAN_message_t &sendmsg)
{
  sendmsg.id = 0x001;
  sendmsg.len = 3;
  sendmsg.buf[0] = 21;
  sendmsg.buf[1] = 22;
  sendmsg.buf[2] = 33;
  sendmsg.buf[3] = 44;
  sendmsg.buf[4] = 55;
  sendmsg.buf[5] = 66;
  sendmsg.buf[6] = 76;
  sendmsg.buf[7] = 89;
  
  
  Serial.print(sendmsg.id);
  Serial.print(sendmsg.buf[0]);
  Serial.print(sendmsg.buf[1]);
  Serial.print(sendmsg.buf[2]);
  Serial.print(sendmsg.buf[3]);
  Serial.print(sendmsg.buf[4]);
  Serial.print(sendmsg.buf[5]);
  Serial.print(sendmsg.buf[6]);
  Serial.println(sendmsg.buf[7]);

  can0.write(sendmsg);

  sentMessageCount++;

}

void receiveMsg(const CAN_message_t &receivemsg) // ogsÃ¥ kjent som canSniff
{

  receivedID = receivemsg.id;
  receivedLen = receivemsg.len;
  for (int i = 0; i < receivemsg.len; i++)
    {
      receivedBuf[i] = receivemsg.buf[i];
    }

  Serial.print(receivemsg.id, HEX);
  Serial.print(receivemsg.len);
  Serial.print(receivemsg.buf[0], HEX);
  Serial.print(receivemsg.buf[1], HEX);
  Serial.println(receivemsg.buf[2], HEX);
  Serial.print(receivemsg.buf[3], HEX);
  Serial.print(receivemsg.buf[4], HEX);
  Serial.println(receivemsg.buf[5], HEX);

  receivedMessageCount++;

  

}

void displayReceivedMessageCount() {

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(9, 33);

  display.print(F("Antall mottatt: "));
  display.println(receivedMessageCount);

}

void displayReceivedMessageID() {

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(9, 41);

  display.print(F("Mottok sist ID: "));
  display.println(receivedID);

}


void retunerMelding(CAN_message_t &returnmsg)
{
  returnmsg.id = receivedID;
  returnmsg.len = receivedLen;
  for (int i = 0; i < receivedLen; i++)
  {
    returnmsg.buf[i] = receivedBuf[i];
  }

  Serial.println(receivedID);


  can0.write(returnmsg);
}


void displaySentMessageCount() {

  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(9, 33);

  display.print(F("Antall sendt: "));
  display.println(sentMessageCount);

}

  void loop()
  {

    sendM(sendmsg);
    retunerMelding(returnmsg);
    

 

    // husk display.display

    // displaySentMessageCount();
    displayReceivedMessageCount();
    displayReceivedMessageID();

    displayStatistikk();
    display.display();
    delay(1000);
    display.clearDisplay();
  }

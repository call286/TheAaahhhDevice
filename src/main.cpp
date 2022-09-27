#include <Arduino.h>

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define DF_RX D7
#define DF_TX D6

SoftwareSerial mySoftwareSerial(DF_RX, DF_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#include <FastLED.h>
#define NUM_LEDS 2
#define LED_DATA_PIN D8

CRGBArray<NUM_LEDS> leds;
CRGB aaahColor = CRGB(255, 213, 0);
uint_fast8_t fractionPart = 0;

void setup()
{
  Serial.begin(115200);
  mySoftwareSerial.begin(9600);

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial))
  { // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); // Set serial communictaion time out 500ms

  //----Set volume----
  myDFPlayer.volume(20);   // Set volume value (0~30).
  myDFPlayer.volumeUp();   // Volume Up
  myDFPlayer.volumeDown(); // Volume Down

  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
  //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
  //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
  //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
  //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

  //----Set device we use SD as default----
  //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
  //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
  //  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);

  //----Mp3 control----
  //  myDFPlayer.sleep();     //sleep
  //  myDFPlayer.reset();     //Reset the module
  //  myDFPlayer.enableDAC();  //Enable On-chip DAC
  //  myDFPlayer.disableDAC();  //Disable On-chip DAC
  //  myDFPlayer.outputSetting(true, 15); //output setting, enable the output and set the gain to 15
  myDFPlayer.next();  //Play next mp3
  //----Read imformation----
  Serial.println(myDFPlayer.readState()); // read mp3 state

  Serial.println("FastLED init");
  FastLED.addLeds<WS2812, LED_DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show(); // Update the display
}

void loop()
{
  EVERY_N_SECONDS(2)
  {
    //----Read imformation----
    Serial.println(myDFPlayer.readState()); // read mp3 state
  }
  EVERY_N_MILLISECONDS(25)
  {
    if (fractionPart <= 255)
    {
      CRGB newColor = blend(CRGB::Black, aaahColor, fractionPart);
      fill_solid(leds, NUM_LEDS, newColor);
      FastLED.show(); // Update the pixel display
      fractionPart += 5;
    }
  }
}
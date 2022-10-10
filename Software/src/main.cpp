#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define DF_BUSY_IN D5
#define DF_TX D6
#define DF_RX D7

#define NUM_LEDS 2
#define LED_DATA_PIN D8

#define INTERVAL_250 250
#define INTERVAL_25 25
#define INTERVAL_1000 1000

unsigned long time_250 = 0;
unsigned long time_25 = 0;
unsigned long time_1000 = 0;

SoftwareSerial mySoftwareSerial(DF_RX, DF_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
boolean DFIsPlaying, DFWasPlayingBefore = false;
void printDetail(uint8_t type, int value);
void handleDFIsPlaying();

struct CRGB
{
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
};

Adafruit_NeoPixel ledstrip = Adafruit_NeoPixel(2, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
CRGB aaahColor = CRGB{(uint8_t)255, (uint8_t)213, (uint8_t)0};
uint8_t ledBrightness = 255;
bool shouldDimDown = false;
bool shouldDimUp = false;

int sensorVal = 0;
float voltage = 0;
const int ANALOG_READ_PIN = A0;

void handleBrightnessSensor();
void ledsDimDown();
void ledsDimUp();
void setLedColor();
void setLedColor(CRGB color);

void setup()
{
  Serial.println("LED init");
  ledstrip.begin();
  ledstrip.setBrightness(255);
  ledstrip.show(); // Initialize all pixels to 'off'

  pinMode(DF_BUSY_IN, INPUT);
  Serial.begin(115200);
  mySoftwareSerial.begin(9600);

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial))
  { // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (!myDFPlayer.waitAvailable(2500))
    {
      delay(10);
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); // Set serial communication time out 500ms

  //----Set volume----
  myDFPlayer.volume(20); // Set volume value (0~30).
  // myDFPlayer.volumeUp();   // Volume Up
  // myDFPlayer.volumeDown(); // Volume Down

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

  handleBrightnessSensor();
  setLedColor();
  ledstrip.show();
}

void loop()
{

  handleDFIsPlaying();

  if (millis() >= time_250 + INTERVAL_250)
  {
    time_250 += INTERVAL_250;
    handleBrightnessSensor();
  }

  if (millis() >= time_25 + INTERVAL_25)
  {
    time_25 += INTERVAL_25;

    ledsDimDown();
    ledsDimUp();

    // Dimming / Brightening the light

    // if (fractionPart <= 255 && DFIsPlaying)
    // {
    //   Serial.println("Dim up");
    //   CRGB newColor = blend(CRGB::Black, aaahColor, fractionPart);
    //   fill_solid(leds, NUM_LEDS, newColor);
    //   FastLED.show(); // Update the pixel display
    //   fractionPart += 5;
    // }
    // if (fractionPart <= 255 && !DFIsPlaying)
    // {
    //   Serial.println("Dim down");
    //   CRGB newColor = blend(aaahColor, CRGB::Black, fractionPart);
    //   fill_solid(leds, NUM_LEDS, newColor);
    //   FastLED.show(); // Update the pixel display
    //   fractionPart += 5;
    // }
  }

  if (millis() >= time_1000 + INTERVAL_1000)
  {
    time_1000 += INTERVAL_1000;
    //   Serial.print("Sensorvalue: ");
    //   // Values from 0-1024
    //   Serial.print(sensorVal);
    //   Serial.print('\t');
    //   Serial.print(", Analog Voltage: ");
    //   Serial.print('\t');
    //   // print the voltage
    //   Serial.println(voltage);
    //   Serial.print("DFIsPlaying: ");
    //   Serial.print(DFIsPlaying);
    //   Serial.print(" - DFWasPlayingBefore: ");
    //   Serial.print(DFWasPlayingBefore);
    //   Serial.print(" - fractionPart: ");
    //   Serial.println(fractionPart);
  }
}

void handleDFIsPlaying()
{
  DFWasPlayingBefore = DFIsPlaying;
  DFIsPlaying = !digitalRead(DF_BUSY_IN);
}

void handleBrightnessSensor()
{
  sensorVal = analogRead(ANALOG_READ_PIN);
  handleDFIsPlaying();

  // Convert the analog reading to voltage
  voltage = sensorVal * (3.3 / 1023.0);

  if (sensorVal < 100) // It's dark
  {
    // Serial.println("It's gotten dark.");
    myDFPlayer.stop();
    shouldDimDown = true;
    shouldDimUp = false;
  }
  else
  {
    if (!DFIsPlaying)
    {
      // myDFPlayer.next();
      myDFPlayer.play(2);
      DFIsPlaying = true;
      shouldDimDown = false;
      shouldDimUp = true;
      Serial.println("Play you f*ck!!!!!!");
    }
  }
}

void ledsDimDown()
{
  if (shouldDimDown)
  {
    if (ledBrightness > 0)
    {
      ledBrightness -= 3;
    }
    uint8_t newR = aaahColor.r * ledBrightness / 255;
    uint8_t newG = aaahColor.g * ledBrightness / 255;
    uint8_t newB = aaahColor.b * ledBrightness / 255;
    setLedColor(CRGB{newR, newG, newB});
    ledstrip.show();
    if (ledBrightness <= 0)
    {
      shouldDimDown = false;
    }
  }
}

void ledsDimUp()
{
  if (shouldDimUp)
  {
    if (ledBrightness < 256)
    {
      ledBrightness += 3;
    }
    uint8_t newR = aaahColor.r * ledBrightness / 255;
    uint8_t newG = aaahColor.g * ledBrightness / 255;
    uint8_t newB = aaahColor.b * ledBrightness / 255;
    setLedColor(CRGB{newR, newG, newB});
    ledstrip.show();
    if (ledBrightness >= 255)
    {
      shouldDimUp = false;
    }
  }
}

void setLedColor()
{
  setLedColor(aaahColor);
}

void setLedColor(CRGB color)
{
  for (uint16_t i = 0; i < ledstrip.numPixels(); i++)
  {
    ledstrip.setPixelColor(i, color.r, color.g, color.b);
  }
}
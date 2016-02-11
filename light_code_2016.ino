#include <Wire.h>
#include "FastLED.h"


FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN_1    3
#define CLK_PIN_1   4
#define DATA_PIN_2    5
#define CLK_PIN_2   6
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS_1    50
#define NUM_LEDS_2    50
#define ASAP_GREEN  0x18FF04
CRGB topleds[NUM_LEDS_1];
CRGB underglow[NUM_LEDS_2];
long index=0;
int bounceIndex=1;
int indexMult=1;
#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

void setup() {
  delay(2000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN_1,CLK_PIN_1,COLOR_ORDER>(topleds, NUM_LEDS_1).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_2,CLK_PIN_2,COLOR_ORDER>(underglow, NUM_LEDS_2).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.setTemperature(UncorrectedTemperature);

  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  clearTopLED();
  clearUnderglowLED();
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {bouncingGreen, theaterChase, solidYellow, solidTopLEDRed};

uint8_t gCurrentPatternNumber =0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'topleds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'topleds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

void nextPattern()
{
  gCurrentPatternNumber++;
  if (gCurrentPatternNumber >= 4)
  {
    gCurrentPatternNumber = 0;
  }
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void clearTopLED()
{
  for (int i = 0; i < NUM_LEDS_1; i++)
  {
    topleds[i] = CRGB::Black;
  }
}

void clearUnderglowLED()
{
  for (int i = 0; i < NUM_LEDS_2; i++)
  {
    underglow[i] = CRGB::Black;
  }
}

void solidYellow()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS_1; i++)
  {
    topleds[i] = CRGB::Yellow;
  }
}

void solidGreen()
{
  FastLED.setBrightness(BRIGHTNESS);
  CRGB foo(5, 100, 5);
  for (int i = 0; i < NUM_LEDS_1; i++)
  {
    topleds[i] = foo;
  }
}

void solidTopLEDRed()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS_1; i++)
  {
    topleds[i] = CRGB::Red;
  }
}

void solidUnderglowRed()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < NUM_LEDS_2; i++)
  {
    underglow[i] = CRGB::Red;
  }
}

void solidBlue()
{
  FastLED.setBrightness(BRIGHTNESS);
  CRGB foo(5, 5, 100);
  for (int i = 0; i < NUM_LEDS_2; i++)
  {
    underglow[i] = foo;
  }
}

void bouncingYellow()
{
  FastLED.setBrightness(BRIGHTNESS);
  if (bounceIndex == 0 || bounceIndex == (NUM_LEDS_1 - 13))
  {
    indexMult *= -1;
  }
  for (int i = bounceIndex; i < (bounceIndex + 13); i++)
  {
    topleds[i] = CRGB::Yellow;
  }
  for (int j = (bounceIndex + 13); j < (bounceIndex + NUM_LEDS_1); j++)
  {
    topleds[j%NUM_LEDS_1] = CRGB::Black;
  }
  bounceIndex += indexMult;
  delay(10);
}

void bouncingGreen()
{
  FastLED.setBrightness(BRIGHTNESS);
  if (bounceIndex == 0 || bounceIndex == (NUM_LEDS_1 - 13))
  {
    indexMult *= -1;
  }
  for (int i = bounceIndex; i < (bounceIndex + 13); i++)
  {
    topleds[i] = ASAP_GREEN;
  }
  for (int j = (bounceIndex + 13); j < (bounceIndex + NUM_LEDS_1); j++)
  {
    topleds[j%NUM_LEDS_1] = CRGB::Black;
  }
  bounceIndex += indexMult;
  delay(10);
}

void cylonRed()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = index; i < (index+13); i++)
  {
      underglow[i%NUM_LEDS_2] = CRGB::Red;
  }
  for (int j = (index+13); j < (index+NUM_LEDS_2); j++)
  {
      underglow[j%NUM_LEDS_2] = CRGB::Black;
  }
  index++;
  delay(20);
}

void cylonBlue()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = index; i < (index+13); i++)
  {
      underglow[i%NUM_LEDS_2] = CRGB::Blue;
  }
  for (int j = (index+13); j < (index+NUM_LEDS_2); j++)
  {
      underglow[j%NUM_LEDS_2] = CRGB::Black;
  }
  index++;
  delay(20);
}

void firstCylon()
{
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = index; i < (index+50); i++)
  {
      topleds[i%NUM_LEDS_1] = CRGB::Red;
  }
  for (int j = (index+50); j < (index+100); j++)
  {
      topleds[j%NUM_LEDS_1] = CRGB::White;
  }
  for (int j = (index+100); j < (index+NUM_LEDS_1); j++)
  {
      topleds[j%NUM_LEDS_1] = CRGB::Blue;
  }
  index++;
  delay(40);
}
void theaterChase()
{
  FastLED.setBrightness(BRIGHTNESS);
    for (int n=0; n<NUM_LEDS_1; n++) {
    if ((n+index) % 3 ==0) {
      topleds[n] = CRGB::Yellow;
    }
    else {
      topleds[n] = CRGB::Black;
    }
  }
  index++;
  delay(100);
}

void fadeRed()
{
  FastLED.setBrightness(cubicwave8(index));
  for (int i = 0; i < NUM_LEDS_2; i++)
  {
    underglow[i] = CRGB::Red;
  }
  index++;
}

void fadeBlue()
{
  FastLED.setBrightness(cubicwave8(index));
  for (int i = 0; i < NUM_LEDS_2; i++)
  {
    underglow[i] = CRGB::Blue;
  }
  index++;
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( topleds, NUM_LEDS_1, gHue, 7);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    topleds[ random16(NUM_LEDS_1) ] += CRGB::White;
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( topleds, NUM_LEDS_1, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    topleds[beatsin16(i+7,0,NUM_LEDS_1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void receiveEvent(int howMany)
{
//  stopStripe = true;
  while(Wire.available()>0)
  {
  gCurrentPatternNumber = (Wire.read()) % ARRAY_SIZE( gPatterns);

  }
}


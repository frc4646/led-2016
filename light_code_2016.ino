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

#define DATA_PIN    3
#define CLK_PIN   4
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define NUM_LEDS    20
CRGB leds[NUM_LEDS];
int index=0;
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(2000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Wire.begin(4);
  Wire.onReceive(receiveEvent);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {cylon, solidGreen, solidBlue, theaterChase, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
//  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


void solidGreen()
{
  CRGB foo(5, 100, 5);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = foo;
  }
}

void solidRed()
{
  CRGB foo(100, 5, 5);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = foo;
  }
}

void solidBlue()
{
  CRGB foo(5, 5, 100);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = foo;
  }
}

void cylon()
{
  int pos = index % NUM_LEDS;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (i >= pos && i < (pos + 5))
    {
      leds[i] = CRGB::Green;
    }
    else
    {
      leds[i] = CRGB::Black;
    }
  }
  index++;
  delay(50);
}
void theaterChase()
{
    for (int n=0; n<NUM_LEDS; n++) {
    if ((n+index) % 3 ==0) {
      leds[n] = CRGB::Green;
    }
    else {
      leds[n] = CRGB::Black;
    }
  }
  index++;
  delay(100);
}



void fade()
{
  FastLED.setBrightness(cubicwave8(index));
  CRGB foo(5, 5, 100);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = foo;
  }
  index++;
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
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


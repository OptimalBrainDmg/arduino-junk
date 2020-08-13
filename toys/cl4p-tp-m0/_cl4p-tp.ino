/**
 * Trinket M0 version of the Prusa talking Cl4p-Tp model.
 * https://blog.prusaprinters.org/how-to-build-a-talking-robot-with-animated-leds_37072/
 * 
 * Uses Serial1 (pins 3 and 4 on the Adafruit Trinket M0) to communicate with DFPLayer Mini
 */

#include <ArduinoLowPower.h>
#include <DFPlayerMini_Fast.h>
#include <Adafruit_NeoPixel.h>

#define NEO_PIN 0
#define INT_PIN 1

#define NUMPIXELS 5

Adafruit_NeoPixel pixels(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

DFPlayerMini_Fast mp3;

volatile boolean talking = false;
volatile boolean boinked = false;

uint16_t tracks = 0;

void boink();


void setup() {

  Serial.begin(115200);

  delay(1000);
  Serial.println("initializing...");

  // initialize the eye
  pixels.setBrightness(0);
  pixels.fill(pixels.Color(0, 200, 150), 0, NUMPIXELS);
  pixels.begin();
  pixels.show();

  // initialize the MP3 player
  Serial1.begin(9600);
  mp3.begin(Serial1);
  mp3.volume(25); 
  tracks = mp3.numSdTracks();
  Serial.print("Found SD Tracks: ");
  Serial.println(tracks);

  // set an isr for when the bot is boinked
  pinMode(INT_PIN, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(INT_PIN), boink, FALLING);

  // cl4p-tp will spend most of his existance snoozing
  snooze();
}

void loop() {
  if (boinked) {
    talk();
  }
  
  if (mp3.isPlaying()) {
    jirojiro();
    delay(100);
  } else {
    snooze();
  }
}

// sets the eye brightness to a random level to modulate with voice
void jirojiro() {
  pixels.setBrightness(random(0,50));
  pixels.fill(pixels.Color(0, 200, 150), 0, NUMPIXELS);
  pixels.show();
}

// wakes up the mp3 player and plays a random track
void talk() {
  if (!talking) {
    talking = true;
    mp3.wakeUp();
    pixels.setBrightness(50);
    pixels.fill(pixels.Color(0, 200, 150), 0, NUMPIXELS);
    pixels.show();
    uint16_t track = random(1, tracks);
    Serial.print("playing track #");
    Serial.print(track);
    Serial.print(" of ");
    Serial.println(tracks);
    mp3.play(track);
    boinked = false;
  }
}

// ISR for button press to boink the bot
void boink() {
  if (!talking && !boinked) {
    boinked = true;
  }
}

// Puts the DFPlayer mini, neopixels, and samd21 into sleep/low-power mode
void snooze() {
  talking = false;
  boinked = false;
  mp3.sleep();
  pixels.setBrightness(0);
  pixels.show();
  LowPower.sleep();
}

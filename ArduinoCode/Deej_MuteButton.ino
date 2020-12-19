#include <OneButton.h>
#include <Adafruit_NeoPixel.h>

const int NUM_SLIDERS = 6;
const int sliderInputs[NUM_SLIDERS] = {A5, A4, A3, A2, A1, A0};
const int buttonInputs[NUM_SLIDERS] = {9, 8, 7, 6, 5, 4};

OneButton buttons[NUM_SLIDERS];
Adafruit_NeoPixel strip(NUM_SLIDERS, 2, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

int analogSliderValues[NUM_SLIDERS];


bool isMuted[NUM_SLIDERS];
uint32_t colors[NUM_SLIDERS] = {strip.Color(255, 255, 255), strip.Color(200, 200, 200), strip.Color(0, 255, 0), strip.Color(255, 130, 0), strip.Color(0, 0, 255), strip.Color(255, 255, 255) };

unsigned long previousMillis = 0;        // will store last Time slider states were sent
const long sendInterval = 50;     

void setup() { 
  
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(sliderInputs[i], INPUT);
    buttons[i] = OneButton(
      buttonInputs[i],   // Input pin for the button
      true,               // Button is active LOW
      true                // Enable internal pull-up resistor
    );
    buttons[i].attachClick(toggleMute, i);
    isMuted[i] = false;
  }
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  
  setLEDs();
  
  Serial.begin(9600);
}

void loop() {
  tickButtons();
  updateSliderValues();
  setLEDs();
  sendSliderValues(); // Actually send data (all the time)
}

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
      analogSliderValues[i] = analogRead(sliderInputs[i]);
   }
}

void tickButtons(){
  for (int i = 0; i < NUM_SLIDERS; i++) {
    buttons[i].tick();
  }
}

void toggleMute(int i) {
  isMuted[i] = !isMuted[i];
}

void setLEDs(){
  for (int i = 0; i < NUM_SLIDERS; i++) {
    if(!isMuted[i]){
      strip.setPixelColor(i, colors[i]);         //  Set pixel's color (in RAM)
    } else {
      strip.setPixelColor(i, strip.Color(255, 0, 0));         //  Set pixel's color (in RAM)
    }
    strip.show();                          //  Update strip to match
  }
}

void sendSliderValues() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sendInterval) {
    previousMillis = currentMillis;
  
    String builtString = String("");
  
    for (int i = 0; i < NUM_SLIDERS; i++) {
      int value = (int)analogSliderValues[i]; 
      
      if (isMuted[i]) {
        value = 0;
      }
      
      builtString += String(value);
      
      if (i < NUM_SLIDERS - 1) {
        builtString += String("|");
      } 
    }
    
    Serial.println(builtString);
  }
}

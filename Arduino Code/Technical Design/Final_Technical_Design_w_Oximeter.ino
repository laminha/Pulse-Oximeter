#include "DHT.h"
#include "MAX30100_PulseOximeter.h"
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int red_light_pin = 11;
int green_light_pin = 10;
int blue_light_pin = 9;
int red2_light_pin = 6;
int green2_light_pin = 5;
int blue2_light_pin = 3;
int randheart;
int randspo2;
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define REPORTING_PERIOD_MS     1000
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PulseOximeter pox;
uint32_t tsLastReport = 0;

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

const unsigned char bitmap [] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  
 

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  displayHeartRate();
  displayOxygenLevel();
  environmentTemperature();
  environmentHumidity();
  onBeatDetected();
  dht.begin();
}

void displayHeartRate(void) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Heart BPM:"));
  display.display();
}

void displayOxygenLevel(void) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(F("Spo2:"));
  display.display();
}

void environmentTemperature(void) {
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(F("Temperature:"));
  display.display();
}

void environmentHumidity(void) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.println(F("Humidity:"));
  display.display();
}

void riskattack(void){
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 50);
  display.println(F("Asthma Risk:"));
  display.display();
}

void onBeatDetected()
{
  display.drawBitmap( 100, 30, bitmap, 28, 28, 1);
  display.display();
}

void buzzersound(){
  int frequency = 5000;
  int buzzer = 2;
  tone(buzzer, frequency, 50);
  delay(1000);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value){
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
  //RGB_color(255,0,0); Red
  //RGB_color(255,100,0); Yellow
  //RGB_color(0,255,0); Green
  //RGB_color(0,0,255); Blue
}

void RGB2_color(int red_light_value, int green_light_value, int blue_light_value){
  analogWrite(red2_light_pin, red_light_value);
  analogWrite(green2_light_pin, green_light_value);
  analogWrite(blue2_light_pin, blue_light_value);
  //RGB2_color(255,0,0); Red
  //RGB2_color(255,100,0); Yellow
  //RGB2_color(0,255,0); Green
  //RGB2_color(0,0,255); Blue
}

void loop() {
  // Initilize the display
  displayHeartRate();
  displayOxygenLevel();
  environmentTemperature();
  environmentHumidity();
  onBeatDetected();
  riskattack();
  delay(1000);

  // Read sensor values & user spo2 and heart rate
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float fahrenheit = (t*1.8)+32;
  pox.update();
  float spo2 = pox.getSpO2();
  float heart = pox.getHeartRate();

  
  //following for heart rate
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(60,0);
  display.println(heart);
  display.display();
  
  //following for spo2
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30,10);
  display.println(spo2);
  display.display();
  
  //following for temperature
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(72, 20);
  display.println(fahrenheit);
  display.display();
  
  //following for degrees fahrenheit
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(102, 20);
  display.println(("F"));
  display.display();
  
  //following for humidity
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(54, 30);
  display.println(h);
  display.display();
  
  //following for percentage
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(86, 30);
  display.println(("%"));
  display.display();
  if (fahrenheit >=72 and h>50){
    if (randspo2 <=95){
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(76, 50);
      display.println(("HIGH"));
      display.display();
    }
    if (randspo2 >95){
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(76, 50);
      display.println(("LOW"));
      display.display();
    }
  }
  else{
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(76, 50);
      display.println(("LOW"));
      display.display();
  }
  if (fahrenheit>=68 or fahrenheit<=72){
    RGB_color(0,255,0);//turns green
  }
  if (fahrenheit<68){
    RGB_color(0,0,255);//turns blue
  }
  if (fahrenheit >72){
    RGB_color(255,0,0);
    buzzersound(); //turns red and buzzer sounds
  }
  if (h>=30 or h<=50){
    RGB2_color(0,255,0);//turns green
  }
  if (h<30){
    RGB2_color(0,0,255); //turns blue
  }
  if (h>50){
    RGB2_color(255,0,0);
    buzzersound(); //turns red and buzzer sounds
  }
  delay(1000);
  display.clearDisplay();
}

#include <Arduino.h>
#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <FastLED.h>
#include <LEDMatrix.h>
#include <FastLED_GFX.h>
#include <Adafruit_MLX90640.h>

// Change the next defines to match your matrix type and size
#define DATA_PIN            12

#define COLOR_ORDER         GRB
#define CHIPSET             WS2812B

#define MATRIX_TILE_WIDTH   5 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  5 // height of each matrix
#define MATRIX_TILE_H       -6  // number of matrices arranged horizontally (negative for reverse order)
#define MATRIX_TILE_V       -7  // number of matrices arranged vertically (negative for reverse order)
#define MATRIX_SIZE         (MATRIX_WIDTH*MATRIX_HEIGHT)
#define MATRIX_PANEL        (MATRIX_WIDTH*MATRIX_HEIGHT)

#define MATRIX_WIDTH        (MATRIX_TILE_WIDTH*MATRIX_TILE_H)
#define MATRIX_HEIGHT       (MATRIX_TILE_HEIGHT*MATRIX_TILE_V)

#define NUM_LEDS            (MATRIX_WIDTH*MATRIX_HEIGHT)

#define MAX_TEMP     50
#define MIN_TEMP     5
//ironbow pallette
DEFINE_GRADIENT_PALETTE( inferno_gp ) {
    0,   0,  0,  0,
    4,   0,  0,  1,
    8,   0,  0,  1,
   12,   0,  0,  4,
   16,   0,  0,  8,
   20,   0,  0, 14,
   24,   0,  0, 22,
   28,   0,  0, 33,
   32,   0,  0, 47,
   36,   0,  0, 63,
   40,   0,  0, 82,
   44,   0,  0,103,
   48,   0,  0,128,
   52,   0,  0,156,
   56,   0,  0,190,
   60,   0,  0,226,
   64,   1,  0,255,
   68,   1,  0,255,
   72,   1,  0,255,
   76,   2,  0,255,
   80,   4,  0,255,
   84,   7,  0,255,
   89,  12,  0,255,
   93,  18,  0,255,
   97,  24,  0,255,
  101,  33,  0,255,
  105,  44,  0,255,
  109,  55,  1,245,
  113,  69,  1,226,
  117,  86,  1,205,
  121, 103,  2,188,
  125, 123,  3,172,
  129, 146,  5,156,
  133, 169,  8,142,
  137, 197, 10,128,
  141, 224, 14,115,
  145, 255, 18,103,
  149, 255, 22, 92,
  153, 255, 27, 82,
  157, 255, 33, 71,
  161, 255, 40, 62,
  165, 255, 46, 53,
  170, 255, 54, 46,
  174, 255, 61, 39,
  178, 255, 70, 33,
  182, 255, 79, 27,
  186, 255, 88, 22,
  190, 255, 99, 18,
  194, 255,109, 14,
  198, 255,121, 11,
  202, 255,135,  8,
  206, 255,147,  5,
  210, 255,161,  3,
  214, 255,175,  2,
  218, 255,189,  1,
  222, 255,205,  1,
  226, 255,221,  1,
  230, 255,237,  1,
  234, 255,255,  1,
  238, 255,255,  5,
  242, 255,255, 26,
  246, 255,255, 72,
  250, 255,255,145,
  255, 255,255,255};

// const uint16_t camColors[] = {0x480F,
// 0x400F,0x400F,0x400F,0x4010,0x3810,0x3810,0x3810,0x3810,0x3010,0x3010,
// 0x3010,0x2810,0x2810,0x2810,0x2810,0x2010,0x2010,0x2010,0x1810,0x1810,
// 0x1811,0x1811,0x1011,0x1011,0x1011,0x0811,0x0811,0x0811,0x0011,0x0011,
// 0x0011,0x0011,0x0011,0x0031,0x0031,0x0051,0x0072,0x0072,0x0092,0x00B2,
// 0x00B2,0x00D2,0x00F2,0x00F2,0x0112,0x0132,0x0152,0x0152,0x0172,0x0192,
// 0x0192,0x01B2,0x01D2,0x01F3,0x01F3,0x0213,0x0233,0x0253,0x0253,0x0273,
// 0x0293,0x02B3,0x02D3,0x02D3,0x02F3,0x0313,0x0333,0x0333,0x0353,0x0373,
// 0x0394,0x03B4,0x03D4,0x03D4,0x03F4,0x0414,0x0434,0x0454,0x0474,0x0474,
// 0x0494,0x04B4,0x04D4,0x04F4,0x0514,0x0534,0x0534,0x0554,0x0554,0x0574,
// 0x0574,0x0573,0x0573,0x0573,0x0572,0x0572,0x0572,0x0571,0x0591,0x0591,
// 0x0590,0x0590,0x058F,0x058F,0x058F,0x058E,0x05AE,0x05AE,0x05AD,0x05AD,
// 0x05AD,0x05AC,0x05AC,0x05AB,0x05CB,0x05CB,0x05CA,0x05CA,0x05CA,0x05C9,
// 0x05C9,0x05C8,0x05E8,0x05E8,0x05E7,0x05E7,0x05E6,0x05E6,0x05E6,0x05E5,
// 0x05E5,0x0604,0x0604,0x0604,0x0603,0x0603,0x0602,0x0602,0x0601,0x0621,
// 0x0621,0x0620,0x0620,0x0620,0x0620,0x0E20,0x0E20,0x0E40,0x1640,0x1640,
// 0x1E40,0x1E40,0x2640,0x2640,0x2E40,0x2E60,0x3660,0x3660,0x3E60,0x3E60,
// 0x3E60,0x4660,0x4660,0x4E60,0x4E80,0x5680,0x5680,0x5E80,0x5E80,0x6680,
// 0x6680,0x6E80,0x6EA0,0x76A0,0x76A0,0x7EA0,0x7EA0,0x86A0,0x86A0,0x8EA0,
// 0x8EC0,0x96C0,0x96C0,0x9EC0,0x9EC0,0xA6C0,0xAEC0,0xAEC0,0xB6E0,0xB6E0,
// 0xBEE0,0xBEE0,0xC6E0,0xC6E0,0xCEE0,0xCEE0,0xD6E0,0xD700,0xDF00,0xDEE0,
// 0xDEC0,0xDEA0,0xDE80,0xDE80,0xE660,0xE640,0xE620,0xE600,0xE5E0,0xE5C0,
// 0xE5A0,0xE580,0xE560,0xE540,0xE520,0xE500,0xE4E0,0xE4C0,0xE4A0,0xE480,
// 0xE460,0xEC40,0xEC20,0xEC00,0xEBE0,0xEBC0,0xEBA0,0xEB80,0xEB60,0xEB40,
// 0xEB20,0xEB00,0xEAE0,0xEAC0,0xEAA0,0xEA80,0xEA60,0xEA40,0xF220,0xF200,
// 0xF1E0,0xF1C0,0xF1A0,0xF180,0xF160,0xF140,0xF100,0xF0E0,0xF0C0,0xF0A0,
// 0xF080,0xF060,0xF040,0xF020,0xF800,};


cLEDMatrix<MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, HORIZONTAL_ZIGZAG_MATRIX, MATRIX_TILE_H, MATRIX_TILE_V, HORIZONTAL_ZIGZAG_BLOCKS> leds;

// const uint16_t colors[] = {
//   matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };

Adafruit_MLX90640 mlx;
static float frame[32*24]; // buffer for full frame of temperatures

uint16_t displayPixelWidth, displayPixelHeight;
CRGBPalette256 tPal = inferno_gp;  
//Returns true if the MLX90640 is detected on the I2C bus
boolean isConnected()
   {
    Wire.beginTransmission((uint8_t)MLX90640_I2CADDR_DEFAULT);
  
    if (Wire.endTransmission() != 0)
       return (false); //Sensor did not ACK
    
    return (true);
   }   


void setup() {
  //while (!Serial) delay(10);
  Serial.begin(115200);
  delay(100);
  Wire.begin();
  Wire.setClock(800000);
  Serial.println("Checking for the MLX90640");
  if (! mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }

  
  Serial.println("Found MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  mlx.setRefreshRate(MLX90640_8_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
  //Prep our screen
  FastLED.addLeds<NEOPIXEL,DATA_PIN>(leds[0], leds.Size()).setCorrection(TypicalSMD5050); 
  FastLED.setBrightness(127);
  FastLED.clear(true);
  delay(500);
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Lime);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.showColor(CRGB::White);
  delay(1000);
  FastLED.clear(true);


}

void loop() {
  uint32_t timestamp = millis();
  if (mlx.getFrame(frame) != 0) {
    Serial.println("Failed");
    return;
  }

  //Go through our camera frame 
  for (uint8_t h=0; h<24; h++) {  
    for (uint8_t w=0; w<32; w++) {
      float t = frame[h*32 + w];
      //Serial.print(t, 1); Serial.print(", ");
      //set temperatures within our bounds
      t = _min(t, MAX_TEMP);
      t = _max(t, MIN_TEMP); 
           
      uint8_t colorIndex = scale8(map(t, MIN_TEMP, MAX_TEMP, 0, 255), 255);
      Serial.print(colorIndex); Serial.print(", "); //Debug
      // Match our color values to a color in our FastLED Pallette
      CRGB camColors = ColorFromPalette(tPal, colorIndex);
      //draw the pixels!
      leds.DrawPixel(h, w, ColorFromPalette(tPal, colorIndex));
    }
  }
  FastLED.show();
  Serial.print((millis()-timestamp) / 2); Serial.println(" ms per frame (2 frames per display)");
  Serial.print(2000.0 / (millis()-timestamp)); Serial.println(" FPS (2 frames per display)");
}
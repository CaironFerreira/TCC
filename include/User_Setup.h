#pragma once

// ===== Driver do display =====
#define ST7789_DRIVER

// ===== Resolução =====
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// ===== Pinos SPI (ESP32) =====
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4     // se ligar RST no 3.3V use -1

// ===== Ajustes ST7789 =====
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_ON

// ===== Frequência do SPI =====
#define SPI_FREQUENCY 40000000

// ===== Fontes =====
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
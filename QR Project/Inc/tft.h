#ifndef TFT_H
#define TFT_H

#include "stm32f4xx_hal.h"

void TFT_Init(void);
void TFT_FillScreen(uint16_t color);
uint16_t TFT_Color565(uint8_t r, uint8_t g, uint8_t b);
void TFT_DrawString(int x, int y, const char *str,uint16_t color, uint16_t bg);


// New QR API:
void TFT_ShowQR(const char *text, int scale);

#endif // TFT_H

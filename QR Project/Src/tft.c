#include "main.h"
#include "tft.h"
#include "qrcodegen.h"
#include "spi.h"

// === your #defines and macros ===
#define TFT_CS_GPIO_Port   GPIOB
#define TFT_CS_Pin         GPIO_PIN_6
#define TFT_DC_GPIO_Port   GPIOB
#define TFT_DC_Pin         GPIO_PIN_1
#define TFT_RST_GPIO_Port  GPIOB
#define TFT_RST_Pin        GPIO_PIN_0

#define TFT_CS_LOW()   HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET)
#define TFT_CS_HIGH()  HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET)
#define TFT_DC_COMMAND() HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET)
#define TFT_DC_DATA()    HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET)
#define TFT_RST_LOW()  HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET)
#define TFT_RST_HIGH() HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET)
// ===== Forward declarations =====
static void TFT_DrawPixel(int x, int y, uint16_t color);
static void TFT_DrawChar(int x, int y, char c, uint16_t color, uint16_t bg);

// === internal static functions (not visible to other files) ===
// 5x7 font for ASCII 32–127
// Full ASCII 5x7 font (characters 0x20 to 0x7E)
static const uint8_t font5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00}, // space
  {0x00,0x00,0x5F,0x00,0x00}, // !
  {0x00,0x07,0x00,0x07,0x00}, // "
  {0x14,0x7F,0x14,0x7F,0x14}, // #
  {0x24,0x2A,0x7F,0x2A,0x12}, // $
  {0x23,0x13,0x08,0x64,0x62}, // %
  {0x36,0x49,0x55,0x22,0x50}, // &
  {0x00,0x05,0x03,0x00,0x00}, // '
  {0x00,0x1C,0x22,0x41,0x00}, // (
  {0x00,0x41,0x22,0x1C,0x00}, // )
  {0x14,0x08,0x3E,0x08,0x14}, // *
  {0x08,0x08,0x3E,0x08,0x08}, // +
  {0x00,0x50,0x30,0x00,0x00}, // ,
  {0x08,0x08,0x08,0x08,0x08}, // -
  {0x00,0x60,0x60,0x00,0x00}, // .
  {0x20,0x10,0x08,0x04,0x02}, // /
  {0x3E,0x51,0x49,0x45,0x3E}, // 0
  {0x00,0x42,0x7F,0x40,0x00}, // 1
  {0x42,0x61,0x51,0x49,0x46}, // 2
  {0x21,0x41,0x45,0x4B,0x31}, // 3
  {0x18,0x14,0x12,0x7F,0x10}, // 4
  {0x27,0x45,0x45,0x45,0x39}, // 5
  {0x3C,0x4A,0x49,0x49,0x30}, // 6
  {0x01,0x71,0x09,0x05,0x03}, // 7
  {0x36,0x49,0x49,0x49,0x36}, // 8
  {0x06,0x49,0x49,0x29,0x1E}, // 9
  {0x00,0x36,0x36,0x00,0x00}, // :
  {0x00,0x56,0x36,0x00,0x00}, // ;
  {0x08,0x14,0x22,0x41,0x00}, // <
  {0x14,0x14,0x14,0x14,0x14}, // =
  {0x00,0x41,0x22,0x14,0x08}, // >
  {0x02,0x01,0x51,0x09,0x06}, // ?
  {0x32,0x49,0x79,0x41,0x3E}, // @
  {0x7E,0x11,0x11,0x11,0x7E}, // A
  {0x7F,0x49,0x49,0x49,0x36}, // B
  {0x3E,0x41,0x41,0x41,0x22}, // C
  {0x7F,0x41,0x41,0x22,0x1C}, // D
  {0x7F,0x49,0x49,0x49,0x41}, // E
  {0x7F,0x09,0x09,0x09,0x01}, // F
  {0x3E,0x41,0x49,0x49,0x7A}, // G
  {0x7F,0x08,0x08,0x08,0x7F}, // H
  {0x00,0x41,0x7F,0x41,0x00}, // I
  {0x20,0x40,0x41,0x3F,0x01}, // J
  {0x7F,0x08,0x14,0x22,0x41}, // K
  {0x7F,0x40,0x40,0x40,0x40}, // L
  {0x7F,0x02,0x0C,0x02,0x7F}, // M
  {0x7F,0x04,0x08,0x10,0x7F}, // N
  {0x3E,0x41,0x41,0x41,0x3E}, // O
  {0x7F,0x09,0x09,0x09,0x06}, // P
  {0x3E,0x41,0x51,0x21,0x5E}, // Q
  {0x7F,0x09,0x19,0x29,0x46}, // R
  {0x46,0x49,0x49,0x49,0x31}, // S
  {0x01,0x01,0x7F,0x01,0x01}, // T
  {0x3F,0x40,0x40,0x40,0x3F}, // U
  {0x1F,0x20,0x40,0x20,0x1F}, // V
  {0x3F,0x40,0x38,0x40,0x3F}, // W
  {0x63,0x14,0x08,0x14,0x63}, // X
  {0x07,0x08,0x70,0x08,0x07}, // Y
  {0x61,0x51,0x49,0x45,0x43}, // Z
};

void TFT_DrawString(int x, int y, const char *str,uint16_t color, uint16_t bg)
{
    while (*str) {
        TFT_DrawChar(x, y, *str, color, bg);
        x += 6;  // 5 pixels + 1 space
        str++;
    }
}
static void TFT_DrawChar(int x, int y, char c, uint16_t color, uint16_t bg)
{
    if (c < 32 || c > 127) return;

    const uint8_t *bitmap = font5x7[c - 32];

    for (int col = 0; col < 5; col++) {
        uint8_t line = bitmap[col];

        for (int row = 0; row < 7; row++) {
            if (line & 0x01)
                TFT_DrawPixel(x + col, y + row, color);
            else
                TFT_DrawPixel(x + col, y + row, bg);

            line >>= 1;
        }
    }
}


static void tft_spi_write(uint8_t d)
{
    SPI1_TransmitByte(d);
}

static void tft_write_cmd(uint8_t cmd)
{
    TFT_CS_LOW();
    TFT_DC_COMMAND();
    tft_spi_write(cmd);
    TFT_CS_HIGH();
}

static void tft_write_data(uint8_t data)
{
    TFT_CS_LOW();
    TFT_DC_DATA();
    tft_spi_write(data);
    TFT_CS_HIGH();
}

static void tft_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    tft_write_cmd(0x2A); // CASET
    TFT_CS_LOW(); TFT_DC_DATA();
    tft_spi_write(0x00); tft_spi_write(x0);
    tft_spi_write(0x00); tft_spi_write(x1);
    TFT_CS_HIGH();

    tft_write_cmd(0x2B); // RASET
    TFT_CS_LOW(); TFT_DC_DATA();
    tft_spi_write(0x00); tft_spi_write(y0);
    tft_spi_write(0x00); tft_spi_write(y1);
    TFT_CS_HIGH();

    tft_write_cmd(0x2C); // RAMWR
}
static void TFT_DrawPixel(int x, int y, uint16_t color)
{
    if (x < 0 || x >= 128 || y < 0 || y >= 160) return;

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    tft_set_addr_window(x, y, x, y);  // 1x1 area

    TFT_CS_LOW();
    TFT_DC_DATA();
    tft_spi_write(hi);
    tft_spi_write(lo);
    TFT_CS_HIGH();
}


static void tft_reset(void)
{
    TFT_RST_LOW();
    HAL_Delay(50);
    TFT_RST_HIGH();
    HAL_Delay(50);
}

// === functions declared in tft.h (no static!) ===
uint16_t TFT_Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void TFT_FillScreen(uint16_t color)
{
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    tft_set_addr_window(0, 0, 127, 159); // 128x160

    TFT_CS_LOW();
    TFT_DC_DATA();
    for (int i = 0; i < 128 * 160; i++) {
        tft_spi_write(hi);
        tft_spi_write(lo);
    }
    TFT_CS_HIGH();
}
void TFT_ShowQR(const char *text, int scale)
{
    // Buffers required by qrcodegen
    uint8_t qr[qrcodegen_BUFFER_LEN_MAX];
    uint8_t temp[qrcodegen_BUFFER_LEN_MAX];

    enum qrcodegen_Ecc ecc = qrcodegen_Ecc_MEDIUM;

    bool ok = qrcodegen_encodeText(text, temp, qr,
                                   ecc,
                                   qrcodegen_VERSION_MIN,
                                   qrcodegen_VERSION_MAX,
                                   qrcodegen_Mask_AUTO,
                                   true);
    if (!ok) return;

    int size = qrcodegen_getSize(qr);          // Number of modules per side
    int qr_pixels = size * scale;

    // Center QR in 128x160
    int offset_x = (128 - qr_pixels) / 2;
    int offset_y = (160 - qr_pixels) / 2;

    // White background
    TFT_FillScreen(TFT_Color565(255, 255, 255));

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {

            bool module = qrcodegen_getModule(qr, x, y);
            uint16_t color = module
                           ? TFT_Color565(0, 0, 0)       // black
                           : TFT_Color565(255, 255, 255); // white

            // Scale each module
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    TFT_DrawPixel(offset_x + x * scale + dx,
                                  offset_y + y * scale + dy,
                                  color);
                }
            }
        }
    }
}

void TFT_Init(void)
{
    tft_reset();

    tft_write_cmd(0x01);         // SWRESET
    HAL_Delay(150);

    tft_write_cmd(0x11);         // SLPOUT
    HAL_Delay(150);

    tft_write_cmd(0x3A);         // COLMOD: 16-bit color
    tft_write_data(0x05);

    tft_write_cmd(0x36);         // MADCTL: memory access control
    tft_write_data(0x00);        // rotation

    tft_write_cmd(0x29);         // DISPON
    HAL_Delay(120);

    // Fill black once after init to confirm panel responds
    TFT_FillScreen(TFT_Color565(0,0,0));
}

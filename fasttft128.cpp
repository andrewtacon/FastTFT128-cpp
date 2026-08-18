#include "pxt.h"

/*
 * Fast native C++ ST7735 driver for the micro:bit MakeCode runtime.
 *
 * 128x128 visible area, RGB565, default 16 MHz SPI.
 * Wiring:
 *   SCK  P13
 *   MOSI P15
 *   DC   P1
 *   CS   P16
 *   RES  3V
 *
 * This is deliberately implemented as a PXT C++ extension. It uses the
 * micro:bit runtime's native SPI transfer path rather than calling the
 * TypeScript pins.spiWrite() operation once per byte.
 */

namespace FastTFT128 {

static const int WIDTH = 128;
static const int HEIGHT = 128;
static const int DC_PIN = MICROBIT_ID_IO_P1;
static const int CS_PIN = MICROBIT_ID_IO_P16;

static const int SWRESET = 0x01;
static const int SLPOUT  = 0x11;
static const int NORON   = 0x13;
static const int INVOFF  = 0x20;
static const int DISPON  = 0x29;
static const int CASET   = 0x2A;
static const int RASET   = 0x2B;
static const int RAMWR   = 0x2C;
static const int MADCTL  = 0x36;
static const int COLMOD  = 0x3A;
static const int FRMCTR1 = 0xB1;
static const int FRMCTR2 = 0xB2;
static const int INVCTR  = 0xB4;
static const int PWCTR1  = 0xC0;
static const int PWCTR2  = 0xC1;
static const int PWCTR3  = 0xC2;
static const int PWCTR4  = 0xC3;
static const int PWCTR5  = 0xC4;
static const int VMCTR1  = 0xC5;
static const int GMCTRP1 = 0xE0;
static const int GMCTRN1 = 0xE1;

static Buffer framebuffer = NULL;
static bool initialized = false;

static void dc(int value) {
    uBit.io.P1.setDigitalValue(value);
}

static void cs(int value) {
    uBit.io.P16.setDigitalValue(value);
}

static void spiByte(uint8_t value) {
    Buffer b = mkBuffer(NULL, 1);
    b->data[0] = value;
    pins::spiTransfer(b, NULL);
}

static void command(uint8_t cmd) {
    dc(0);
    cs(0);
    spiByte(cmd);
    cs(1);
}

static void commandData(uint8_t cmd, const uint8_t *data, int len) {
    dc(0);
    cs(0);
    spiByte(cmd);
    dc(1);
    for (int i = 0; i < len; ++i)
        spiByte(data[i]);
    cs(1);
}

static void setWindow(int x0, int y0, int x1, int y1) {
    dc(0);
    spiByte(CASET);
    dc(1);
    spiByte(0); spiByte(x0); spiByte(0); spiByte(x1);

    dc(0);
    spiByte(RASET);
    dc(1);
    spiByte(0); spiByte(y0); spiByte(0); spiByte(y1);

    dc(0);
    spiByte(RAMWR);
    dc(1);
}

static void begin() {
    pins::spiPins(MICROBIT_ID_IO_P15, MICROBIT_ID_IO_P14, MICROBIT_ID_IO_P13);
    pins::spiFormat(8, 0);
    pins::spiFrequency(16000000);
    cs(1);
    dc(1);

    command(SWRESET);
    fiber_sleep(120);
    command(SLPOUT);
    fiber_sleep(120);

    const uint8_t a[] = {0x01,0x2C,0x2D}; commandData(FRMCTR1,a,3);
    const uint8_t b[] = {0x01,0x2C,0x2D}; commandData(FRMCTR2,b,3);
    const uint8_t c[] = {0x07}; commandData(INVCTR,c,1);
    const uint8_t d[] = {0xA2,0x02,0x84}; commandData(PWCTR1,d,3);
    const uint8_t e[] = {0xC5}; commandData(PWCTR2,e,1);
    const uint8_t f[] = {0x0A,0x00}; commandData(PWCTR3,f,2);
    const uint8_t g[] = {0x8A,0x2A}; commandData(PWCTR4,g,2);
    const uint8_t h[] = {0x8A,0xEE}; commandData(PWCTR5,h,2);
    const uint8_t i[] = {0x0E}; commandData(VMCTR1,i,1);
    command(INVOFF);
    const uint8_t j[] = {0xC8}; commandData(MADCTL,j,1);
    const uint8_t k[] = {0x05}; commandData(COLMOD,k,1);
    fiber_sleep(10);

    const uint8_t cols[] = {0x00,0x00,0x00,0x7F}; commandData(CASET,cols,4);
    const uint8_t rows[] = {0x00,0x00,0x00,0x7F}; commandData(RASET,rows,4);

    const uint8_t gp[] = {0x02,0x1C,0x07,0x12,0x37,0x32,0x29,0x2D,0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10};
    commandData(GMCTRP1,gp,16);
    const uint8_t gn[] = {0x03,0x1D,0x07,0x06,0x2E,0x2C,0x29,0x2D,0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10};
    commandData(GMCTRN1,gn,16);
    command(NORON);
    fiber_sleep(10);
    command(DISPON);
    fiber_sleep(100);
    initialized = true;
}

static void ensureInit() {
    if (!initialized)
        begin();
}

//% color=#275C6B icon="\uf26c" block="Fast TFT 128"
namespace api {

/** Initialise the ST7735 TFT. */
//% block="initialize TFT"
//% weight=100
void init() {
    begin();
}

/** Set the native SPI clock. Start with 16000000. */
//% block="set SPI speed to $hz Hz"
//% hz.min=1000000 hz.max=24000000 hz.defl=16000000
//% weight=98
void setSPISpeed(int hz) {
    ensureInit();
    pins::spiFrequency(hz);
}

/** RGB565 colour from 8-bit RGB values. */
//% block="RGB565 red $red green $green blue $blue"
//% red.min=0 red.max=255 green.min=0 green.max=255 blue.min=0 blue.max=255
//% weight=60
int rgb(int red, int green, int blue) {
    if (red < 0) red = 0; if (red > 255) red = 255;
    if (green < 0) green = 0; if (green > 255) green = 255;
    if (blue < 0) blue = 0; if (blue > 255) blue = 255;
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

/** Clear the complete 128x128 display. */
//% block="clear screen with color $color"
//% color.defl=0
//% weight=95
void clear(int color) {
    ensureInit();
    fillRect(0, 0, WIDTH, HEIGHT, color);
}

/** Fast filled rectangle; one display window and one pixel stream. */
//% block="fast filled rectangle x $x y $y width $width height $height color $color"
//% x.min=0 x.max=127 y.min=0 y.max=127 width.min=1 width.max=128 height.min=1 height.max=128
//% weight=90
void fillRect(int x, int y, int width, int height, int color) {
    ensureInit();
    if (width <= 0 || height <= 0) return;
    int x0=x, y0=y, x1=x+width-1, y1=y+height-1;
    if (x1 < 0 || y1 < 0 || x0 >= WIDTH || y0 >= HEIGHT) return;
    if (x0 < 0) x0=0; if (y0 < 0) y0=0;
    if (x1 >= WIDTH) x1=WIDTH-1; if (y1 >= HEIGHT) y1=HEIGHT-1;

    cs(0);
    setWindow(x0,y0,x1,y1);
    uint8_t hi=(color>>8)&0xff, lo=color&0xff;
    int count=(x1-x0+1)*(y1-y0+1);
    Buffer b=mkBuffer(NULL, count*2);
    for (int n=0,p=0;n<count;++n) { b->data[p++]=hi; b->data[p++]=lo; }
    pins::spiTransfer(b,NULL);
    cs(1);
}

/** Draw one pixel. Use framebuffer functions for many pixels. */
//% block="fast pixel x $x y $y color $color"
//% x.min=0 x.max=127 y.min=0 y.max=127
//% weight=80
void pixel(int x, int y, int color) {
    ensureInit();
    if (x<0 || x>=WIDTH || y<0 || y>=HEIGHT) return;
    cs(0); setWindow(x,y,x,y);
    uint8_t data[2]={(uint8_t)(color>>8),(uint8_t)color};
    Buffer b=mkBuffer(NULL,2); b->data[0]=data[0]; b->data[1]=data[1];
    pins::spiTransfer(b,NULL); cs(1);
}

//% block="fast horizontal line x $x y $y length $length color $color"
//% length.min=1 length.max=128
//% weight=88
void hLine(int x,int y,int length,int color) { fillRect(x,y,length,1,color); }

//% block="fast vertical line x $x y $y length $length color $color"
//% length.min=1 length.max=128
//% weight=87
void vLine(int x,int y,int length,int color) { fillRect(x,y,1,length,color); }

/** Allocate the 32 KB RGB565 framebuffer. */
//% block="create framebuffer"
//% weight=75
void createFramebuffer() {
    ensureInit();
    if (!framebuffer) framebuffer=mkBuffer(NULL, WIDTH*HEIGHT*2);
}

/** Clear framebuffer without sending it to the display. */
//% block="clear framebuffer with color $color"
//% weight=74
void clearFramebuffer(int color) {
    createFramebuffer();
    uint8_t hi=(color>>8)&0xff, lo=color&0xff;
    for(int i=0,p=0;i<WIDTH*HEIGHT;++i){ framebuffer->data[p++]=hi; framebuffer->data[p++]=lo; }
}

/** Set a framebuffer pixel. This does not communicate with the TFT. */
//% block="set framebuffer pixel x $x y $y color $color"
//% x.min=0 x.max=127 y.min=0 y.max=127
//% weight=73
void setPixel(int x,int y,int color) {
    createFramebuffer();
    if(x<0||x>=WIDTH||y<0||y>=HEIGHT)return;
    int p=(y*WIDTH+x)*2; framebuffer->data[p]=(color>>8)&0xff; framebuffer->data[p+1]=color&0xff;
}

/** Fill a framebuffer rectangle without communicating with the TFT. */
//% block="fill framebuffer rectangle x $x y $y width $width height $height color $color"
//% x.min=0 x.max=127 y.min=0 y.max=127 width.min=1 width.max=128 height.min=1 height.max=128
//% weight=72
void fillFramebufferRect(int x,int y,int width,int height,int color) {
    createFramebuffer();
    if(width<=0||height<=0)return;
    int x0=x<0?0:x, y0=y<0?0:y, x1=x+width-1, y1=y+height-1;
    if(x1>=WIDTH)x1=WIDTH-1; if(y1>=HEIGHT)y1=HEIGHT-1;
    if(x0>x1||y0>y1)return;
    uint8_t hi=(color>>8)&0xff, lo=color&0xff;
    for(int yy=y0;yy<=y1;++yy){ int p=(yy*WIDTH+x0)*2; for(int xx=x0;xx<=x1;++xx){ framebuffer->data[p++]=hi; framebuffer->data[p++]=lo; } }
}

/** Transfer the complete 128x128 framebuffer using one native SPI transfer. */
//% block="show framebuffer"
//% weight=70
void show() {
    createFramebuffer();
    cs(0); setWindow(0,0,127,127); pins::spiTransfer(framebuffer,NULL); cs(1);
}

}
}

#include "picopixel.h"
#include "sctp.h"
#include "protocol.h"
#include "logo.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define PANEL_RES_X 64
#define PANEL_RES_Y 64
#define PANEL_CHAIN 1
#define SWAP_BLUE_GREEN 0

#define debugSerial Serial  // 定义调试打印的串口，不需要串口打印则注释这一行
#ifdef debugSerial
#define debugPrint(...) \
  { debugSerial.print(__VA_ARGS__); }
#define debugPrintln(...) \
  { debugSerial.println(__VA_ARGS__); }
#else
#define debugPrint(...) \
  {}
#define debugPrintln(...) \
  {}
#endif

// Module configuration
HUB75_I2S_CFG mxconfig(
	PANEL_RES_X,
	PANEL_RES_Y,
	PANEL_CHAIN
);

MatrixPanel_I2S_DMA *dma_display = nullptr;
static CtrlPkg pkg PROGMEM;

void printCenter(const char *buf, int y) {
  int16_t x1, y1;
  uint16_t w, h;
  dma_display->clearScreen();
  dma_display->getTextBounds(buf, 0, y, &x1, &y1, &w, &h);
  dma_display->setCursor(32 - (w / 2), y);
  dma_display->print(buf);
}

void onUpdateImageReq(UpdateImageReq *req) {
  debugPrintln("on update image req");
  /*
  char tmp[64] = {0};
  sprintf(tmp, "size=%d,w=%d,h=%d", size, req->width, req->height);
  printCenter(tmp, 32);
  */
  dma_display->clearScreen();
  dma_display->drawRGBBitmap(req->x, req->y, req->buff, req->width, req->height);
}

void onDrawPixelReq(DrawPixelReq *req) {
  /*
  // send buff for testing: CC 00 00 0B 00 01 02 00 01 06 E0 07 02 06 64 F8 00
  char tmp[64] = {0};
  sprintf(tmp, "count=%d, x=%d, y=%d, c=%d", req->count, req->pixels[0].x, req->pixels[0].y, req->pixels[0].color);
  printCenter(tmp, 32);
  */
  for(int i = 0; i < req->count; i++) {
    Pixel *pixel = req->pixels + i;
    dma_display->drawPixel(pixel->x, pixel->y, pixel->color);
  }
}

void onClearScreen() {
  dma_display->clearScreen();
}

static int myPacketHandler(SCTPPacket *p) {
  Serial.println("get sctp data");
  if(!p) {
    return -1;
  }
  if(p->port != PORT_CTRL) {
    return -2;
  }
  char text[32] = {0};
  protocolUnpack(p->data, &pkg);
  switch(pkg.cmd) {
  case CMD_UPDATE_IMAGE:
    onUpdateImageReq(&pkg.body.updateImageReq);
    break;
  case CMD_DRAW_PIXEL:
    onDrawPixelReq(&pkg.body.drawPixelReq);
    break;
  case CMD_CLEAR_SCREEN:
    onClearScreen();
    break;
  default:
    break;
  }
  return 0;  
}

void setup() {
  Serial.begin(115200);
  #if SWAP_BLUE_GREEN
  mxconfig.gpio.b1 = 26;
  mxconfig.gpio.b2 = 12;
  mxconfig.gpio.g1 = 27;
  mxconfig.gpio.g2 = 13;
  #endif
  mxconfig.gpio.e = 18;
  mxconfig.clkphase = false;
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); //0-255
  dma_display->setRotation(3); // 0,1,2,3
  dma_display->setFont(&Picopixel);
  dma_display->setTextColor(0xffff);
  
  dma_display->drawRGBBitmap(0, 0, epd_bitmap_logo, 64, 64);
  
  sctpInit(myPacketHandler);
  
  Serial.println("ready to game");
}


void loop() {
  int n = Serial.available();
  if(n > 0) {
    debugPrintln(n);
    uint8_t buff[256] = {0};
    if(n > sizeof(buff)) {
      n = sizeof(buff);
    }
    size_t size = Serial.readBytes(buff, n);
    debugPrint("get data size=");
    debugPrintln(size);
    sctpParseString(buff, size);
  }
}

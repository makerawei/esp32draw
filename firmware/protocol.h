#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <Arduino.h>

#pragma pack(1)

enum tagCmd {
  CMD_UPDATE_IMAGE = 0,
  CMD_DRAW_PIXEL = 1,
  CMD_CLEAR_SCREEN = 2
};

typedef struct {
  uint8_t width;
  uint8_t height;
  uint8_t x;
  uint8_t y;
  uint16_t *buff;
} UpdateImageReq;

typedef struct {
  uint8_t x;
  uint8_t y;
  uint16_t color; // color565
} Pixel;

typedef struct {
  uint16_t count;
  Pixel *pixels;
} DrawPixelReq;

typedef struct {
  unsigned char cmd;
  union {
    UpdateImageReq updateImageReq;
    DrawPixelReq drawPixelReq;
  } body;

} CtrlPkg;

#pragma pack()

bool protocolUnpack(uint8_t *buf, CtrlPkg *pctrlPkg);

#endif

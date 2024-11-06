#include <sys/_stdint.h>
#include "HardwareSerial.h"
#include "protocol.h"

static void parseUpdateImageReq(uint8_t *buf, CtrlPkg *pstCtrlPkg) {
  UpdateImageReq *req = &pstCtrlPkg->body.updateImageReq;
  req->width = buf[1];
  req->height = buf[2];
  req->x = buf[3];
  req->y = buf[4];
  req->buff = (uint16_t *)((char *)buf + 5);
}

static void parseDrawPixelReq(uint8_t *buf, CtrlPkg *pstCtrlPkg) {
  DrawPixelReq *req = &pstCtrlPkg->body.drawPixelReq;
  req->count = *(uint16_t *)(buf + 1);
  req->pixels = (Pixel *)(buf + 3);
}

bool protocolUnpack(uint8_t *buf, CtrlPkg *pstCtrlPkg) {
  if (buf == NULL || pstCtrlPkg == NULL)
    return false;

  pstCtrlPkg->cmd = *buf;
  uint8_t *p = buf + 1;
  switch (pstCtrlPkg->cmd) {
  case CMD_UPDATE_IMAGE:
    parseUpdateImageReq(buf, pstCtrlPkg);
    break;
  case CMD_DRAW_PIXEL:
    parseDrawPixelReq(buf, pstCtrlPkg);
    break;
  case CMD_CLEAR_SCREEN:
    break;
  default:
    break;
  }

  return true;
}

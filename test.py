# -*- coding: utf-8 -*-
import sys
import time
import struct
import serial
from PIL import Image



SCREEN_WIDTH = 64
SCREEN_HEIGHT = 64


def convert_to_rgb565_buffer(image):
    array = []
    for y in range(image.height):
        for x in range(image.width):
            r, g, b = image.getpixel((x, y))
            r = (r >> 3) & 0x1F
            g = (g >> 2) & 0x3F
            b = (b >> 3) & 0x1F
            rgb565 = (r << 11) | (g << 5) | b
            array.append(struct.pack("H", rgb565))

    return b"".join(array)

image = Image.open(sys.argv[1])
width, height = image.size
ratio = width / height
scale_width = SCREEN_WIDTH
scale_height = int(scale_width / ratio)
y_offset = (SCREEN_HEIGHT - scale_height) // 2
print(f"resize {width}x{height} to {scale_width}x{scale_height}, y offset is {y_offset}")
resized_frame = image.resize((scale_width, scale_height))
image_buff = convert_to_rgb565_buffer(resized_frame.convert('RGB'))
image_size = len(image_buff)
size_buff = struct.pack("H", image_size + 5)
print("image size=", image_size)
print("size buff=", size_buff)
y_offset_buff = struct.pack("B", y_offset)
height_buff = struct.pack("B", scale_height)
data = b"\xCC\x00\x00" + size_buff + b"\x00\x40" + height_buff + b"\x00" + y_offset_buff + image_buff + b"\x00"
print(len(data))
#hex_string = data.hex().upper()
#s = ' '.join(hex_string[i:i+2] for i in range(0, len(hex_string), 2))
#print(s)
ser = serial.Serial(port="/dev/cu.usbserial-0001",
                    baudrate=115200,
		    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_NONE,
		    stopbits=serial.STOPBITS_ONE,
		    timeout=1)
print("open success")
time.sleep(1)
n = 1024
for i in range(0, len(data), n):
    print("sending ...", i)
    ser.write(data[i:i+n])
    hex_string = data[i:i+n].hex().upper()
    s = ' '.join(hex_string[i:i+2] for i in range(0, len(hex_string), 2))
    print(s)
    print("===============")
    time.sleep(0.1)

ser.close()

print("send complete")

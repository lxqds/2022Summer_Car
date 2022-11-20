# Untitled - By: 27825 - 周二 8月 16 2022

import sensor
import image
import lcd
import time

import ustruct#导入ustruct模块方便转换数据

from machine import UART
from board import board_info
from fpioa_manager import fm

fm.register(24,fm.fpioa.UART1_TX,force=True)
fm.register(25,fm.fpioa.UART1_RX,force=True)

lcd.init(freq=15000000,invert=1)
clock = time.clock()
lcd.init()
sensor.reset(freq=24000000,dual_buff=True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_hmirror(0)
sensor.set_vflip(0)
sensor.set_auto_gain(False,5)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(True)
sensor.set_brightness(2)
sensor.set_saturation(0)
sensor.run(1)
sensor.skip_frames()

def find_max(blobs):#定义找最大的色块
    max_size = 0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob = blob
            max_size = blob[2]*blob[3]
    return max_blob

def Send_Data(X,Y,D):
    global uart
    data = ustruct.pack("<bbhhfb",#数据包的格式控制
                              0x23,#数据包头部1
                              0x66,#数据包头部2
                              int(X),#数据1
                              int(Y),#数据2
                              float(D),#数据3
                              0x11)#数据包结束
    uart = UART(UART.UART1,115200,8,0,1,timeout=1000,read_buf_len=4096)
    uart.write(data);

green_threshold = (22, 92, -67, -19, -11, 72)
size_threshold = 2000

K=1360
Distance=0
x_stride=10
y_stride=10
area_threshold =150
fps = 0
while True:
    clock.tick()
    img = sensor.snapshot()#获取图片

    blobs = img.find_blobs([green_threshold], x_stride=10, y_stride=10,area_threshold =150)#找色块

    if blobs:
        max_blobs = find_max(blobs)#选择最大的
        for b in blobs:#遍历blobs里的值
            img.draw_rectangle(max_blobs[0:4])
            img.draw_cross(max_blobs[5],max_blobs[6])
            #计算距离
            Lm = (max_blobs[2]+max_blobs[3])/2
            Distance = K/Lm
            if Distance < 150:
                Send_Data(max_blobs[5],max_blobs[6],Distance)
            else:
                Send_Data(-1,-1,-1)
        img.draw_string(0,2,("x0=%s y0=%s Distance=%s\n "%(max_blobs[5],max_blobs[6],Distance)),color = (0,255,255),scale = 1.5)
    else:
        Send_Data(-1,-1,-1)
    img.draw_string(0,24, ("%2.1ffps" %(fps)), color=(255,255,255), scale=2)

    lcd.rotation(0)
    lcd.display(img)
    fps = clock.fps()


#ifndef __LCD_ST7735_H__
#define __LCD_ST7735_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "drivers/spi.h"
#include "drv_spi.h"

/* 0-0 angle|1-90 angle|2-180 angle|-270 angle */
#define USE_HORIZONTAL 1


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

#define u16 uint16_t

#define PKG_ST_7735_SPI_BUS_NAME    "spi0"
#define PKG_ST_7735_SPI_DEVICE_NAME "spi01"
#define PKG_ST_7735_DC_PIN  GET_PIN(11, 4)
#define PKG_ST_7735_RES_PIN  GET_PIN(11, 5)
#define PKG_ST_7735_BLK_PIN  GET_PIN(11, 3)
#define PKG_ST_7735_CS_PIN   GET_PIN(0, 5)


#define LCD_DC_CLR  rt_pin_write(PKG_ST_7735_DC_PIN, PIN_LOW)
#define LCD_DC_SET  rt_pin_write(PKG_ST_7735_DC_PIN, PIN_HIGH)
#define LCD_RES_CLR rt_pin_write(PKG_ST_7735_RES_PIN, PIN_LOW)
#define LCD_RES_SET rt_pin_write(PKG_ST_7735_RES_PIN, PIN_HIGH)
#define LCD_BLK_SET rt_pin_write(PKG_ST_7735_BLK_PIN, PIN_HIGH)
#define DELAY       rt_thread_mdelay




#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458

#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12








rt_err_t spi_lcd_init(uint32_t freq);

#ifdef __cplusplus
}
#endif

#endif

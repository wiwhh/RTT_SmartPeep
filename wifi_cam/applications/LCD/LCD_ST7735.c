#include "LCD_ST7735.h"
#include <rtdbg.h>

static struct rt_spi_device *lcd_dev;

static void LCD_RESET(void)
{
    DELAY(25);
    LCD_RES_CLR;
    DELAY(25);
    LCD_RES_SET;
    DELAY(50);
}

static void LCD_WR_REG(uint8_t reg)
{
    LCD_DC_CLR;
    rt_spi_send(lcd_dev, &reg, 1);
    LCD_DC_SET;
}

static void LCD_WR_DATA(uint8_t data)
{
    LCD_DC_SET;
    rt_spi_send(lcd_dev, &data, 1);
}

static void LCD_WriteData_16Bit(uint16_t Data)
{
    uint8_t buf[2];
    LCD_DC_SET;
    buf[0] = Data >> 8;
    buf[1] = Data & 0xff;
    rt_spi_send(lcd_dev, buf, 2);
}

void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WriteData_16Bit(x1+2);
        LCD_WriteData_16Bit(x2+2);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WriteData_16Bit(y1+1);
        LCD_WriteData_16Bit(y2+1);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WriteData_16Bit(x1);
        LCD_WriteData_16Bit(x2);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WriteData_16Bit(y1);
        LCD_WriteData_16Bit(y2);
        LCD_WR_REG(0x2c);//储存器写
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WriteData_16Bit(x1+1);
        LCD_WriteData_16Bit(x2+1);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WriteData_16Bit(y1+2);
        LCD_WriteData_16Bit(y2+2);
        LCD_WR_REG(0x2c);//储存器写
    }
    else
    {
        LCD_WR_REG(0x2a);//列地址设置
        LCD_WriteData_16Bit(x1+1);
        LCD_WriteData_16Bit(x2+1);
        LCD_WR_REG(0x2b);//行地址设置
        LCD_WriteData_16Bit(y1+2);
        LCD_WriteData_16Bit(y2+2);
        LCD_WR_REG(0x2c);//储存器写
    }
}


static void LCD_Init(void)
{
    LCD_RESET();

    LCD_BLK_SET;//打开背光
    DELAY(100);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); //Sleep out
    DELAY(120);              //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);
    LCD_WR_REG(0xB3);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x3C);
    //------------------------------------End ST7735S Frame Rate---------------------------------//
    LCD_WR_REG(0xB4); //Dot inversion
    LCD_WR_DATA(0x03);
    //------------------------------------ST7735S Power Sequence---------------------------------//
    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x04);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0XC0);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x8D);
    LCD_WR_DATA(0x2A);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x8D);
    LCD_WR_DATA(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_WR_REG(0xC5); //VCOM
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0x36); //MX, MY, RGB mode
    if(USE_HORIZONTAL==0)LCD_WR_DATA(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA(0x70);
    else LCD_WR_DATA(0xA0);
    //------------------------------------ST7735S Gamma Sequence---------------------------------//
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x2E);
    LCD_WR_DATA(0x30);
    LCD_WR_DATA(0x25);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x2E);
    LCD_WR_DATA(0x3A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x13);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x27);
    LCD_WR_DATA(0x27);
    LCD_WR_DATA(0x25);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x3B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x13);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------//
    LCD_WR_REG(0x3A); //65k mode
    LCD_WR_DATA(0x05);
    LCD_WR_REG(0x29); //Display on
}


void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            LCD_WriteData_16Bit(color);
        }
    }
}


void LCD_DrawPicture(uint8_t* picture)
{
    uint16_t i;
    LCD_Address_Set(0, 0, 95, 95);
    for (i = 0; i < 96*96*2; i++)
    {
        LCD_WR_DATA(picture[i]);
    }
}


static void Lcd_pin_init(void)
{
    rt_pin_mode(PKG_ST_7735_DC_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(PKG_ST_7735_RES_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(PKG_ST_7735_BLK_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(PKG_ST_7735_CS_PIN, PIN_MODE_OUTPUT);
}


rt_err_t spi_device_attach(const char *bus_name, const char *device_name, rt_base_t cs_pin)
{
    RT_ASSERT(bus_name != RT_NULL);
    RT_ASSERT(device_name != RT_NULL);

    rt_err_t result = RT_EOK;
    struct rt_spi_device *spi_device;

    /* attach the device to spi bus*/
    spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device));
    RT_ASSERT(spi_device != RT_NULL);

    result=rt_hw_spi_device_attach(bus_name, device_name, cs_pin);
    if (RT_EOK != result)
    {
        LOG_E("%s attach to %s faild, %d\n", device_name, bus_name, result);
    }
    else
    {
        LOG_I("%s attach to %s done", device_name, bus_name);
    }

    return result;
}

static uint16_t color_array[] =
{
    WHITE, BLACK, BLUE, BRED,
    GRED, GBLUE, RED, YELLOW
};

rt_err_t  _spi_lcd_init(void)
{
    rt_err_t res = RT_EOK;
    Lcd_pin_init();
    spi_device_attach(PKG_ST_7735_SPI_BUS_NAME, PKG_ST_7735_SPI_DEVICE_NAME, PKG_ST_7735_CS_PIN);
    lcd_dev = (struct rt_spi_device *)rt_device_find(PKG_ST_7735_SPI_DEVICE_NAME);
    if (lcd_dev != RT_NULL)
    {
        struct rt_spi_configuration spi_config;
        spi_config.data_width = 8;
        spi_config.max_hz = 25 * 1000 * 1000;
        spi_config.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
        rt_spi_configure(lcd_dev, &spi_config);
    }
    else
    {
        res = -RT_ERROR;
    }

    LCD_Init();
    return res;
}

INIT_COMPONENT_EXPORT(_spi_lcd_init);















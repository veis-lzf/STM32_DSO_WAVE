#include "bsp_oled.h"
#include "oledfont.h"
#include "bsp_i2c_gpio.h"
#include "app_cfg.h"
#include "debug.h"

#if OLED_APP_CFG
// OLED的显存
// 存放格式如下.
// [0] 0 1 2 3 ... 127
// [1] 0 1 2 3 ... 127
// [2] 0 1 2 3 ... 127
// [3] 0 1 2 3 ... 127
// [4] 0 1 2 3 ... 127
// [5] 0 1 2 3 ... 127
// [6] 0 1 2 3 ... 127
// [7] 0 1 2 3 ... 127
uint8_t OLED_GRAM[128][8];

/**
 * @name	  oled_refresh_gram
 * @brief     更新显存到OLED
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_refresh_gram(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		oled_write_byte (0xb0 + i, OLED_CMD); // 设置页地址（0~7）
		oled_write_byte (0x00, OLED_CMD);     // 设置显示位置—列低地址
		oled_write_byte (0x10, OLED_CMD);     // 设置显示位置—列高地址
		for (n = 0; n < 128; n++)
		{
			oled_write_byte(OLED_GRAM[n][i], OLED_DATA);
		}
	}
}

/**
 * @name	  oled_write_byte
 * @brief     往oled写一个字节
 * @param	  dat：需要写入的字节
			  cmd：命令（0）/数据（1）
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_write_byte(uint8_t dat, uint8_t cmd)
{
#if (OLED_MODE==SPI_MODE)
	uint8_t i;
	OLED_RS = cmd; // 写命令
	OLED_CS = 0;
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK = 0;
		if (dat & 0x80)
		{
			OLED_SDIN = 1;		// 写入1
		}
		else 
		{
			OLED_SDIN = 0;		// 写入0
		}
		OLED_SCLK = 1;
		dat <<= 1;
	}
	OLED_CS = 1;
	OLED_RS = 1;
#elif	(OLED_MODE==I2C_MODE)
	if(cmd == OLED_CMD)
	{
		i2c_Start();
		i2c_SendByte(OLED_ADDRESS);// OLED地址
		i2c_WaitAck();
		i2c_SendByte(0x00);// 寄存器地址
		i2c_WaitAck();
		i2c_SendByte(dat);
		i2c_WaitAck();
		i2c_Stop();
	}
	else if(cmd == OLED_DATA)
	{
		i2c_Start();
		i2c_SendByte(OLED_ADDRESS);//OLED地址
		i2c_WaitAck();
		i2c_SendByte(0x40);//寄存器地址
		i2c_WaitAck();
		i2c_SendByte(dat);
		i2c_WaitAck();
		i2c_Stop();
	}
#endif
}


/**
 * @name	  oled_display_on
 * @brief     开启OLED显示
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_display_on(void)
{
	oled_write_byte(0X8D, OLED_CMD); // SET DCDC命令
	oled_write_byte(0X14, OLED_CMD); // DCDC ON
	oled_write_byte(0XAF, OLED_CMD); // DISPLAY ON
}

/**
 * @name	  oled_display_off
 * @brief     关闭OLED显示
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_display_off(void)
{
	oled_write_byte(0X8D, OLED_CMD); // SET DCDC命令
	oled_write_byte(0X10, OLED_CMD); // DCDC OFF
	oled_write_byte(0XAE, OLED_CMD); // DISPLAY OFF
}

/**
 * @name	  oled_clear
 * @brief     清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		for (n = 0; n < 128; n++)
		{
			OLED_GRAM[n][i] = 0x00;
		}
	}
	oled_refresh_gram();	// 更新显示
}

// 局部请屏函数
void oled_part_clear(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1)
{
	uint8_t i,n;
    for(i = x0; i < x1; i++)
    {
        for(n = y0; n < y1; n++)
        {
            OLED_GRAM[i][n] = 0x00;
        }
    }
    oled_refresh_gram();
}


/**
 * @name	  oled_drawpoint
 * @brief     画点
 * @param	  x：0~127
			  y：0~63
              t：1 填充 0,清空
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_drawpoint(uint8_t x, uint8_t y, uint8_t t)
{
	uint8_t pos, bx, temp = 0;
	if (x > 127 || y > 63)	// 判断是否超出范围了.
	{
		return;	 
	}
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)
	{
		OLED_GRAM[x][pos] |= temp;
	}
	else
	{
		OLED_GRAM[x][pos] &= ~temp;
	}
}

/**
 * @name	  oled_fill
 * @brief     对屏幕进行矩形区域填充
 * @param	  x1,y1,x2,y2：填充区域的对角坐标
			  确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
              ot：0,清空;1,填充
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
	uint8_t x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)oled_drawpoint(x, y, dot);
	}
	oled_refresh_gram();	// 更新显示
}

/**
 * @name	  oled_show_char
 * @brief     在（x,y）处开始显示一个字符
 * @param	  在指定位置显示一个字符,包括部分字符
			  x：0~127
              y：0~63
              mode：0,反白显示;1,正常显示
              size：选择字体 12/16/24
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
	uint8_t temp, t, t1;
	uint8_t y0 = y;
	uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		// 得到字体一个字符对应点阵集所占的字节数
	chr = chr - ' ';									// 得到偏移后的值
	for (t = 0; t < csize; t++)
	{
		if (size == 12 )temp = asc2_1206[chr][t]; 	 	// 调用1206字体
		else if (size == 16) temp = asc2_1608[chr][t];	// 调用1608字体
		else if (size == 24) temp = asc2_2412[chr][t];	// 调用2412字体
		else if (size == 32) temp = asc2_3216[chr][t];  // 调用3216字体
		else return;									// 没有的字库
		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)oled_drawpoint(x, y, mode);
			else oled_drawpoint(x, y, !mode);
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

/**
 * @name	  mypow
 * @brief     求m^n函数
 * @param	  m：底数
			  n：次幂
 * @return    uint32_t类型
 * @DateTime  2019-7-17
 */
uint32_t mypow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--) result *= m;
	return result;
}

/**
 * @name	  oled_show_number
 * @brief     在屏幕（x,y）为起始位置开始显示一个数字
 * @param	  显示2个数字
			  x,y :起点坐标
              len :数字的位数
              size:字体大小
              mode:模式	0,填充模式;1,叠加模式
              num:数值(0~4294967295);
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_show_number(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				oled_show_char(x + (size / 2)*t, y, ' ', size, 1);
				continue;
			} else enshow = 1;

		}
		oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);
	}
}

/**
 * @name	  oled_show_string
 * @brief     在屏幕（x,y）位置显示数字
 * @param	  显示字符串
              x,y：起点坐标
              size：字体大小
              *p：字符串起始地址
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_show_string(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size)
{
	while ((*p <= '~') && (*p >= ' '))	// 判断是不是非法字符!
	{
		if (x > (128 - (size / 2)))
		{
			x = 0; y += size;
		}
		if (y > (64 - size)) 
		{
			y = x = 0; oled_clear();
		}
		oled_show_char(x, y, *p, size, 1);
		x += size / 2;
		p++;
	}
}

/**
 * @name	  oled_init
 * @brief     初始化OLED控制芯片SSD1306
 * @param	  空      
 * @return    空
 * @DateTime  2019-7-17
 */
void oled_init(void)
{
	p_dbg_enter;
#if (OLED_MODE==SPI_MODE)
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(OLED_CLK, ENABLE);	// 使能GPIO时钟

	GPIO_InitStructure.GPIO_Pin = SCLK_PIN | SDIN_PIN | RST_PIN | RS_PIN | CS_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// 普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(OLED_PORT, &GPIO_InitStructure);				// 初始化

	OLED_SDIN = 1;
	OLED_SCLK = 1;

	OLED_CS = 1;
	OLED_RS = 1;

	OLED_RST = 0;
	delay_ms(100);
	OLED_RST = 1;
#elif (OLED_MODE==I2C_MODE)
	if(i2c_CheckDevice(OLED_ADDRESS))
	{
		p_err("no detect i2c device on allress:0x%x", OLED_ADDRESS);
		return;
	}
#endif 
	oled_write_byte(0xAE, OLED_CMD); // 关闭显示
	oled_write_byte(0xD5, OLED_CMD); // 设置时钟分频因子,震荡频率
	oled_write_byte(80, OLED_CMD);   // [3:0],分频因子;[7:4],震荡频率
	oled_write_byte(0xA8, OLED_CMD); // 设置驱动路数
	oled_write_byte(0X3F, OLED_CMD); // 默认0X3F(1/64)
	oled_write_byte(0xD3, OLED_CMD); // 设置显示偏移
	oled_write_byte(0X00, OLED_CMD); // 默认为0

	oled_write_byte(0x40, OLED_CMD); // 设置显示开始行 [5:0],行数.

	oled_write_byte(0x8D, OLED_CMD); // 电荷泵设置
	oled_write_byte(0x14, OLED_CMD); // bit2，开启/关闭
	oled_write_byte(0x20, OLED_CMD); // 设置内存地址模式
	oled_write_byte(0x02, OLED_CMD); // [1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	oled_write_byte(0xA1, OLED_CMD); // 段重定义设置,bit0:0,0->0;1,0->127;
	oled_write_byte(0xC0, OLED_CMD); // 设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	oled_write_byte(0xDA, OLED_CMD); // 设置COM硬件引脚配置
	oled_write_byte(0x12, OLED_CMD); // [5:4]配置

	oled_write_byte(0x81, OLED_CMD); // 对比度设置
	oled_write_byte(0xEF, OLED_CMD); // 1~255;默认0X7F (亮度设置,越大越亮)
	oled_write_byte(0xD9, OLED_CMD); // 设置预充电周期
	oled_write_byte(0xf1, OLED_CMD); // [3:0],PHASE 1;[7:4],PHASE 2;
	oled_write_byte(0xDB, OLED_CMD); // 设置VCOMH 电压倍率
	oled_write_byte(0x30, OLED_CMD); // [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_write_byte(0xA4, OLED_CMD); // 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	oled_write_byte(0xA6, OLED_CMD); // 设置显示方式;bit0:1,反相显示;0,正常显示
	oled_write_byte(0xAF, OLED_CMD); // 开启显示
	oled_clear();
}

#endif 

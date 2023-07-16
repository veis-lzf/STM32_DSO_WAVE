#include "bsp_oled.h"
#include "oledfont.h"
#include "bsp_i2c_gpio.h"
#include "app_cfg.h"
#include "debug.h"

#if OLED_APP_CFG
// OLED���Դ�
// ��Ÿ�ʽ����.
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
 * @brief     �����Դ浽OLED
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_refresh_gram(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		oled_write_byte (0xb0 + i, OLED_CMD); // ����ҳ��ַ��0~7��
		oled_write_byte (0x00, OLED_CMD);     // ������ʾλ�á��е͵�ַ
		oled_write_byte (0x10, OLED_CMD);     // ������ʾλ�á��иߵ�ַ
		for (n = 0; n < 128; n++)
		{
			oled_write_byte(OLED_GRAM[n][i], OLED_DATA);
		}
	}
}

/**
 * @name	  oled_write_byte
 * @brief     ��oledдһ���ֽ�
 * @param	  dat����Ҫд����ֽ�
			  cmd�����0��/���ݣ�1��
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_write_byte(uint8_t dat, uint8_t cmd)
{
#if (OLED_MODE==SPI_MODE)
	uint8_t i;
	OLED_RS = cmd; // д����
	OLED_CS = 0;
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK = 0;
		if (dat & 0x80)
		{
			OLED_SDIN = 1;		// д��1
		}
		else 
		{
			OLED_SDIN = 0;		// д��0
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
		i2c_SendByte(OLED_ADDRESS);// OLED��ַ
		i2c_WaitAck();
		i2c_SendByte(0x00);// �Ĵ�����ַ
		i2c_WaitAck();
		i2c_SendByte(dat);
		i2c_WaitAck();
		i2c_Stop();
	}
	else if(cmd == OLED_DATA)
	{
		i2c_Start();
		i2c_SendByte(OLED_ADDRESS);//OLED��ַ
		i2c_WaitAck();
		i2c_SendByte(0x40);//�Ĵ�����ַ
		i2c_WaitAck();
		i2c_SendByte(dat);
		i2c_WaitAck();
		i2c_Stop();
	}
#endif
}


/**
 * @name	  oled_display_on
 * @brief     ����OLED��ʾ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_display_on(void)
{
	oled_write_byte(0X8D, OLED_CMD); // SET DCDC����
	oled_write_byte(0X14, OLED_CMD); // DCDC ON
	oled_write_byte(0XAF, OLED_CMD); // DISPLAY ON
}

/**
 * @name	  oled_display_off
 * @brief     �ر�OLED��ʾ
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_display_off(void)
{
	oled_write_byte(0X8D, OLED_CMD); // SET DCDC����
	oled_write_byte(0X10, OLED_CMD); // DCDC OFF
	oled_write_byte(0XAE, OLED_CMD); // DISPLAY OFF
}

/**
 * @name	  oled_clear
 * @brief     ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
 * @param	  ��
 * @return    ��
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
	oled_refresh_gram();	// ������ʾ
}

// �ֲ���������
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
 * @brief     ����
 * @param	  x��0~127
			  y��0~63
              t��1 ��� 0,���
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_drawpoint(uint8_t x, uint8_t y, uint8_t t)
{
	uint8_t pos, bx, temp = 0;
	if (x > 127 || y > 63)	// �ж��Ƿ񳬳���Χ��.
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
 * @brief     ����Ļ���о����������
 * @param	  x1,y1,x2,y2���������ĶԽ�����
			  ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
              ot��0,���;1,���
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
	uint8_t x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)oled_drawpoint(x, y, dot);
	}
	oled_refresh_gram();	// ������ʾ
}

/**
 * @name	  oled_show_char
 * @brief     �ڣ�x,y������ʼ��ʾһ���ַ�
 * @param	  ��ָ��λ����ʾһ���ַ�,���������ַ�
			  x��0~127
              y��0~63
              mode��0,������ʾ;1,������ʾ
              size��ѡ������ 12/16/24
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
	uint8_t temp, t, t1;
	uint8_t y0 = y;
	uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		// �õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr = chr - ' ';									// �õ�ƫ�ƺ��ֵ
	for (t = 0; t < csize; t++)
	{
		if (size == 12 )temp = asc2_1206[chr][t]; 	 	// ����1206����
		else if (size == 16) temp = asc2_1608[chr][t];	// ����1608����
		else if (size == 24) temp = asc2_2412[chr][t];	// ����2412����
		else if (size == 32) temp = asc2_3216[chr][t];  // ����3216����
		else return;									// û�е��ֿ�
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
 * @brief     ��m^n����
 * @param	  m������
			  n������
 * @return    uint32_t����
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
 * @brief     ����Ļ��x,y��Ϊ��ʼλ�ÿ�ʼ��ʾһ������
 * @param	  ��ʾ2������
			  x,y :�������
              len :���ֵ�λ��
              size:�����С
              mode:ģʽ	0,���ģʽ;1,����ģʽ
              num:��ֵ(0~4294967295);
 * @return    ��
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
 * @brief     ����Ļ��x,y��λ����ʾ����
 * @param	  ��ʾ�ַ���
              x,y���������
              size�������С
              *p���ַ�����ʼ��ַ
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_show_string(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size)
{
	while ((*p <= '~') && (*p >= ' '))	// �ж��ǲ��ǷǷ��ַ�!
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
 * @brief     ��ʼ��OLED����оƬSSD1306
 * @param	  ��      
 * @return    ��
 * @DateTime  2019-7-17
 */
void oled_init(void)
{
	p_dbg_enter;
#if (OLED_MODE==SPI_MODE)
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(OLED_CLK, ENABLE);	// ʹ��GPIOʱ��

	GPIO_InitStructure.GPIO_Pin = SCLK_PIN | SDIN_PIN | RST_PIN | RS_PIN | CS_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(OLED_PORT, &GPIO_InitStructure);				// ��ʼ��

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
	oled_write_byte(0xAE, OLED_CMD); // �ر���ʾ
	oled_write_byte(0xD5, OLED_CMD); // ����ʱ�ӷ�Ƶ����,��Ƶ��
	oled_write_byte(80, OLED_CMD);   // [3:0],��Ƶ����;[7:4],��Ƶ��
	oled_write_byte(0xA8, OLED_CMD); // ��������·��
	oled_write_byte(0X3F, OLED_CMD); // Ĭ��0X3F(1/64)
	oled_write_byte(0xD3, OLED_CMD); // ������ʾƫ��
	oled_write_byte(0X00, OLED_CMD); // Ĭ��Ϊ0

	oled_write_byte(0x40, OLED_CMD); // ������ʾ��ʼ�� [5:0],����.

	oled_write_byte(0x8D, OLED_CMD); // ��ɱ�����
	oled_write_byte(0x14, OLED_CMD); // bit2������/�ر�
	oled_write_byte(0x20, OLED_CMD); // �����ڴ��ַģʽ
	oled_write_byte(0x02, OLED_CMD); // [1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	oled_write_byte(0xA1, OLED_CMD); // ���ض�������,bit0:0,0->0;1,0->127;
	oled_write_byte(0xC0, OLED_CMD); // ����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	oled_write_byte(0xDA, OLED_CMD); // ����COMӲ����������
	oled_write_byte(0x12, OLED_CMD); // [5:4]����

	oled_write_byte(0x81, OLED_CMD); // �Աȶ�����
	oled_write_byte(0xEF, OLED_CMD); // 1~255;Ĭ��0X7F (��������,Խ��Խ��)
	oled_write_byte(0xD9, OLED_CMD); // ����Ԥ�������
	oled_write_byte(0xf1, OLED_CMD); // [3:0],PHASE 1;[7:4],PHASE 2;
	oled_write_byte(0xDB, OLED_CMD); // ����VCOMH ��ѹ����
	oled_write_byte(0x30, OLED_CMD); // [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_write_byte(0xA4, OLED_CMD); // ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	oled_write_byte(0xA6, OLED_CMD); // ������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
	oled_write_byte(0xAF, OLED_CMD); // ������ʾ
	oled_clear();
}

#endif 

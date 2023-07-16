#ifndef __BSP_OLED_H
#define __BSP_OLED_H	

#include "main.h"


#define I2C_MODE	1
#define SPI_MODE	2

#define OLED_CMD  	0			// д����
#define OLED_DATA 	1			// д����

#define OLED_MODE	I2C_MODE


#if (OLEDMODE == SPI_MODE)
// -----------------OLED�˿ڶ���- ---------------  	
#define OLED_CLK 	RCC_AHB1Periph_GPIOB
#define OLED_PORT   GPIOB
#define SCLK_PIN 	GPIO_Pin_5
#define SDIN_PIN 	GPIO_Pin_6
#define RST_PIN  	GPIO_Pin_7
#define RS_PIN 		GPIO_Pin_8
#define CS_PIN 		GPIO_Pin_9

// λ������
#define OLED_CS 	PBout(9)  	// CS
#define OLED_RS 	PBout(8)  	// DC
#define OLED_RST    PBout(7)  	// RES
#define OLED_SDIN 	PBout(6)  	// D1
#define OLED_SCLK 	PBout(5)  	// D0	

#elif (OLED_MODE == I2C_MODE)
	#define OLED_ADDRESS	(0x78)
#endif 

// OLED�ڲ������ú���
void oled_write_byte(uint8_t dat,uint8_t cmd);	    
void oled_display_on(void);
void oled_display_off(void);
void oled_refresh_gram(void);	

// �û��ⲿ���ú���
void oled_init(void);
void oled_clear(void);
void oled_drawpoint(uint8_t x,uint8_t y,uint8_t t);
void oled_fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void oled_show_char(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void oled_show_number(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void oled_show_string(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);
void oled_part_clear(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
#endif /* __BSP_OLED_H */

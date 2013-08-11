#include "stm32f4xx.h"
#include "platform_config.h"
#include "hw_config.h"
#include "glcd.h"
#include "fontdata.h"
#ifdef USE_KANJI
#include "libnkf.h"
#endif

// 400x240ドットのグラフィック液晶を使う。
// フォントが半角 8x16なら、50文字x15行 750文字
// フォントが全角16x16なら、25文字x15行 375文字

uint8_t glcd_buf[50*240];
uint8_t glcd_x,glcd_y,flgUpdate;
/* 2013.07.21 */
uint16_t glcd_kanji_text_buf[50*16];

void glcd_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStruct;

	/* GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* SPI2SCK:PB13 SPI2MOSI:PB15 SPI2_NSS:PB12	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	/* GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);

//	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // 1.3MHz
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
//	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);
}

void glcd_BufClear(uint8_t f)
{
	int i;
	if (f)
		f = 0xff;
	for (i = 0;i < (50*240);i++)
		glcd_buf[i] = f;
	for (i = 0;i < (50*16);i++) {
		if (f) {
			glcd_kanji_text_buf[i] = 0x8000;
		} else {
			glcd_kanji_text_buf[i] = 0x0;
		}
	}

}

void glcd_TransFromBuf(void)
{
	int l,x;
	int y = 0;
	static int m0 = 0;
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	for (l = 1;l <= 240;l++) {
		if (m0) {
			SPI2->DR = 0x03;	// M0 = H M1 = H
			m0 = 0;
		} else {
			SPI2->DR = 0x01;	// M0 = H M1 = L
			m0 = 1;
		}
		while( !(SPI2->SR & SPI_I2S_FLAG_TXE) );
		SPI2->DR = l;	// ラインアドレス
		while( !(SPI2->SR & SPI_I2S_FLAG_TXE) );
		for (x = 0; x < 50;x++){
			SPI2->DR = glcd_buf[y++];
			while( !(SPI2->SR & SPI_I2S_FLAG_TXE) );
		}
	}
	SPI2->DR = 0x00;	// DUMMY
	while( !(SPI2->SR & SPI_I2S_FLAG_TXE) );
	SPI2->DR = 0x00;	// DUMMY
	while( !(SPI2->SR & SPI_I2S_FLAG_TXE) );
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void glcd_PutChar(uint16_t c)
{
	int i,flg,flgUp,w;
	uint8_t flgInverse;
	uint32_t sp;
	if (c & 0x8000) {
		flgInverse = 0xff;
	} else {
		flgInverse = 0;
	}
	c &= 0x7fff;
	flg = 0;
	flgUp = 0;
	sp = 0;
	w = width1bytefont;
	if ((c == 0x07) || (c == 0x08) || (c == 0x0a) || (c == 0x0d)) {
		if ((c == 0x08) && (glcd_x > 0)) {
			glcd_x--;
		}
		if (c == 0x0d) {
			glcd_x = 0;
		}
		if (c == 0x0a) {
			glcd_y ++;
/*
			if (glcd_y >= GLCD_H) {
				glcd_y = GLCD_H - 1;
				for(i = 0;i < (50*240-400*2);i++) {
					glcd_buf[i] = glcd_buf[i+400*2];
				}
				for(i = 50*240-400*2;i < (50*240);i++) {
					glcd_buf[i] = 0xff;
				}
			}
*/
		}
		return;
	} else {
		if (glcd_y >= GLCD_H) {
			glcd_y = GLCD_H - 1;
			for(i = 0;i < (50*240-400*2);i++) {
				glcd_buf[i] = glcd_buf[i+400*2];
			}
			for(i = 50*240-400*2;i < (50*240);i++) {
				glcd_buf[i] = 0xff;
			}
			flgUp = 1;
		}
		for (i = 0;i < sizeOFblock1bytecode;i +=2) {
			if ((c >= block1bytecode[i]) && (c <= block1bytecode[i + 1])) {
				flg = 1;
				w = width1bytefont;
				if (width1bytefont > 4) {
					sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w * 2);
				} else {
					sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w);
				}
			}
		}
		if ((flg == 0) && (c >= 0x100))
			for (i = 0;i < sizeOFblock2bytecode;i +=2) {
				if ((c >= block2bytecode[i]) && (c <= block2bytecode[i + 1])) {
					flg = 1;
					w = width2bytefont;
					if (width1bytefont > 4) {
						sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w * 2);
					} else {
						sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w);
					}
				}
			}
	}
	if (flg == 0) {
		if (c >= 0x100)
			w = width2bytefont;
	}
	if ((glcd_x >= GLCD_W) || ((glcd_x >= (GLCD_W - 1)) && (w > width1bytefont))) {
		glcd_x = 0;
		glcd_y ++;
	}
/*
	if (glcd_y >= GLCD_H) { 
		glcd_y = GLCD_H - 1;
		for(i = 0;i < (50*240-400*2);i++) {
			glcd_buf[i] = glcd_buf[i+400*2];
		}
		for(i = 50*240-400*2;i < (50*240);i++) {
			glcd_buf[i] = 0xff;
		}
	}
*/
	if (flg) {
		for(i = 0;i < heightfont;i++) {
			glcd_buf[glcd_x+(i+glcd_y*heightfont)*GLCD_W] = flgInverse ^ allFontDatas[sp+i];
			if (w > 8) {
				glcd_buf[glcd_x+(i+glcd_y*heightfont)*GLCD_W+1] = flgInverse ^ allFontDatas[sp+w+i];
			}
		}
	}
	if (w == width2bytefont) {
		glcd_x += 2;
	} else {
		glcd_x++;
	}
	if (flgUp && flgUpdate) {
		glcd_TransFromBuf();
	}
}

void glcd_Puts(const char *buf)
{
	int flg16 = 0;
	int i = 0;
	short j = 0;
	int k;
	while(buf[i]) {
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x24)&&(buf[i+2] ==0x42)) {
			i += 3;
			flg16 = 1;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x24)&&(buf[i+2] ==0x40)) {
			i += 3;
			flg16 = 1;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x28)&&(buf[i+2] ==0x42)) {
			i += 3;
			flg16 = 0;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]=='[')&&(buf[i+2] =='H')) {
			i += 3;
			glcd_x = glcd_y = 0;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]=='[')&&(buf[i+2] =='2')&&(buf[i+2] =='2')) {
			i += 4;
			for (k = 0;k < (50*240);k++)
				glcd_buf[k] = 0xff;
		} else
		if (flg16) {
			if (j) {
				glcd_PutChar(0x8000 | (j << 8 | buf[i]));
				j = 0;
			} else {
				j = buf[i];
			}
			i++ ;
		} else {
			glcd_PutChar(0x8000 | (buf[i] & 0x0ff));
			i++ ;
		}
	}
}

void glcd_transChar()
{
	int i,flg,w;
	uint8_t flgInverse;
	uint32_t sp;

	uint16_t c;
	int x,y;
	y = 0;
	while (y < GLCD_H) {
		x = 0;
		while (x < GLCD_W) {
			c = glcd_kanji_text_buf[x + y * GLCD_W];
			if (c & 0x8000) {
				flgInverse = 0xff;
			} else {
				flgInverse = 0;
			}
			c &= 0x7fff;
			flg = 0;
			sp = 0;
			w = width1bytefont;
			for (i = 0;i < sizeOFblock1bytecode;i +=2) {
				if ((c >= block1bytecode[i]) && (c <= block1bytecode[i + 1])) {
					flg = 1;
					w = width1bytefont;
					if (width1bytefont > 4) {
						sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w * 2);
					} else {
						sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w);
					}
				}
			}
			if ((flg == 0) && (c >= 0x100))
				for (i = 0;i < sizeOFblock2bytecode;i +=2) {
					if ((c >= block2bytecode[i]) && (c <= block2bytecode[i + 1])) {
						flg = 1;
						w = width2bytefont;
						if (width1bytefont > 4) {
							sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w * 2);
						} else {
							sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w);
						}
					}
				}
			if (flg == 0) {
				if (c >= 0x100)
					w = width2bytefont;
			}
			if (flg) {
				for(i = 0;i < heightfont;i++) {
					glcd_buf[x+(i+y*heightfont)*GLCD_W] = flgInverse ^ allFontDatas[sp+i];
					if (w > 8) {
						glcd_buf[x+(i+y*heightfont)*GLCD_W+1] = flgInverse ^ allFontDatas[sp+w+i];
					}
				}
			} else {
				for(i = 0;i < heightfont;i++) {
					glcd_buf[x+(i+y*heightfont)*GLCD_W] = flgInverse;
					if (w > 8) {
						glcd_buf[x+(i+y*heightfont)*GLCD_W+1] = flgInverse;
					}
				}
			}
			if (w == width2bytefont) {
				x += 2;
			} else {
				x++;
			}
		}
		y++;
	}
}

void glcd_Puts_Serial(const char *buf)
{
	int flg16 = 0;
	int i = 0;
	int k;
	uint16_t j = 0;
	while(buf[i]) {
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x24)&&(buf[i+2] ==0x42)) {
			i += 3;
			flg16 = 1;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x24)&&(buf[i+2] ==0x40)) {
			i += 3;
			flg16 = 1;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]==0x28)&&(buf[i+2] ==0x42)) {
			i += 3;
			flg16 = 0;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]=='[')&&(buf[i+2] =='H')) {
			i += 3;
			glcd_x = glcd_y = 0;
		} else
		if ((buf[i] == 0x1B)&&(buf[i+1]=='[')&&(buf[i+2] =='2')&&(buf[i+2] =='2')) {
			i += 4;
			for (k = 0;k < (50*16);k++)
				glcd_kanji_text_buf[k] = 0x8000;
		} else
		if (flg16) {
			if (j) {
				if (glcd_x >= (GLCD_W - 1)) {
					glcd_x = 0;
					glcd_y ++;
				}
				glcd_kanji_text_buf[glcd_x + glcd_y * GLCD_W] = 0x8000 | (j << 8 | buf[i]);
				j = 0;
				glcd_x += 2;
			} else {
				j = buf[i];
			}
			i++ ;
		} else {
			if ((buf[i] == 0x08) && (glcd_x > 0)) {
				glcd_x--;
			} else if (buf[i] == 0x0d) {
				glcd_x = 0;
			} else if (buf[i] == 0x0a) {
				glcd_y ++;
			} else {
				if (glcd_x >= GLCD_W) {
					glcd_x = 0;
					glcd_y ++;
				}
				glcd_kanji_text_buf[glcd_x + glcd_y * GLCD_W] = 0x8000 | (buf[i] & 0x0ff);
				glcd_x++;
			}
			i++ ;
		}
		if (glcd_y >= GLCD_H) {
			glcd_y = GLCD_H-1;
			for(k = 0;k < (GLCD_W*GLCD_H);k++) {
				glcd_kanji_text_buf[k] = glcd_kanji_text_buf[k+GLCD_W];
			}
			for(k = GLCD_W*GLCD_H;k < (GLCD_W*(GLCD_H+1));k++) {
				glcd_kanji_text_buf[k] = 0x8000;
			}
		}
	}
}


void glcd_PutCharAt(uint16_t x,uint16_t y,uint16_t c)
{
	int i,flg,w;
	uint8_t flgInverse;
	uint32_t sp;
	if (c & 0x8000) {
		flgInverse = 0xff;
	} else {
		flgInverse = 0;
	}
	x /= 8;
	c &= 0x7fff;
	flg = 0;
	sp = 0;
	w = width1bytefont;
	if ((c == 0x07) || (c == 0x08) || (c == 0x0a) || (c == 0x0d)) {
		return;
	} else {
		for (i = 0;i < sizeOFblock1bytecode;i +=2) {
			if ((c >= block1bytecode[i]) && (c <= block1bytecode[i + 1])) {
				flg = 1;
				w = width1bytefont;
				if (width1bytefont > 4) {
					sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w * 2);
				} else {
					sp = block1bytesp[i / 2] + ((c - block1bytecode[i]) * w);
				}
			}
		}
		if ((flg == 0) && (c >= 0x100))
			for (i = 0;i < sizeOFblock2bytecode;i +=2) {
				if ((c >= block2bytecode[i]) && (c <= block2bytecode[i + 1])) {
					flg = 1;
					w = width2bytefont;
					if (width1bytefont > 4) {
						sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w * 2);
					} else {
						sp = block2bytesp[i / 2] + ((c - block2bytecode[i]) * w);
					}
				}
			}
	}
	if (flg == 0) {
		if (c >= 0x100)
			w = width2bytefont;
	}
	if (flg) {
		for(i = 0;i < heightfont;i++) {
			glcd_buf[x+(i+y)*GLCD_W] = flgInverse ^ allFontDatas[sp+i];
			if (w > 8) {
				glcd_buf[x+(i+y)*GLCD_W+1] = flgInverse ^ allFontDatas[sp+w+i];
			}
		}
	}
}

void glcd_PutsAt(uint16_t x,uint16_t y,const char *buf)
{
	int i=0;
	while(buf[i]) {
		glcd_PutCharAt(x,y,0x8000 | buf[i]);
		i++;
		x += 8;
	}
}

void glcd_PutsAtINV(uint16_t x,uint16_t y,const char *buf)
{
	int i=0;
	while(buf[i]) {
		glcd_PutCharAt(x,y,buf[i]);
		i++;
		x += 8;
	}
}

void glcd_Vline(uint16_t x,int32_t d)
{
	int i,p;
	int xx;
	uint8_t e;
	uint8_t buf[20];
	xx = x / 8;
	e = (0x01 << (x % 8));
	for(i = 0;i < 240;i++) {
		if ((i/2) % 2)
			glcd_buf[xx+i*GLCD_W] &= (~e);
	}
	p = 0;
	if (d < 0) {
		d = -d;
		buf[p++] = '-';
		return;
	}
	uint8_t flg = 1;
	int32_t j = 10;
	if (d >= 1000000000) {
		buf[p++] = '0' + (d / 1000000000);
		d = d % 1000000000;
	}
	if ((d >= 1000) && (d % 1000 == 0)) {
		flg = 0;
		d /= 1000;
	} 
	while (d >= j) {
		j *= 10;
	}
	j /= 10;
	while (j >= 1) {
		buf[p++] = '0' + (d / j);
		d = d % j;
		j /= 10;
	}
	if (flg)
		buf[p++] = 'm';
	buf[p++] = 's';
	x -= (8 * ((p-1)/2));
	for(j = 0;j < p;j++) {
		glcd_PutCharAt(x,0,0x8000|buf[j]);
		x += 8;
	}
}

void glcd_setLine(uint16_t x,uint16_t y,uint8_t d)
{
	x /= 8;
	glcd_buf[x+y*GLCD_W] &= d;
}

void glcd_setDot(uint16_t x,uint16_t y)
{
	glcd_buf[(x/8)+y*GLCD_W] &= (~(0x01 << (x % 8)));
}

void glcd_Dec(uint16_t x,uint16_t y,int n,int d,uint8_t flg)
{
	int i,j,k;
	uint8_t c;
	for (i = 0;i < n;i++) {
		k = 1;
		for(j = 0;j < (n - i - 1);j ++)
			k *= 10;
		c = (d / k) % 10;
		if (flg) {
			glcd_PutCharAt(x,y,0x8000|('0'+c));
		} else {
			glcd_PutCharAt(x,y,'0'+c);
		}
		x += 8;
	}
}

void glcd_Hex(uint16_t x,uint16_t y,int n,int32_t d)
{
	int i,j,k;
	uint8_t c;
	for (i = 0;i < n;i++) {
		k = 1;
		for(j = 0;j < (n - i - 1);j ++)
			k *= 16;
		c = (d / k) % 16;
		if (c >= 10) {
			glcd_PutCharAt(x,y,0x8000|('A'+c-10));
		} else {
			glcd_PutCharAt(x,y,0x8000|('0'+c));
		}
		x += 8;
	}
}

void glcd_PutsUint(int32_t d)
{
/*
	if (d < 0) {
		d = -d;
		glcd_PutChar(0x8000 | ('-'));
	}
*/
	int j = 10;
	if (d >= 1000000000) {
		glcd_PutChar(0x8000 | ('0' + (d / 1000000000)));
		d = d % 1000000000;
	}
	while (d >= j) {
		j *= 10;
	}
	j /= 10;
	while (j >= 1) {
		glcd_PutChar(0x8000 | ('0' + (d / j)));
		d = d % j;
		j /= 10;
	}
}

void glcd_PutsUint8(uint32_t d)
{
/*
	if (d < 0) {
		d = -d;
		glcd_PutChar(0x8000 | ('-'));
	}
*/
	if (d < 10) {
		glcd_PutChar(0x8000 | ('0'));
	}
	int j = 10;
	if (d >= 1000000000) {
		glcd_PutChar(0x8000 | ('0' + (d / 1000000000)));
		d = d % 1000000000;
	}
	while (d >= j) {
		j *= 10;
	}
	j /= 10;
	while (j >= 1) {
		glcd_PutChar(0x8000 | ('0' + (d / j)));
		d = d % j;
		j /= 10;
	}
}

void glcd_PutsInt(int32_t d)
{
	if (d < 0) {
		d = -d;
		glcd_PutChar(0x8000 | ('-'));
	}
	int i = 0;
	int j = 10;
	if (d >= 1000000000) {
		glcd_PutChar(0x8000 | ('0' + (d / 1000000000)));
		d = d % 1000000000;
	}
	while (d >= j) {
		j *= 10;
		i++;
	}
	j /= 10;
	while (j >= 1) {
		glcd_PutChar(0x8000 | ('0' + (d / j)));
		d = d % j;
		j /= 10;
	}
}


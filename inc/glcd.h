void glcd_Init(void);
void glcd_BufClear(uint8_t flg);
void glcd_TransFromBuf(void);
void glcd_PutChar(uint16_t c);
void glcd_Puts(const char *buf);
void glcd_Puts_Serial(const char *buf);
void glcd_transChar();
void glcd_PutCharAt(uint16_t x,uint16_t y,uint16_t c);
void glcd_PutsAt(uint16_t x,uint16_t y,const char *);
void glcd_PutsAtINV(uint16_t x,uint16_t y,const char *);
void glcd_Dec(uint16_t x,uint16_t y,int,int,uint8_t);
void glcd_Hex(uint16_t x,uint16_t y,int,int32_t);
void glcd_Vline(uint16_t i,int32_t j);
void glcd_setLine(uint16_t x,uint16_t y,uint8_t d);
void glcd_setDot(uint16_t x,uint16_t y);

void glcd_PutsUint(int32_t d);
void glcd_PutsUint8(uint32_t d);
void glcd_PutsInt(int32_t d);

extern uint8_t glcd_x,glcd_y,flgUpdate;


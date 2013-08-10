#ifndef __TERM_IO_H
#define __TERM_IO_H	0x0300

#ifdef __cplusplus
 extern "C" {
#endif

/* Basics */
#include <stdarg.h>
#include <string.h>
#include "monitor.h"

/* Miscs */
#include "hw_config.h"
#include "ff.h"
#include "display_if.h"
#ifdef USE_ADS7843
 #include "touch_if.h"
#endif

/* #define xavail() keypressed() */
#define __kbhit() keypressed()

/* Externals */
extern char xgetc_n (void);
extern unsigned char xgetc (void);

/* Macros */
#define BTN_UP		0x05
#define BTN_DOWN	0x18
#define BTN_LEFT	0x13
#define BTN_RIGHT	0x04
#define BTN_OK		0x0D
#define BTN_ESC		0x1A
#define BTN_CAN		0x1B
#define BTN_POWER	0x7F
#define CMD_LOWBAT	0x1F


#ifdef __cplusplus
}
#endif

#endif	/* __TERM_IO_H */

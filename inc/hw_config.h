#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H 0x0100

#ifdef __cplusplus
 extern "C" {
#endif

/* General Inclusion */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_conf.h"
#include "platform_config.h"

/* Function Inclusion */
#include "systick.h"
#include "uart_support.h"
#include "rtc_support.h"

/* High Level Function */
#include "diskio.h"
#include "ff.h"
//#include "ff_support.h"
//#include "monitor.h"
//#include "term_io.h"
//#include "display_if.h"
//#include "display_if_support.h"
//#include "ts.h"

/* Macros */
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Externals */
extern __IO uint16_t CmdKey;
extern void Set_System(void);
extern void NVIC_Configuration(void);
extern void LED_Configuration(void);
extern void disk_timerproc(void);

#ifdef __cplusplus
}
#endif

#endif  /*__HW_CONFIG_H*/

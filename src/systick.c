/* Includes ------------------------------------------------------------------*/
#include "systick.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
__IO uint32_t TimingDelay;

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    @brief	SysTickTimer Initialize(every 1mSec)
	@param	interval: Set Systick timer interval ratio.
    @retval	none
*/
/**************************************************************************/
void SysTickInit(__IO uint32_t interval)
{
	/* Making MilliSecond-Order Timer */
	/* Select Clock Source  */
	SystemCoreClockUpdate();
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(SystemCoreClock/ interval))
	{ 
		/* Capture error */ 
		while (1);
	}

	/* Making MicroSecond-Order Timer */
	/* Enable timer clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / USEC_INTERVAL) - 1;
	TIM_TimeBaseStructure.TIM_Period = UINT32_MAX; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/* Enable counter */
	TIM_Cmd(TIM5, ENABLE);

}


/**************************************************************************/
/*! 
    @brief	Delay Millisecond Order (upto 16777214mSec!)
	@param	mSec: 24-bit value due to systick downcount timer
    @retval	none
*/
/**************************************************************************/
void _delay_ms(__IO uint32_t mSec)
{ 
	TimingDelay = mSec;

	while(TimingDelay != 0);
}

/**************************************************************************/
/*! 
    @brief	Delay Microsecond Order (upto 4294967296uSec!)
	@param	uSec: 32-bit value due to TIM5 upcount timer
    @retval	none
*/
/**************************************************************************/
void _delay_us(__IO uint32_t uSec)
{

#if 1
	volatile uint16_t start = TIM5->CNT;
	/* use 32 bit count wrap around */
	while((uint16_t)(TIM5->CNT - start) <= uSec);

#else
	/* This is the stupid method */
	while(uSec--){ 
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					}
#endif
}

/**************************************************************************/
/*! 
    @brief	Delay Stupid.
	@param	nCount
    @retval	none
*/
/**************************************************************************/
void Delay(__IO uint32_t nCount)
{
	while(nCount--){ 
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					__NOP();
					}
}

/* End Of File ---------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "hw_config.h"

/* Defines -------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/;

/* Prototypes ----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**************************************************************************/
/*! 
    @brief	Configures Main system clocks & power.
	@param	None.
    @retval	None.
*/
/**************************************************************************/
void Set_System(void)
{
	/* SystemInit was already executed in asm startup */

	/* NVIC configuration */
	NVIC_Configuration();

}


/**************************************************************************/
/*! 
    @brief	Configures the LED on STM3220G-EVAL.
	@param	None.
    @retval	None.
*/
/**************************************************************************/
void LED_Configuration(void)
{
}


/**************************************************************************/
/*! 
    @brief	Configures the KEY-Input on STM3220G-EVAL.
	@param	None.
    @retval	None.
*/
/**************************************************************************/
void KEY_Configuration(void)
{
}


/**************************************************************************/
/*! 
    @brief	Configures Vector Table base location.
	@param	None.
    @retval	None.
*/
/**************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_SetPriority(SysTick_IRQn, 0x0);	
}

/* End Of File ---------------------------------------------------------------*/

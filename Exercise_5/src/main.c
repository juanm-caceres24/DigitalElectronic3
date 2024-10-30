/**
 * @file main.c
 * @brief
 *
 */

/* --- INCLUDEs --- */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"    /* GPIO handling */
#include "lpc17xx_pinsel.h"  /* Pin function selection */
#include "lpc17xx_systick.h" /* Systic handling */
#include "lpc17xx_timer.h"   /* Timer handling */
#include "lpc17xx_adc.h"     /* ADC handling */
#include "lpc17xx_dac.h"     /* DAC handling */
#include "lpc17xx_gpdma.h"   /* DMA handling */

/* --- DEFINEs and TYPEDEFs --- */

/**
 * @brief GPIO I/O  defines.
 *
 */
#define INPUT 0
#define OUTPUT 1

/**
 * @brief Boolean values defines.
 *
 */
#define TRUE 1
#define FALSE 0
#define HIGH 1
#define LOW 0

/**
 * @brief Interrupt edge defines.
 *
 */
#define RISING_EDGE 0
#define FALLING_EDGE 1

/**
 * @brief Systick timer defines.
 *
 */
#define SYSTICK_TIME 100 /* Interrupt time for systick in [ms] */

/**
 * @brief Timer defines.
 *
 */
#define SECOND_IN_US 1000000 /* 1 second in [us] */

/**
 * @brief ADC defines.
 *
 */
#define ADC_FREQ 100000 /* ADC frequency in [Hz] */

/**
 * @brief Ports pins defines.
 *
 */
#define PWM_SIGNAL_INPUT_PIN ((uint32_t)(1 << 4)) /* P0.4 connected to PWM signal input. */
#define DAC_OUTPUT_PIN ((uint32_t)(1 << 26))      /* P0.26 connected to DAC signal output. */

/* --- Global variables --- */

/**
 * @brief ADC variables.
 *
 */
static uint32_t adc_value = 0; /* ADC conversion value */

/* --- Methods --- */

/**
 * @brief Setup the system.
 *
 */
void setup(void)
{
    /* Initialize the system clock and power (default clock: 100 [MHz]) */
    SystemInit();

    /* Load the configuration of each peripheral. */
    config_GPIO_ports();
    config_SysTick();
    config_timer();
    config_ADC();
    config_DAC();
    config_DMA();
}

/**
 * @brief Initialize the GPIO peripherals.
 *
 */
void config_GPIO_ports(void)
{
    PINSEL_CFG_Type pin_cfg;

    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_4;
    pin_cfg.Funcnum = PINSEL_FUNC_3;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_26;
    pin_cfg.Funcnum = PINSEL_FUNC_2;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0, PWM_SIGNAL_INPUT_PIN, INPUT);
    GPIO_SetDir(PINSEL_PORT_0, DAC_OUTPUT_PIN, OUTPUT);
}

/**
 * @brief Configure systick timer.
 *
 */
void config_SysTick(void)
{
}

/**
 * @brief Configure timer 0.
 *
 */
void config_timer(void)
{
    
}

/**
 * @brief Configure ADC.
 *
 */
void config_ADC(void)
{
}

/**
 * @brief Configure DAC.
 *
 */
void config_DAC(void)
{
}

/**
 * @brief Configure DMA.
 *
 */
void config_DMA(void)
{
}

/**
 * @brief Start the interruptions.
 *
 */
void start_int(void)
{
}

/**
 * @brief Start the Systick timer.
 *
 */
void start_SysTick(void)
{
}

/**
 * @brief Start the timer 0.
 *
 */
void start_timer(void)
{
}

/**
 * @brief Start the ADC.
 *
 */
void start_ADC(void)
{
}

/**
 * @brief Overwrite the interrupt handler routine for GPIO.
 *
 */
void EINT3_IRQHandler(void)
{
}

/**
 * @brief Overwrite the Systick handler routine.
 *
 */
void SysTick_Handler(void)
{
}

/**
 * @brief Overwrite the Timer 0 handler routine.
 *
 */
void TIMER0_IRQHandler(void)
{
}

/**
 * @brief Overwrite the ADC handler routine.
 *
 */
void ADC_IRQHandler(void)
{
}

/* --- Main method --- */

/**
 * @brief Main function.
 *
 */
int main(void)
{
    setup();

    while (TRUE)
    {
        /* Wait for interrupts. */
    }

    return 0; /* Program should never reach this point. */
}

/**
 * @file main.c
 * @brief This project demonstrates how to control a door configurating GPIO pins and systick in the LPC1769 using CMSIS.
 * The GPIO pins are configured as output and input, and the LED connected to P0.6 is toggled Faster or slower (using systick)
 * based on the state of the battery (simulated with P0.28 - P0.30 buttons).
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"    /* GPIO handling */
#include "lpc17xx_pinsel.h"  /* Pin function selection */
#include "lpc17xx_systick.h" /* Systic * SEk handling */
#include "lpc17xx_timer.h"   /* Timer handling */
#include "lpc17xx_adc.h"     /* ADC handling */

/* Pin Definitions */

/* GPIO I/O Definitions */
#define INPUT 0
#define OUTPUT 1

/* Boolean Values */
#define TRUE 1
#define FALSE 0

/* Interrupt level or edge */
#define RISING_EDGE 0
#define FALLING_EDGE 1

/* Systick time */
#define SYSTICK_TIME 100 /* Interrupt time for systick in [ms] */

/* Timer */
#define SECOND_IN_US 1000000 /* 1 second in [us] */

/* ADC frequency */
#define ADC_FREQ 100000 /* ADC frequency in [Hz] */

/* Port 0 */
#define RED_LED_PIN ((uint32_t)(1 << 22))   /* P0.20 connected to RED_LED */
#define GREEN_LED_PIN ((uint32_t)(1 << 20)) /* P0.21 connected to GREEN_LED */
#define BLUE_LED_PIN ((uint32_t)(1 << 21))  /* P0.22 connected to BLUE_LED */
#define ADC_INPUT ((uint32_t)(1 << 2))      /* P0.2 connected to ADC_INPUT */

static uint32_t adc_value = 0;

/**
 * @brief Initialize the GPIO peripherals
 *
 */
void configure_GPIO_ports(void)
{
    PINSEL_CFG_Type pin_cfg;

    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_20;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum = PINSEL_PIN_21;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum = PINSEL_PIN_22;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum = PINSEL_PIN_2;
    pin_cfg.Funcnum = PINSEL_FUNC_2;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0, RED_LED_PIN | GREEN_LED_PIN | BLUE_LED_PIN, OUTPUT);
}

/**
 * @brief Configurate systick with internal clock to interrupt every SYSTICK_TIME
 *
 */
void configure_SysTick(void)
{
    SYSTICK_InternalInit(SYSTICK_TIME);
}

void configure_timer(void)
{
    TIM_TIMERCFG_Type timer_cgf;
    TIM_MATCHCFG_Type match_cfg;

    timer_cgf.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cgf.PrescaleValue = (uint32_t)100;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cgf);

    match_cfg.MatchChannel = 0;
    match_cfg.IntOnMatch = ENABLE;
    match_cfg.StopOnMatch = DISABLE;
    match_cfg.ResetOnMatch = ENABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match_cfg.MatchValue = (uint32_t)(60 * SECOND_IN_US);
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    TIM_Cmd(LPC_TIM0, ENABLE);
}

void configure_ADC(void)
{
    ADC_Init(LPC_ADC, ADC_FREQ);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_7, ENABLE);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN7, ENABLE);
}

void start_interruptions(void)
{
    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(ADC_IRQn);
}

void start_SysTick(void)
{
}

void start_timer(void)
{
    TIM_Cmd(LPC_TIM0, ENABLE);
}

void start_ADC(void)
{
    ADC_StartCmd(LPC_ADC, ADC_START_NOW);
}

/**
 * @brief Overwrite the interrupt handler routine for GPIO
 *
 */
void EINT3_IRQHandler(void)
{

}

/**
 * @brief Overwrite the Systick handler routine
 *
 */
void SysTick_Handler(void)
{

}

void TIMER0_IRQHandler(void)
{
    NVIC_DisableIRQ(TIMER0_IRQn);

    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    start_ADC();

    NVIC_EnableIRQ(TIMER0_IRQn);
}

void ADC_IRQHandler(void)
{
    NVIC_DisableIRQ(ADC_IRQn);

    adc_value = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_7);

    NVIC_EnableIRQ(ADC_IRQn);
}

/**
 * @brief Main function.
 *
 */
int main(void)
{
    SystemInit();           /* Initialize the system clock (default: 100 MHz) */

    configure_GPIO_ports(); /* Configure GPIO pins */
    configure_SysTick();    /* Configure SysTick */
    configure_timer();      /* Configure Timer */
    configure_ADC();        /* Configure ADC */
    start_interruptions();  /* Enable interruptions */
    start_SysTick();        /* Start SysTick */
    start_timer();          /* Start Timer */

    while (TRUE)
    {
        /* Wait for interrupts */
    }
    
    return 0; /* Program should never reach this point */
}

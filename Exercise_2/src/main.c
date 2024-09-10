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

#include "lpc17xx_gpio.h"   /* GPIO handling */
#include "lpc17xx_pinsel.h" /* Pin function selection */

/* Pin Definitions */

/* Port 0 */
#define DOOR_BUTTON_PIN ((uint32_t)(1 << 3))         /* P0.3 connected to DOOR_BUTTON */
#define ENDSTOP_1_PIN ((uint32_t)(1 << 4))           /* P0.4 connected to ENSDTOP_1 */
#define ENDSTOP_2_PIN ((uint32_t)(1 << 5))           /* P0.5 connected to ENSDTOP_2 */
#define BATTERY_LED_PIN ((uint32_t)(1 << 6))         /* P0.6 connected to BATTERY_LED */
#define LOW_BATTERY_BUTTON_PIN ((uint32_t)(1 << 28)) /* P0.28 connected to LOW_BATTERY_BUTTON */
#define MID_BATTERY_BUTTON_PIN ((uint32_t)(1 << 29)) /* P0.29 connected to MID_BATTERY_BUTTON */
#define MAX_BATTERY_BUTTON_PIN ((uint32_t)(1 << 30)) /* P0.30 connected to MAX_BATTERY_BUTTON */

/* Port 1 */
#define RELAY_2_PIN ((uint32_t)(1 << 0)) /* P1.0 connected to RELAY_2 */
#define RELAY_1_PIN ((uint32_t)(1 << 1)) /* P1.1 connected to RELAY_1 */

/* GPIO I/O Definitions */
#define INPUT 0
#define OUTPUT 1

/* Boolean Values */
#define TRUE 1
#define FALSE 0

/* Interrupt level or edge */
#define RISING_EDGE 0

/* Battery level status */
#define MAX_BATTERY (uint8_t)2
#define MID_BATTERY (uint8_t)1
#define LOW_BATTERY (uint8_t)0

/* Systick time */
#define SYSTICK_TIME 100 // Interrupt time for systick in [ms]

uint8_t systick_counter = 0;         // Counter for systick
uint8_t battery_level = MAX_BATTERY; // It can be 2(max), 1(mid), 0(low)
uint8_t is_closed = 1;

/**
 * @brief Initialize the GPIO peripheral
 *
 */
void configure_GPIO_port(void)
{
    PINSEL_CFG_Type pin_cfg; /* Create a variable to store the configuration of the pins */

    /* We need to configure the struct with the desired configuration */

    // START CONFIGURATION FOR DOOR_BUTTON_PIN
    pin_cfg.Portnum = PINSEL_PORT_0;           /* The port number is 0 */
    pin_cfg.Pinnum = PINSEL_PIN_3;             /* The pin number is 3 */
    pin_cfg.Funcnum = PINSEL_FUNC_0;           /* The function number is 0 */
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLDOWN; /* The pin mode is pull-down */
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL; /* The pin is in the normal mode */
    PINSEL_ConfigPin(&pin_cfg);                /* Configure the pin */

    // START CONFIG FOR ENDSTOP_1_PIN
    pin_cfg.Pinnum = PINSEL_PIN_4;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR ENDSTOP_2_PIN
    pin_cfg.Pinnum = PINSEL_PIN_5;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR LED_PIN
    pin_cfg.Pinnum = PINSEL_PIN_6;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR RELAY_2
    pin_cfg.Portnum = PINSEL_PORT_1;
    pin_cfg.Pinnum = PINSEL_PIN_0;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR RELAY_1
    pin_cfg.Pinnum = PINSEL_PIN_1;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR LOW_BATTERY_PIN
    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_28;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR MID_BATTERY_PIN
    pin_cfg.Pinnum = PINSEL_PIN_29;
    PINSEL_ConfigPin(&pin_cfg);

    // START CONFIG FOR MAX_BATTERY_PIN
    pin_cfg.Pinnum = PINSEL_PIN_30;
    PINSEL_ConfigPin(&pin_cfg);

    /* Set the pins as input or output */
    GPIO_SetDir(PINSEL_PORT_0, DOOR_BUTTON_PIN | ENDSTOP_1_PIN | ENDSTOP_2_PIN | LOW_BATTERY_BUTTON_PIN | MID_BATTERY_BUTTON_PIN | MAX_BATTERY_BUTTON_PIN, INPUT); /* P0.3 connected to DOOR BUTTON */

    GPIO_SetDir(PINSEL_PORT_0, BATTERY_LED_PIN, OUTPUT);
    /* P0.6 connected to LED */
    GPIO_SetDir(PINSEL_PORT_1, RELAY_2_PIN | RELAY_1_PIN, OUTPUT); /* P1.0 connected to RELAY2 */

    /*Interrupt habilitations*/
    GPIO_IntCmd(PINSEL_PORT_0, DOOR_BUTTON_PIN | ENDSTOP_1_PIN | ENDSTOP_2_PIN | LOW_BATTERY_BUTTON_PIN | MID_BATTERY_BUTTON_PIN | MAX_BATTERY_BUTTON_PIN, RISING_EDGE);
}

/**
 * @brief Enable current flow from relay1 to relay 2 closing the door
 *
 */
void close_door(void)
{
    GPIO_ClearValue(PINSEL_PORT_0, RELAY_1_PIN); // le damos corriente al relay1 (si le damos corriente lo mandamos a fase)
    GPIO_SetValue(PINSEL_PORT_0, RELAY_2_PIN);   // no le damos corriente por lo que va a neutro
    // NOTA los relay estan con un transistor pnp por lo que ponerlo a 0 darles corriente
}

/**
 * @brief Enable current flow from relay2 to relay1 opening the door
 *
 */
void open_door(void)
{                                                // Corriente fluye del relay2 al relay1
    GPIO_SetValue(PINSEL_PORT_0, RELAY_1_PIN);   // no le damos corriente por lo que va a neutro
    GPIO_ClearValue(PINSEL_PORT_0, RELAY_2_PIN); // le damos corriente al relay2 (si le damos corriente lo mandamos a fase)
    // NOTA los relay estan con un transistor pnp por lo que ponerlo a 0 darles corriente
}

/**
 * @brief Disable current flow from relay2 to relay1 and vice versa stopping the motor
 * it will be use when a endstop is activated
 *
 */
void stop_motor(void)
{
    GPIO_SetValue(PINSEL_PORT_0, RELAY_1_PIN);
    GPIO_SetValue(PINSEL_PORT_0, RELAY_1_PIN);
}

/**
 * @brief Open or Close the door based in is_closed variable
 *
 */
void toggle_DOOR(void)
{
    if (is_closed == 1)
    {
        open_door();
        is_closed == 0;
    }
    else
    {
        close_door();
        is_closed == 1;
    }
}

/**
 * @brief Overwrite the interrupt handler routine for GPIO
 * Toggle door if DOOR_BUTTON has been presioned
 * Stop the motor if the interrupt has been caused by the anyone of the endstops
 * Change battery value if anyone of the three buttons has been presioned
 *
 */
void EINT3_IRQHandler(void)
{
    if (GPIO_GetIntStatus(PINSEL_PORT_0, DOOR_BUTTON_PIN, RISING_EDGE == ENABLE))
    {
        toggle_DOOR();
    }
    else if (GPIO_GetIntStatus(PINSEL_PORT_0, ENDSTOP_1_PIN, RISING_EDGE) == ENABLE || GPIO_GetIntStatus(PINSEL_PORT_0, ENDSTOP_2_PIN, RISING_EDGE) == ENABLE)
    {
        stop_motor();
    }
    else if (GPIO_GetIntStatus(PINSEL_PORT_0, LOW_BATTERY_BUTTON_PIN, RISING_EDGE) == ENABLE)
    {
        battery_level = LOW_BATTERY;
    }
    else if (GPIO_GetIntStatus(PINSEL_PORT_0, MID_BATTERY_BUTTON_PIN, RISING_EDGE) == ENABLE)
    {
        battery_level = MID_BATTERY;
    }
    else if (GPIO_GetIntStatus(PINSEL_PORT_0, MAX_BATTERY_BUTTON_PIN, RISING_EDGE) == ENABLE)
    {
        battery_level = MAX_BATTERY;
    }
}

/**
 * @brief Configurate systick with internal clock to interrupt every SYSTICK_TIME
 *
 */
void configure_Systick(void)
{
    SYSTICK_InternalInit(SYSTICK_TIME);
}

/**
 * @brief Toggle the LED based in previous status
 *
 */
void toggle_LED(void)
{
    if (GPIO_ReadValue(PINSEL_PORT_0) & BATTERY_LED_PIN)
    {
        GPIO_ClearValue(PINSEL_PORT_0, BATTERY_LED_PIN);
    }
    else
    {
        GPIO_SetValue(PINSEL_PORT_0, BATTERY_LED_PIN);
    }
}

/**
 * @brief Overwrite the Systick handler routine
 * Determine the led light frecuency based on the battery status
 * Increase a counter to can count one second (count = 10) or 400ms (count =4)
 *
 */
void SysTick_Handler(void)
{
    systick_counter++;
    if (battery_level == MAX_BATTERY)
    {
        GPIO_ClearValue(PINSEL_PORT_0, BATTERY_LED_PIN);
    }
    else if (battery_level == MID_BATTERY && systick_counter == 10)
    {
        toggle_LED();
        systick_counter = 0;
    }
    else if (battery_level == LOW_BATTERY && systick_counter == 4)
    {
        toggle_LED();
        systick_counter = 0;
    }
    return;
}

/**
 * @brief Main function.
 * Initializes the system and toggles the LED based on the input pin state.
 *
 */
int main(void)
{
    SystemInit();           /* Initialize the system clock (default: 100 MHz) */
    configure_GPIO_port();  /* Configure GPIO pins */
    configure_Systick();    /* Configure SysTick */
    SYSTICK_IntCmd(ENABLE); /* Enable SysTick interrupts */
    SYSTICK_Cmd(ENABLE);    /* Enable SysTick counter */
    while (TRUE)
    {
        /* Wait for interrupts */
    }
    return 0; /* Program should never reach this point */
}

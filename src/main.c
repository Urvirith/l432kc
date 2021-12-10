//DUMMY MAIN TO ALLOW FOR THE TURN ON AND OFF OF LED IN C
//USED TO TROUBLE SHOOT REST OF CODE

#include "board/l432kc.h"
#include "hal/common.h"
#include "hal/gpio.h"
#include "hal/rcc.h"
#include "hal/timer.h"
#include "hal/usart.h"
#include "hal/i2c.h"
#include "hal/nvic.h"
#include "main.h"


/*---------------------*/
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE) 
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)                 /* GPIO Structure Declare */
#define TIMER2              ((TIMER_TypeDef *) TIMER2_BASE)
#define TIMER7              ((TIMER_TypeDef *) TIMER7_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define NVIC                ((NVIC_TypeDef *) NVIC_BASE)

extern void system_init() {
    rcc_write_msi_range(RCC, Clk16MHz);
    rcc_write_ahb2_enr(RCC, GPIOA_RCC_AHB2_ENABLE);
    rcc_write_ahb2_enr(RCC, GPIOB_RCC_AHB2_ENABLE);
    rcc_write_apb1_enr1(RCC, TIMER2_RCC_APB1R1_ENABLE);
    rcc_write_apb1_enr1(RCC, TIMER7_RCC_APB1R1_ENABLE);
    rcc_write_apb1_enr1(RCC, USART2_RCC_APB1R1_ENABLE);
    rcc_write_apb1_enr1(RCC, I2C1_RCC_APB1R1_ENABLE);
}

extern void start() { 
    /* USER LED SETUP */
    gpio_type(GPIOB, USER_LED, Gpio_Output, Gpio_Push_Pull, AF0);
    /* USART SETUP */
    gpio_type(GPIOA, USART2_TX, Gpio_Alternate, Gpio_Push_Pull, USART2_GPIO_AF);
    gpio_type(GPIOA, USART2_RX, Gpio_Alternate, Gpio_Push_Pull, USART2_GPIO_AF);
    /* I2C SETUP */
    gpio_type(GPIOB, I2C1_SCL, Gpio_Alternate, Gpio_Open_Drain, I2C1_GPIO_AF);
    gpio_type(GPIOB, I2C1_SDA, Gpio_Alternate, Gpio_Open_Drain, I2C1_GPIO_AF);
    gpio_speed(GPIOB, I2C1_SCL, Gpio_Low_Speed);
    gpio_speed(GPIOB, I2C1_SDA, Gpio_Low_Speed);
    gpio_pupd(GPIOB, I2C1_SCL, Gpio_NoPuPd);
    gpio_pupd(GPIOB, I2C1_SDA, Gpio_NoPuPd);
    /* TIMER SETUP */
    timer_open(TIMER2, Timer_Cont, Timer_Upcount);
    timer_set_time(TIMER2, 500, 16000, 1500);
    timer_start(TIMER2);

    timer_open(TIMER7, Timer_Ons, Timer_Upcount);
    timer_set_interrupt(TIMER7);
    timer_ons_delay(TIMER7, 500);

    /* DRIVER SETUP */
    usart_open(USART2, USART_8_Bits, USART_1_StopBit, USART_9600_BAUD, 16000, USART_Oversample_16);

    while (!timer_get_flag(TIMER2)) {

    }
    timer_clr_flag(TIMER2);

    nvic_enable_interrupt(NVIC, TIM7_IRQ);
    timer_set_time(TIMER7, 1000, 16000, 1500);
    timer_start(TIMER7);

    uint8_t buf_fxas[8] = {0x03, 0x00, 0x00, 0x00 ,0x00, 0x00, 0x00, 0x0D};
    uint16_t buffer_fxas[3] = {0, 0, 0};
    //uint8_t buf_fxos[14] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ,0x00, 0x00, 0x00, 0x0D};
    //uint16_t buffer_fxos[6] = {0, 0, 0, 0, 0, 0};

    int i = 0;
    
    while (1) {
        if (timer_get_flag(TIMER2)) {

            /* MAIN LED SEQUENCE HERE */
            timer_clr_flag(TIMER2);
        }
    }
}

extern void __aeabi_unwind_cpp_pr0() {
    //loop {}
}

volatile uint8_t led = 0;

/* BEING USED TO TEST THE NVIC */
extern void TIM7_IRQHandler() { 
    timer_clr_flag(TIMER7);

    if (led == 0) {
        gpio_set_pin(GPIOB, USER_LED_BIT);
        led = 1;
    } else {
        gpio_clr_pin(GPIOB, USER_LED_BIT);
        led = 0;
    }

    timer_start(TIMER7);
}

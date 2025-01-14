#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "hardware/pwm.h"

uint forwardGPIO = 20;
uint backwardGPIO = 21;

void displayInit()
{
    gpio_set_function(0, GPIO_FUNC_I2C); // A2
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    uart_init(uart0, 115200);
    i2c_init(i2c0, 100000);

    {
        uint8_t cmd = 0x21; // 0010 0001 System Setup
        i2c_write_blocking(i2c0, 0x70, &cmd, 1, false);
    }
    {
        uint8_t cmd = 0x81; // 1000 0001 Display Setup
        i2c_write_blocking(i2c0, 0x70, &cmd, 1, false);
    }
    {
        uint8_t cmd = 0xE0; // 1110 0000 Dimming Set
        i2c_write_blocking(i2c0, 0x70, &cmd, 1, false);
    }

    volatile uint8_t zeilen[8] = {
        0x00, // Zeile 1 //A5
        0x00, // Zeile 2
        0x00, // Zeile 3
        0x00, // Zeile 4
        0x00, // Zeile 5
        0x00, // Zeile 6
        0x00, // Zeile 7
        0x00  // Zeile 8
    };
}

void echoInit()
{
    displayInit();
    timer_hw->dbgpause = 0;
    gpio_init(27);
    gpio_init(28);
    gpio_set_dir(28, GPIO_OUT);
}

void setupEchoTriggerPWM()
{
    gpio_set_function(28, GPIO_FUNC_PWM); // Tell GPIO 28 it is allocated to the PWM
    // uint slice_num = pwm_gpio_to_slice_num(6); // Find out which PWM slice is connected to GPIO 28 (it's slice 6)
    pwm_set_clkdiv(6, 125);
    pwm_set_wrap(6, 60000);
    pwm_set_chan_level(6, PWM_CHAN_A, 10);
    pwm_set_enabled(6, true);
}

void setDisplay(volatile uint progress)
{
    uint displayedProgress;
    if(progress>200){
        displayedProgress = 200;
    }else{
        displayedProgress = progress;
    }

    volatile uint8_t zeilen[8] = {
        0x00, // Zeile 1 //A5
        0x00, // Zeile 2
        0x00, // Zeile 3
        0x00, // Zeile 4
        0x00, // Zeile 5
        0x00, // Zeile 6
        0x00, // Zeile 7
        0x00  // Zeile 8
    };

    volatile uint anzahlLed = displayedProgress * 0.32;
    volatile uint nrFullRows = anzahlLed / 8;
    for (volatile uint i = 0; i < nrFullRows; i++)
    {
        zeilen[i] = 0xFF;
    }
    if (nrFullRows < 8)
    {
        volatile uint restLed = anzahlLed % 8;
        if (restLed > 0)
        {
            volatile uint restZeile = nrFullRows;
            zeilen[restZeile] = 0xFF >> (8 - restLed);
        }
    }

    uint8_t data1[] = {0x00, // Anfangszeile
                       zeilen[0], 0x00,
                       zeilen[1], 0x00, /*0011 0110*/
                       zeilen[2], 0x00,
                       zeilen[3], 0x00,
                       zeilen[4], 0x00,
                       zeilen[5], 0x00,
                       zeilen[6], 0x00,
                       zeilen[7], 0x00};

    i2c_write_blocking(i2c0, 0x70, data1, sizeof(data1), false);
}

volatile uint32_t duration[3] = {0, 0, 0};
uint8_t durationIndex = 0;
void gpio_callback(uint gpio, uint32_t events)
{
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        uint slice_num = pwm_gpio_to_slice_num(27); // Find out which PWM slice is connected to GPIO 27 (it's slice 0)
        duration[durationIndex] = pwm_get_counter(slice_num);
        durationIndex = (durationIndex + 1) % 3;
        pwm_set_counter(slice_num, 0);
    }
}

void echoOutputPWM()
{
    uint slice_num = pwm_gpio_to_slice_num(27); // Find out which PWM slice is connected to GPIO 27 (it's slice 0)
    gpio_set_function(27, GPIO_FUNC_PWM);       // Tell GPIO 27 it is allocated to the PWM
    pwm_set_clkdiv(slice_num, 125);
    pwm_set_clkdiv_mode(slice_num, PWM_DIV_B_HIGH);
    pwm_set_enabled(slice_num, true);
    gpio_set_irq_enabled_with_callback(27, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void initMotor()
{
    gpio_init(19);
    gpio_set_dir(19, GPIO_OUT);
    gpio_set_function(19, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(19); // Find out which PWM slice is connected to GPIO 27 (it's slice 0)
    pwm_set_clkdiv(slice_num, 125);
    pwm_set_wrap(slice_num, 10000);
    pwm_set_enabled(slice_num, true);

    gpio_pull_down(forwardGPIO);  // Vorwärts
    gpio_pull_down(backwardGPIO); // Rückwärts
    for (volatile uint i = 0; i < 1000000; i++)
        ;
}

int main()
{
    echoInit();
    setupEchoTriggerPWM();
    echoOutputPWM();
    initMotor();

    volatile uint previousProgress = 1;

    // gpio_pull_down(20);
    // gpio_pull_up(21);

    // pwm_set_chan_level(1, PWM_CHAN_B, 2000); //Losfahren 1900/10000
    // for(volatile uint i=0;i<1000000;i++);

    while (true)
    {
        uint32_t durationAvg = (duration[0] + duration[1] + duration[2]) / 3;
        volatile uint32_t distance = (durationAvg * 340 / 2) / 100;
        uint target = 10;                           // cm
        volatile uint progress = distance / target; // A6gpio_init(19);
        gpio_set_dir(19, GPIO_OUT);
        gpio_set_function(19, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(19); // Find out which PWM slice is connected to GPIO 27 (it's slice 0)
        pwm_set_clkdiv(slice_num, 125);
        pwm_set_wrap(slice_num, 10000);
        if (progress > 1000)
        {
            progress = 1000;
        }

        if (previousProgress != progress)
        {
            setDisplay(progress);
            previousProgress = progress;
        }

        if (progress > 101)
        {
            gpio_pull_up(forwardGPIO);    // Vorwärts
            gpio_pull_down(backwardGPIO); // Rückwärts
            uint speed = (progress-100) * 9 + 1900;
            pwm_set_chan_level(slice_num, PWM_CHAN_B, speed); // Losfahren 2000/10000 Fährt nicht < 900
        }
        else if (progress <= 99)
        {
            gpio_pull_down(forwardGPIO);  // Vorwärts
            gpio_pull_up(backwardGPIO); // Rückwärts
            uint speed = (100-progress) * 9 + 1900;
            pwm_set_chan_level(slice_num, PWM_CHAN_B, speed); // Losfahren 2000/10000 Fährt nicht < 900
        }
        else
        {
            gpio_pull_down(forwardGPIO);  // Vorwärts
            gpio_pull_down(backwardGPIO); // Rückwärts
        }
    }

    return 0;
}
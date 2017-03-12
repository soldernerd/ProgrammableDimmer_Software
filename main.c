/* Main file for Programmable Dimmer
 * 
 * File:   main.c
 * Author: Lukas Faessler
 * Visit www.soldernerd.com for schematics, photos and more
 *
 * Created on 2017-02-23
 */

#include <xc.h>
#include <stdint.h>
#include <pic16f18325.h>

#include "encoder.h"
#include "pwm.h"

// PIC16F18325 Configuration Bit Settings
// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with 2x PLL (32MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)
// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = SBOREN   // Brown-out Reset Enable bits (Brown-out Reset enabled according to SBOREN)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = OFF    // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be set and cleared repeatedly (subject to the unlock sequence))
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)
// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)
// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

//CPU Frequency
#define _XTAL_FREQ 32000000

//Enable N-FET on pin 13, RA0
#define ENABLE_TRIS TRISAbits.TRISA0
#define ENABLE_PIN PORTAbits.RA0
#define ENABLE_PORT PORTA
#define ENABLE_VARIABLE portA
#define ENABLE_MASK 0b00000001


//Global variables
uint8_t portA = 0x00;
uint8_t portC = 0x00;


int8_t encoder1_count = 0;
int8_t encoder2_count = 0;

uint16_t brightness_levels[16] = {0,5,41,90,138,165,198,238,286,343,411,493,592,710,853,1023};
uint8_t brightness1 = 0;
uint8_t brightness2 = 0;
uint8_t brightness3 = 0;
uint8_t brightness4 = 0;



//Function prototypes
void output1_on(void);
void output1_off(void);
void output2_on(void);
void output2_off(void);



/*
//Temperature measurement
uint8_t adc_count;
uint32_t adc_sum;
uint16_t adc_voltage; //in millivolts
uint16_t temperature; //in 0.1 degrees centigrade
uint8_t ready;

//Pulse width modulation (PWM)
uint16_t duty_cycle;

//LED indicator
uint16_t led_count;

//Fan speed measurement
uint16_t tach_capture_0;
uint16_t tach_capture_1;
uint16_t tach_measurement;
uint16_t actual_fan_speed; //in 10 revolutions per minute, e.g. 450=4500rpm

//Target fan speed
uint16_t target_fan_speed; //in 10 revolutions per minute, e.g. 450=4500rpm 
 * 
 * */

void interrupt _isr(void)
{
    encoder_isr();
}


void output1_on(void)
{
    OUTPUT1_VARIABLE |= OUTPUT1_MASK;
    OUTPUT1_PORT = OUTPUT1_VARIABLE;
}

void output1_off(void)
{
    OUTPUT1_VARIABLE &= ~OUTPUT1_MASK;
    OUTPUT1_PORT = OUTPUT1_VARIABLE;   
}

void output2_on()
{
    OUTPUT2_VARIABLE |= OUTPUT2_MASK;
    OUTPUT2_PORT = OUTPUT2_VARIABLE;
}

void output2_off()
{
    OUTPUT2_VARIABLE &= ~OUTPUT2_MASK;
    OUTPUT2_PORT = OUTPUT2_VARIABLE;   
}

void init(void)
{
    //Configure pin 13 as output and drive it high immediately
    //This enables the N-FET and ensures power stays on
    ENABLE_TRIS = 0;
    ENABLE_PIN = 1;
    ENABLE_VARIABLE |= ENABLE_MASK;
    
    //Initialize PWM
    pwm_init();
    
    //Enable encoder
    encoder_init();
}

void main(void) 
{
    int16_t count = 0;
    init(); 
    
    while(1)
    {

        if(encoder1_count>0)
        {
            if(brightness1<15)
            {
                ++brightness1;
                pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness1]);
                pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness1]);
            }
            encoder1_count = 0;
        }   
        if(encoder1_count<0)
        {
            if(brightness1>0)
            {
                --brightness1;
                pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness1]);
                pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness1]);
            }
            encoder1_count = 0;
        }  
        
        if(encoder2_count>0)
        {
            if(brightness3<15)
            {
                ++brightness3;
                pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness3]);
                pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness3]);
            }
            encoder2_count = 0;
        }   
        if(encoder2_count<0)
        {
            if(brightness3>0)
            {
                --brightness3;
                pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness3]);
                pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness3]);
            }
            encoder2_count = 0;
        }    
                
        __delay_ms(1);
        
        //Turn off power supply after 5 seconds of inactivity
        if(brightness1==0 && brightness2==0 && brightness3==0 && brightness4==0)
        {
            ++count;
            if(count==5000)
            {
                ENABLE_PIN = 0;
            }
        }
        else
        {
            count = 0;
        }
    }
}

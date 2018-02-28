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
//#include <pic16f18325.h>

#include "config.h"
#include "encoder.h"
#include "pwm.h"


#ifdef FUNCTION_DUALWHITE
    #include "function_dualwhite.h"
#endif

#ifdef FUNCTION_RGB
    #include "function_rgb.h"
#endif

#ifdef FUNCTION_FREQUENCY
    #include "function_frequency.h"
#endif



//Enable N-FET on pin 13, RA0
#define ENABLE_TRIS TRISAbits.TRISA0
#define ENABLE_PIN PORTAbits.RA0
#define ENABLE_PORT PORTA
#define ENABLE_VARIABLE portA
#define ENABLE_MASK 0b00000001

//Global variables
uint8_t portA = 0x00;
uint8_t portC = 0x00;
int8_t encoder1_count;
uint8_t encoder1_button;
int8_t encoder2_count;
uint8_t encoder2_button;

//Function prototypes
void turn_on(void);
void turn_off(void);


void interrupt _isr(void)
{
    #ifdef FUNCTION_FREQUENCY
        if(PIR1bits.TMR1IF)
        {
            timer1_isr();
            return;
        }
    #endif /*FUNCTION_FREQUENCY*/

    encoder_isr();
}

void turn_on(void)
{
    ENABLE_TRIS = 0;
    ENABLE_PIN = 1;
}

void turn_off(void)
{
    ENABLE_PIN = 0;
}

void init(void)
{
    //Configure pin 13 as output and drive it high immediately
    //This enables the N-FET and ensures power stays on
    turn_on();
    
    //Initialize PWM
    pwm_init();
    
    //Enable encoder
    encoder_init();
}

void main(void) 
{
    init(); 
    
    //Call the function-specific main loop
    #ifdef FUNCTION_DUALWHITE
        main_loop_dualwhite();
    #endif

    #ifdef FUNCTION_RGB
        main_loop_rgb();
    #endif

    #ifdef FUNCTION_FREQUENCY
        main_loop_frequency();
    #endif
}


#include <xc.h>
#include <stdint.h>
#include "pwm.h"

uint16_t channel_1_dutycycle = 0;
uint16_t channel_2_dutycycle = 0;
uint16_t channel_3_dutycycle = 0;
uint16_t channel_4_dutycycle = 0;

void pwm_init(void)
{
    //Configure output pins
    OUTPUT1_TRIS = 0;
    OUTPUT1_PPS = CCP1_MODULE;
    OUTPUT2_TRIS = 0;
    OUTPUT2_PPS = CCP2_MODULE;
    OUTPUT3_TRIS = 0;
    OUTPUT3_PPS = CCP3_MODULE;
    OUTPUT4_TRIS = 0;
    OUTPUT4_PPS = CCP4_MODULE;
    
    //Configure output1 using CCP1, Timer 2
    PR2 = 0xFF; // 10 bits resolution
    TMR2 = 00; // Start at zero
    T2CONbits.T2CKPS = 0b00; //prescaler=1
    T2CONbits.T2OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C1TSEL = 0b00; // Select timer 2 for CCP1 module
    CCP1CONbits.CCP1FMT = 1; //Left aligned
    CCP1CONbits.CCP1MODE = 0b1111; //PWM mode
    CCPR1H = 0x00; //Most significant bits
    CCPR1L = 0x00; //Least significant bits
    CCP1CONbits.CCP1EN = 1; //Module enabled
    
    //Configure output2 using CCP2, Timer 4
    PR4 = 0xFF; // 10 bits resolution
    TMR4 = 85; // Start at 85, 120 degrees out-of-phase
    T4CONbits.T4CKPS = 0b00; //prescaler=1
    T4CONbits.T4OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C2TSEL = 0b10; // Select timer 4 for CCP2 module
    CCP2CONbits.CCP2FMT = 1; //Left aligned
    CCP2CONbits.CCP2MODE = 0b1111; //PWM mode
    CCPR2H = 0x00; //Most significant bits
    CCPR2L = 0x00; //Least significant bits
    CCP2CONbits.CCP2EN = 1; //Module enabled
    
    //Configure output3 using CCP3, Timer 6
    PR6 = 0xFF; // 10 bits resolution
    TMR6 = 170; // Start at 170, 240 degrees out-of-phase
    T6CONbits.T6CKPS = 0b00; //prescaler=1
    T6CONbits.T6OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C3TSEL = 0b11; // Select timer 6 for CCP3 module
    CCP3CONbits.CCP3FMT = 1; //Left aligned
    CCP3CONbits.CCP3MODE = 0b1111; //PWM mode
    CCPR3H = 0x00; //Most significant bits
    CCPR3L = 0x00; //Least significant bits
    CCP3CONbits.CCP3EN = 1; //Module enabled
    
    //Configure output3 using CCP4, Timer 2
    CCPTMRSbits.C4TSEL = 0b00; // Select timer 2 for CCP4 module
    CCP4CONbits.CCP4FMT = 1; //Left aligned
    CCP4CONbits.CCP4MODE = 0b1111; //PWM mode
    CCPR4H = 0x00; //Most significant bits
    CCPR4L = 0x00; //Least significant bits
    CCP4CONbits.CCP4EN = 1; //Module enabled
    
    T2CONbits.TMR2ON = 1;
    T4CONbits.TMR4ON = 1;
    T6CONbits.TMR6ON = 1;
}

uint16_t pwm_get_dutycycle(PwmChannel_t channel)
{
    switch(channel)
    {
        case PWM_CHANNEL_1:
            return channel_1_dutycycle;
            break;
        case PWM_CHANNEL_2:
            return channel_2_dutycycle;
            break;
        case PWM_CHANNEL_3:
            return channel_3_dutycycle;
            break;
        case PWM_CHANNEL_4:
            return channel_4_dutycycle;
            break;
    }
}

void pwm_set_dutycycle(PwmChannel_t channel, uint16_t dutycycle)
{
    switch(channel)
    {
        case PWM_CHANNEL_1:
            channel_1_dutycycle = dutycycle & 0b1111111111;
            CCPR1H = channel_1_dutycycle>>2; //Most significant bits
            CCPR1L = (channel_1_dutycycle&0b11)<<6; //Least significant bits
            break;
        case PWM_CHANNEL_2:
            channel_2_dutycycle = dutycycle & 0b1111111111;
            CCPR2H = channel_2_dutycycle>>2; //Most significant bits
            CCPR2L = (channel_2_dutycycle&0b11)<<6; //Least significant bits
            break;
        case PWM_CHANNEL_3:
            channel_3_dutycycle = dutycycle & 0b1111111111;
            CCPR3H = channel_3_dutycycle>>2; //Most significant bits
            CCPR3L = (channel_3_dutycycle&0b11)<<6; //Least significant bits
            break;
        case PWM_CHANNEL_4:
            channel_4_dutycycle = dutycycle & 0b1111111111;
            CCPR4H = channel_4_dutycycle>>2; //Most significant bits
            CCPR4L = (channel_4_dutycycle&0b11)<<6; //Least significant bits
            break;
    }    
}

void pwm_change_dutycycle(PwmChannel_t channel, int16_t change_amount)
{
    switch(channel)
    {
        case PWM_CHANNEL_1:
            change_amount += channel_1_dutycycle;
            break;
        case PWM_CHANNEL_2:
            change_amount += channel_2_dutycycle;
            break;
        case PWM_CHANNEL_3:
            change_amount += channel_3_dutycycle;
            break;
        case PWM_CHANNEL_4:
            change_amount += channel_4_dutycycle;
            break;
    }   
    if(change_amount<PWM_DUTYCYCLE_MINIMUM)
        change_amount = PWM_DUTYCYCLE_MINIMUM;
    if(change_amount>PWM_DUTYCYCLE_MAXIMUM)
        change_amount = PWM_DUTYCYCLE_MAXIMUM;
    pwm_set_dutycycle(channel, change_amount);
}
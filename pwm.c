
#include <xc.h>
#include <stdint.h>
#include "pwm.h"
#include "config.h"

uint16_t channel_1_dutycycle = 0;
uint16_t channel_2_dutycycle = 0;
uint16_t channel_3_dutycycle = 0;
uint16_t channel_4_dutycycle = 0;

void pwm_init(void)
{
    //Configure output pins
    OUTPUT1_TRIS = 0;
    OUTPUT2_TRIS = 0;
    OUTPUT3_TRIS = 0;
    OUTPUT4_TRIS = 0;
   
    PPSUnLock()
    OUTPUT1_PPS = CCP1_MODULE;  
    OUTPUT2_PPS = CCP2_MODULE;
    OUTPUT3_PPS = CCP3_MODULE;
    OUTPUT4_PPS = CCP4_MODULE;
    PPSLock()
    
    
    //Configure output1 using CCP1
    PR2 = 0xFF; // 10 bits resolution
    T2CONbits.T2CKPS = 0b00; //prescaler=1
    T2CONbits.T2OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C1TSEL = OUTPUT_1_TIMER_SOURCE;
    CCP1CONbits.CCP1FMT = 1; //Left aligned
    CCP1CONbits.CCP1MODE = 0b1111; //PWM mode
    CCPR1H = 0x00; //Most significant bits
    CCPR1L = 0x00; //Least significant bits
    CCP1CONbits.CCP1EN = 1; //Module enabled
    
    //Configure output2 using CCP2
    PR4 = 0xFF; // 10 bits resolution
    T4CONbits.T4CKPS = 0b00; //prescaler=1
    T4CONbits.T4OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C2TSEL = OUTPUT_2_TIMER_SOURCE;
    CCP2CONbits.CCP2FMT = 1; //Left aligned
    CCP2CONbits.CCP2MODE = 0b1111; //PWM mode
    CCPR2H = 0x00; //Most significant bits
    CCPR2L = 0x00; //Least significant bits
    CCP2CONbits.CCP2EN = 1; //Module enabled
    
    //Configure output3 using CCP3
    PR6 = 0xFF; // 10 bits resolution
    T6CONbits.T6CKPS = 0b00; //prescaler=1
    T6CONbits.T6OUTPS = 0b0000; //postscaler=1
    CCPTMRSbits.C3TSEL = OUTPUT_3_TIMER_SOURCE;
    CCP3CONbits.CCP3FMT = 1; //Left aligned
    CCP3CONbits.CCP3MODE = 0b1111; //PWM mode
    CCPR3H = 0x00; //Most significant bits
    CCPR3L = 0x00; //Least significant bits
    CCP3CONbits.CCP3EN = 1; //Module enabled
    
    //Configure output3 using CCP4
    CCPTMRSbits.C4TSEL = OUTPUT_4_TIMER_SOURCE;
    CCP4CONbits.CCP4FMT = 1; //Left aligned
    CCP4CONbits.CCP4MODE = 0b1111; //PWM mode
    CCPR4H = 0x00; //Most significant bits
    CCPR4L = 0x00; //Least significant bits
    CCP4CONbits.CCP4EN = 1; //Module enabled
    
    //Assign starting values to used timers
    #ifdef TIMER_2_STARTING_VALUE
        TMR2 = TIMER_2_STARTING_VALUE;
    #endif
    #ifdef TIMER_4_STARTING_VALUE
        TMR4 = TIMER_4_STARTING_VALUE;
    #endif    
    #ifdef TIMER_6_STARTING_VALUE
        TMR6 = TIMER_6_STARTING_VALUE;
    #endif
    
    //Start used tmers
    #ifdef TIMER_2_STARTING_VALUE
        T2CONbits.TMR2ON = 1;
    #endif
    #ifdef TIMER_4_STARTING_VALUE
        T4CONbits.TMR4ON = 1;
    #endif    
    #ifdef TIMER_6_STARTING_VALUE
        T6CONbits.TMR6ON = 1;
    #endif
}

void pwm_start(void)
{
    CCP1CONbits.CCP1EN = 1; //Module enabled
    CCP2CONbits.CCP2EN = 1; //Module enabled
    CCP3CONbits.CCP3EN = 1; //Module enabled
    CCP4CONbits.CCP4EN = 1; //Module enabled
    PPSUnLock()
    OUTPUT1_PPS = CCP1_MODULE;
    OUTPUT2_PPS = CCP2_MODULE;
    OUTPUT3_PPS = CCP3_MODULE;
    OUTPUT4_PPS = CCP4_MODULE;
    PPSLock()
}

void pwm_stop(void)
{
    CCP1CONbits.CCP1EN = 0; //Module disabled
    CCP2CONbits.CCP2EN = 0; //Module disabled
    CCP3CONbits.CCP3EN = 0; //Module disabled
    CCP4CONbits.CCP4EN = 0; //Module disabled
    PPSUnLock()
    OUTPUT1_PPS = OUTPUT_LATCH;
    OUTPUT2_PPS = OUTPUT_LATCH;
    OUTPUT3_PPS = OUTPUT_LATCH;
    OUTPUT4_PPS = OUTPUT_LATCH;
    PPSLock()
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
    return 0;
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
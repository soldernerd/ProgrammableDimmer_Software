
#include <xc.h>
#include <stdint.h>
#include "encoder.h"
#include "config.h"

#define COUNT_MIN -128
#define COUNT_MAX 127

//External variables
extern int8_t encoder1_count;
extern uint8_t encoder1_button;
extern int8_t encoder2_count;
extern uint8_t encoder2_button;


void encoder_init(void)
{
    //All encoder signals as digital inputs
    ENCODER1_A_TRIS = 1;
    ENCODER1_A_ANSEL = 0;
    ENCODER1_B_TRIS = 1;
    ENCODER1_B_ANSEL = 0;
    ENCODER1_PB_TRIS = 1;
    ENCODER1_PB_ANSEL = 0;
    ENCODER2_A_TRIS = 1;
    ENCODER2_A_ANSEL = 0;
    ENCODER2_B_TRIS = 1;
    ENCODER2_B_ANSEL = 0;
    ENCODER2_PB_TRIS = 1;
    ENCODER2_PB_ANSEL = 0;
    
    //Clear all interrupt flags
    ENCODER1_A_FLAG = 0;
    ENCODER1_B_FLAG = 0;
    ENCODER1_PB_FLAG = 0;
    ENCODER2_A_FLAG = 0;
    ENCODER2_B_FLAG = 0;
    ENCODER2_PB_FLAG = 0;
    
    //Enable interrupts on rising edges
    ENCODER1_A_RISING = 1;
    ENCODER1_B_RISING = 1;
    ENCODER1_PB_RISING = 1;
    ENCODER2_A_RISING = 1;
    ENCODER2_B_RISING = 1;
    ENCODER2_PB_RISING = 1;
    
    //Initialize variables
    encoder1_count = 0;
    encoder1_button = 0;
    encoder2_count = 0;
    encoder2_button = 0;
    
    //Enable interrupt-on-change interrupts
    PIE0bits.IOCIE = 1;
    
    //Enable peripheral interrupts
    INTCONbits.PEIE = 1;
    
    //Enable global interrupts
    INTCONbits.GIE = 1;
}

void encoder_isr(void)
{
    //Encoder 1
    if(ENCODER1_A_FLAG)
    {
        if(!ENCODER1_B_PIN)
        {
            #ifdef ENCODER_PEC09_TOP
                if(encoder1_count>COUNT_MIN)
                    --encoder1_count;
            #endif
            #ifdef ENCODER_PEC15_BOTTOM
                if(encoder1_count<COUNT_MAX)
                    ++encoder1_count;
            #endif  
        }
        ENCODER1_A_FLAG = 0;
    }   
    if(ENCODER1_B_FLAG)
    {
        if(!ENCODER1_A_PIN)
        {
            #ifdef ENCODER_PEC09_TOP
                if(encoder1_count<COUNT_MAX)
                    ++encoder1_count;
            #endif
            #ifdef ENCODER_PEC15_BOTTOM
                if(encoder1_count>COUNT_MIN)
                    --encoder1_count;
            #endif
        }
        ENCODER1_B_FLAG = 0;
    }
    if(ENCODER1_PB_FLAG)
    {
        encoder1_button = 1;
        ENCODER1_PB_FLAG = 0;
    }
    //Encoder 2
    if(ENCODER2_A_FLAG)
    {
        if(!ENCODER2_B_PIN)
        {
            #ifdef ENCODER_PEC09_TOP
                if(encoder2_count>COUNT_MIN)
                    --encoder2_count;
            #endif
            #ifdef ENCODER_PEC15_BOTTOM
                if(encoder2_count<COUNT_MAX)
                    ++encoder2_count;
            #endif
        }
        ENCODER2_A_FLAG = 0;
    }   
    if(ENCODER2_B_FLAG)
    {
        if(!ENCODER2_A_PIN)
        {
            #ifdef ENCODER_PEC09_TOP
                if(encoder2_count<COUNT_MAX)
                    ++encoder2_count;
            #endif
            #ifdef ENCODER_PEC15_BOTTOM
                if(encoder2_count>COUNT_MIN)
                    --encoder2_count;
            #endif
        }
        ENCODER2_B_FLAG = 0;
    } 
    if(ENCODER2_PB_FLAG)
    {
        encoder2_button = 1;
        ENCODER2_PB_FLAG = 0;
    }
}
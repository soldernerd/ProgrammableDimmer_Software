
#include <xc.h>
#include <stdint.h>
#include "function_dualwhite.h"
#include "config.h"
#include "pwm.h"

//External variables
extern int8_t encoder1_count;
extern int8_t encoder2_count;

void main_loop_rgb(void)
{
    uint16_t brightness_levels[16] = {0,5,41,90,138,165,198,238,286,343,411,493,592,710,853,1023};
    uint8_t brightness1 = 0;
    uint8_t brightness2 = 0;
    uint8_t brightness3 = 0;
    uint8_t brightness4 = 0;
    int16_t count = 0;
    
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
                turn_off();
            }
        }
        else
        {
            count = 0;
        }
    }
}
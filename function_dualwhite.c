
#include <xc.h>
#include <stdint.h>
#include "function_dualwhite.h"
#include "config.h"
#include "pwm.h"

//External variables
extern int8_t encoder1_count;
extern uint8_t encoder1_button;
extern int8_t encoder2_count;
extern uint8_t encoder2_button;

void main_loop_dualwhite(void)
{
    //Initialize brightness lookup arrays
    uint16_t brightness_levels1[BRIGHTNESS_LEVEL_COUNT_1] = BRIGHTNESS_LEVELS_1;
    uint16_t brightness_levels2[BRIGHTNESS_LEVEL_COUNT_2] = BRIGHTNESS_LEVELS_2;
    //Restore brightness 1 from eeprom
    uint8_t brightness1 = eeprom_read(EEPROM_BRIGHTNESS_1_ADDRESS);
    if(brightness1<BRIGHTNESS_LEVEL_MIN_1 || brightness1>BRIGHTNESS_LEVEL_MAX_1)
    {
        brightness1 = BRIGHTNESS_LEVEL_MIN_1;
        eeprom_write(EEPROM_BRIGHTNESS_1_ADDRESS, brightness1);
    }
    uint8_t brightness1_saved = brightness1;
    //Restore brightness 2 from eeprom
    uint8_t brightness2 = eeprom_read(EEPROM_BRIGHTNESS_2_ADDRESS);
    if(brightness2<BRIGHTNESS_LEVEL_MIN_2 || brightness2>BRIGHTNESS_LEVEL_MAX_2)
    {
        brightness2 = BRIGHTNESS_LEVEL_MIN_2;
        eeprom_write(EEPROM_BRIGHTNESS_2_ADDRESS, brightness2);
    }
    uint8_t brightness2_saved = brightness2;
    int16_t off_count = 0;
    
    //Set dutycycle
    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels1[brightness1]);
    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels1[brightness1]);
    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels2[brightness2]);
    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels2[brightness2]);
    
    while(1)
    {
        if(brightness1>=BRIGHTNESS_LEVEL_MIN_1)
        {
            //Output 1, i.e. channels 1 & 2
            if(encoder1_count>0)
            {
                if(brightness1<BRIGHTNESS_LEVEL_MAX_1)
                {
                    ++brightness1;
                    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels1[brightness1]);
                    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels1[brightness1]);
                }
                encoder1_count = 0;
            }   
            if(encoder1_count<0)
            {
                if(brightness1>BRIGHTNESS_LEVEL_MIN_1)
                {
                    --brightness1;
                    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels1[brightness1]);
                    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels1[brightness1]);
                }
                encoder1_count = 0;
            }
        }
        
        if(encoder1_button)
        {
            if(brightness1==0)
            {
                //Output is currently off. Restore saved value and clear count
                brightness1 = brightness1_saved;
                encoder1_count = 0;
            }    
            else
            {
                //Output is currently on. Save value and turn output off
                brightness1_saved = brightness1;
                brightness1 = 0;
            }
            //Clear button pressed flag
            encoder1_button = 0;
            //Set outputs
            pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels1[brightness1]);
            pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels1[brightness1]);
        }

        if(brightness2>=BRIGHTNESS_LEVEL_MIN_2)
        {
            //Output 2, i.e. channels 3 & 4
            if(encoder2_count>0)
            {
                if(brightness2<BRIGHTNESS_LEVEL_MAX_2)
                {
                    ++brightness2;
                    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels2[brightness2]);
                    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels2[brightness2]);
                }
                encoder2_count = 0;
            }   
            if(encoder2_count<0)
            {
                if(brightness2>BRIGHTNESS_LEVEL_MIN_2)
                {
                    --brightness2;
                    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels2[brightness2]);
                    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels2[brightness2]);
                }
                encoder2_count = 0;
            }    
        }
        
        if(encoder2_button)
        {
            if(brightness2==0)
            {
                //Output is currently off. Restore saved value and clear count
                brightness2 = brightness2_saved;
                encoder2_count = 0;
            }    
            else
            {
                //Output is currently on. Save value and turn output off
                brightness2_saved = brightness2;
                brightness2 = 0;
            }
            //Clear button pressed flag
            encoder2_button = 0;
            //Set outputs
            pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels2[brightness2]);
            pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels2[brightness2]);
        }

        //Turn off power supply after 5 seconds of inactivity
        if(brightness1==0 && brightness2==0)
        {
            ++off_count;
            if(off_count==TURN_OFF_DELAY)
            {
                //Save last brightness to eeprom
                eeprom_write(EEPROM_BRIGHTNESS_1_ADDRESS, brightness1_saved);
                eeprom_write(EEPROM_BRIGHTNESS_2_ADDRESS, brightness2_saved);
                //Disable power supply
                turn_off();
            }
        }
        else
        {
            off_count = 0;
        }
        
        __delay_ms(1);
    }
}

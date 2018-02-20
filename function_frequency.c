
#include "config.h"

#ifdef FUNCTION_FREQUENCY

#include <xc.h>
#include <stdint.h>
#include "function_frequency.h"
#include "pwm.h"

//External variables
extern int8_t encoder1_count;
extern uint8_t encoder1_button;
extern int8_t encoder2_count;
extern uint8_t encoder2_button;

void main_loop_frequency(void)
{
    //Initialize brightness lookup array
    uint16_t brightness_levels[BRIGHTNESS_LEVEL_COUNT] = BRIGHTNESS_LEVELS;
    
    //Restore brightness 1 from eeprom
    uint8_t brightness = eeprom_read(EEPROM_BRIGHTNESS_ADDRESS);
    if(brightness<BRIGHTNESS_LEVEL_MIN || brightness>BRIGHTNESS_LEVEL_MAX)
    {
        brightness = BRIGHTNESS_LEVEL_MIN;
        eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness);
    }
    uint8_t brightness_saved = brightness;
    int16_t off_count = 0;
    
    //Set dutycycle
    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]);
    
    while(1)
    {
        if(brightness>=BRIGHTNESS_LEVEL_MIN)
        {
            //Output 1, i.e. channels 1 & 2
            if(encoder1_count>0)
            {
                if(brightness<BRIGHTNESS_LEVEL_MAX)
                {
                    ++brightness;
                    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]);
                }
                encoder1_count = 0;
            }   
            if(encoder1_count<0)
            {
                if(brightness>BRIGHTNESS_LEVEL_MIN)
                {
                    --brightness;
                    pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
                    pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]);
                }
                encoder1_count = 0;
            }
        }
        
        if(encoder1_button)
        {
            if(brightness==0)
            {
                //Output is currently off. Restore saved value and clear count
                brightness = brightness_saved;
                encoder1_count = 0;
            }    
            else
            {
                //Output is currently on. Save value and turn output off
                brightness_saved = brightness;
                brightness = 0;
            }
            //Clear button pressed flag
            encoder1_button = 0;
            //Set outputs
            pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
            pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
            pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
            pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]);
        }

        /*
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
         * */

        //Turn off power supply after some seconds of inactivity
        if(brightness==0)
        {
            ++off_count;
            if(off_count==TURN_OFF_DELAY)
            {
                //Save last brightness to eeprom
                eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness_saved);
                //eeprom_write(EEPROM_BRIGHTNESS_2_ADDRESS, brightness2_saved);
                //Wait until data is definitely written
                __delay_ms(10);
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

#endif	/* FUNCTION_FREQUENCY */

#include "config.h"

#ifdef FUNCTION_WARMCOOL

#include <xc.h>
#include <stdint.h>
#include "function_warmcool.h"
#include "pwm.h"

//External variables
extern int8_t encoder1_count;
extern uint8_t encoder1_button;
extern int8_t encoder2_count;
extern uint8_t encoder2_button;

//Global lookup arrays
uint16_t brightness_levels[BRIGHTNESS_LEVEL_COUNT] = BRIGHTNESS_LEVELS;
uint16_t color_levels[COLOR_LEVEL_COUNT] = COLOR_LEVELS;

static void _set_dutycycle(uint8_t brightness, uint8_t color);

void main_loop_warmcool(void)
{
    //Restore brightness from eeprom
    uint8_t brightness = eeprom_read(EEPROM_BRIGHTNESS_ADDRESS);
    if(brightness<BRIGHTNESS_LEVEL_MIN || brightness>BRIGHTNESS_LEVEL_MAX)
    {
        brightness = BRIGHTNESS_LEVEL_MIN;
        eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness);
    }
    uint8_t brightness_saved = brightness;
    
    //Restore color from eeprom
    uint8_t color = eeprom_read(EEPROM_COLOR_ADDRESS);
    if(color<COLOR_LEVEL_MIN || color>COLOR_LEVEL_MAX)
    {
        color = COLOR_LEVEL_NEUTRAL;
        eeprom_write(EEPROM_COLOR_ADDRESS, color);
    }
    int16_t off_count = 0;
    
    //Set dutycycle
    _set_dutycycle(brightness, color);
    
    while(1)
    {
        //Output is on
        if(brightness>=BRIGHTNESS_LEVEL_MIN)
        {
            //Brightness
            if(encoder1_count>0)
            {
                if(brightness<BRIGHTNESS_LEVEL_MAX)
                {
                    ++brightness;
                    _set_dutycycle(brightness, color);
                }
                encoder1_count = 0;
            }   
            if(encoder1_count<0)
            {
                if(brightness>BRIGHTNESS_LEVEL_MIN)
                {
                    --brightness;
                    _set_dutycycle(brightness, color);
                }
                encoder1_count = 0;
            }
            
            //Color
            if(encoder2_count>0)
            {
                if(color<COLOR_LEVEL_MAX)
                {
                    ++color;
                    _set_dutycycle(brightness, color);
                }
                encoder2_count = 0;
            }   
            if(encoder2_count<0)
            {
                if(color>COLOR_LEVEL_MIN)
                {
                    --color;
                    _set_dutycycle(brightness, color);
                }
                encoder2_count = 0;
            }   
            
            //Turn off
            if(encoder1_button)
            {
                //Output is currently on. Save value and turn output off
                brightness_saved = brightness;
                brightness = 0;
                _set_dutycycle(brightness, color);
                //Clear button pressed flag
                encoder1_button = 0;
            }
            
            //Neutralize color
            if(encoder2_button)
            {
                color = COLOR_LEVEL_NEUTRAL;
                _set_dutycycle(brightness, color);
                //Clear button pressed flag
                encoder2_button = 0;
            }
        }
        //Output is off, react only to pressing the on button
        else
        {
            //Turn on
            if(encoder1_button)
            {
                //Output is currently off. Restore saved value and clear count
                brightness = brightness_saved;
                _set_dutycycle(brightness, color);
                //Clear button pressed flag
                encoder1_button = 0;
            }
        }
        


        //Turn off power supply after some time of inactivity
        if(brightness==0)
        {
            ++off_count;
            if(off_count==TURN_OFF_DELAY)
            {
                //Save last brightness to eeprom
                eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness_saved);
                eeprom_write(EEPROM_COLOR_ADDRESS, color);
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

static void _set_dutycycle(uint8_t brightness, uint8_t color)
{
    uint32_t tmp;
    uint16_t warm;
    uint16_t cool;
    
    //Calculate warm
    tmp = brightness_levels[brightness];
    tmp *= color_levels[color];
    warm = (uint16_t)(tmp>>8);
    
    //Calculate cool
    tmp = brightness_levels[brightness];
    tmp *= (256 - color_levels[color]);
    cool = (uint16_t)(tmp>>8);
    
    //Set outputs
    pwm_set_dutycycle(PWM_CHANNEL_1, cool);
    pwm_set_dutycycle(PWM_CHANNEL_2, cool);
    pwm_set_dutycycle(PWM_CHANNEL_3, warm);
    pwm_set_dutycycle(PWM_CHANNEL_4, warm);
}

#endif	/* FUNCTION_WARMCOOL */
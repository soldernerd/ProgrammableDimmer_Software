#include "config.h"

#ifdef FUNCTION_RGB

#include <xc.h>
#include <stdint.h>
#include "function_rgb.h"

#include "pwm.h"

//External variables
extern int8_t encoder1_count;
extern uint8_t encoder1_button;
extern int8_t encoder2_count;
extern uint8_t encoder2_button;

//Local variables
uint8_t white;
uint8_t off;
uint8_t brightness;
uint8_t color;
uint8_t update;
uint16_t red;
uint16_t green;
uint16_t blue;

//Initialize brightness lookup arrays
uint16_t brightness_levels[BRIGHTNESS_LEVEL_COUNT] = BRIGHTNESS_LEVELS;

//Initialize 2-dimensional color array
uint8_t colors[NUMBER_OF_COLORS][3] =
{
  {255, 0, 0},
  {223, 32, 0},
  {191, 64, 0},
  {159, 96, 0},
  {128, 128, 0},
  {96, 159, 0},
  {64, 191, 0},
  {32, 223, 0},
  {0, 255, 0},
  {0, 223, 32},
  {0, 191, 64},
  {0, 159, 96},
  {0, 128, 128},
  {0, 96, 159},
  {0, 64, 191},
  {0, 32, 223},
  {0, 0, 255},
  {32, 0, 223},
  {64, 0, 191},
  {96, 0, 159},
  {128, 0, 128},
  {159, 0, 96},
  {191, 0, 64},
  {223, 0, 32}
};

static void update_outputs();

void main_loop_rgb(void)
{
    off = 0;
    white = eeprom_read(EEPROM_WHITE_ADDRESS);
    if(white != (white&0x01))
    {
        white = 1;
        eeprom_write(EEPROM_WHITE_ADDRESS, white);
    }
    brightness = eeprom_read(EEPROM_BRIGHTNESS_ADDRESS);
    if(brightness<BRIGHTNESS_LEVEL_MIN || brightness>BRIGHTNESS_LEVEL_MAX)
    {
        brightness = BRIGHTNESS_LEVEL_MIN;
        eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness);
    }
    color = eeprom_read(EEPROM_COLOR_ADDRESS);
    if(color>NUMBER_OF_COLORS)
    {
        color = 0;
        eeprom_write(EEPROM_COLOR_ADDRESS, color);
    }
    update = 1;

    //Restore brightness 1 from eeprom
    /*
    uint8_t brightness1 = eeprom_read(EEPROM_BRIGHTNESS_1_ADDRESS);
    if(brightness1<BRIGHTNESS_LEVEL_MIN_1 || brightness1>BRIGHTNESS_LEVEL_MAX_1)
    {
        brightness1 = BRIGHTNESS_LEVEL_MIN_1;
        eeprom_write(EEPROM_BRIGHTNESS_1_ADDRESS, brightness1);
    }
    uint8_t brightness1_saved = brightness1;
     * */

    int16_t off_count = 0;
    
    //Set dutycycle
    pwm_set_dutycycle(PWM_CHANNEL_1, 0);
    pwm_set_dutycycle(PWM_CHANNEL_2, 0);
    pwm_set_dutycycle(PWM_CHANNEL_3, 0);
    pwm_set_dutycycle(PWM_CHANNEL_4, 0);
    
    while(1)
    {
        //Encoder 1 turned clockwise
        if(encoder1_count>0)
        {
            if(brightness<BRIGHTNESS_LEVEL_MAX)
            {
                //Don't change brightness when off flag is set
                if(!off)
                {
                    ++brightness;
                    update = 1;
                }
            }
            encoder1_count = 0;
        }
        
        //Encoder 1 turned counterclockwise
        if(encoder1_count<0)
        {
            if(brightness>BRIGHTNESS_LEVEL_MIN)
            {
                //Don't change brightness when off flag is set
                if(!off)
                {
                    --brightness;
                    update = 1;
                }
            }
            encoder1_count = 0;
        }
        
        //Encoder 1 Pushbutton
        if(encoder1_button)
        {
            //Toggle off flag
            off ^= 0x01;
            update = 1;
            encoder1_button = 0;
        }
        
        //Encoder 2 turned clockwise
        if(encoder2_count>0)
        {
            //Don't change color when white flag or off flag is set
            if(!white && !off)
            {
                ++color;
                if(color==NUMBER_OF_COLORS)
                    color = 0;
                update = 1;
            }
            encoder2_count = 0;
        }
        
        //Encoder 2 turned counterclockwise
        if(encoder2_count<0)
        {
            //Don't change color when white flag or off flag is set
            if(!white && !off)
            {
                --color;
                if(color>=NUMBER_OF_COLORS)
                    color = NUMBER_OF_COLORS-1;
                update = 1;
            }
            encoder2_count = 0;
        }
        
        //Encoder 2 Pushbutton
        if(encoder2_button)
        {
            //Don't toggle between color and white when off flag is set
            if(!off)
            {
                //Toggle white flag
                white ^= 0x01;
                update = 1;
            }
            encoder2_button = 0;
        }
        
        //Update if and only if necessary
        if(update)
        {
            update_outputs();
            //Clear update flag
            update = 0;
        }

        //Turn off power supply after 5 seconds of inactivity
        if(off)
        {
            ++off_count;
            if(off_count==TURN_OFF_DELAY)
            {
                //Save parameters
                eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness);
                eeprom_write(EEPROM_COLOR_ADDRESS, color);
                eeprom_write(EEPROM_WHITE_ADDRESS, white);
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

static void update_outputs()
{
    uint32_t tmp;
    
    if(off)
    {
        red = 0;
        green = 0;
        blue = 0;
    }
    else
    {
        if(white)
        {
            tmp = ((uint32_t) brightness_levels[brightness] * RGB_NEUTRAL_RED) >> 8;
            red = (uint16_t) tmp;
            tmp = ((uint32_t) brightness_levels[brightness] * RGB_NEUTRAL_GREEN) >> 8;
            green = (uint16_t) tmp;
            tmp = ((uint32_t) brightness_levels[brightness] * RGB_NEUTRAL_BLUE) >> 8;
            blue = (uint16_t) tmp;
        }
        else
        {
            tmp = ((uint32_t) brightness_levels[brightness] * colors[color][0]) >> 8;
            red = (uint16_t) tmp;
            tmp = ((uint32_t) brightness_levels[brightness] * colors[color][1]) >> 8;
            green = (uint16_t) tmp;
            tmp = ((uint32_t) brightness_levels[brightness] * colors[color][2]) >> 8;
            blue = (uint16_t) tmp;
        }
    }
    
    //Set PWM outputs
    pwm_set_dutycycle(OUTPUT_RED, red);
    pwm_set_dutycycle(OUTPUT_GREEN, green);
    pwm_set_dutycycle(OUTPUT_BLUE, blue);
}

#endif /* FUNCTION_RGB */
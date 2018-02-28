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
extern uint8_t portA;
extern uint8_t portC;

//Local variables
operating_mode_t op_mode;
uint8_t timeslot;
uint8_t timer_high_byte;
uint8_t timer_low_byte;
uint8_t outputs12_turnoff_time;
uint8_t outputs34_turnoff_time;

static void _setup_timer_1(void);
static void _start_timer_1(void);
static void _stop_timer_1(void);

static void _setup_timer_1(void)
{
    //Configure timer 1
    //Resulting input frequency is 8 MHz
    T1CONbits.TMR1CS = 0b00; //Clock source = Fosc /4
    T1CONbits.T1CKPS = 0b00; //Prescaler = 1
}

static void _start_timer_1(void)
{
    timeslot = 0;
    PIR1bits.TMR1IF = 0; //Clear interrupt flag
    TMR1H = timer_high_byte;
    TMR1L = timer_low_byte;
    //Configure timer 1 interrupts
    INTCONbits.GIE = 1; //General interrupt enable
    INTCONbits.PEIE = 1; //Peripheral interrupt enable
    PIE1bits.TMR1IE = 1; //Enable timer 1 interrupts
    T1CONbits.TMR1ON = 1; //Turn timer 1 on
}

static void _stop_timer_1(void)
{
    PIE1bits.TMR1IE = 0; //Disable timer 1 interrupts
    T1CONbits.TMR1ON = 0; //Turn timer 1 off
    OUTPUT1_PIN = 0;
    OUTPUT2_PIN = 0;
    OUTPUT3_PIN = 0;
    OUTPUT4_PIN = 0;
}

void timer1_isr(void)
{
    //Load timer values
    TMR1H = timer_high_byte;
    TMR1L = timer_low_byte;
    
    //Increment time slot
    ++timeslot;

    //Do what's needed
    if(timeslot==0)
    {
        OUTPUT1_PIN = 1;
        OUTPUT2_PIN = 1;
    }
    else if(timeslot==outputs12_turnoff_time)
    {
        OUTPUT1_PIN = 0;
        OUTPUT2_PIN = 0;
    }
   
    if(timeslot==128)
    {
        OUTPUT3_PIN = 1;
        OUTPUT4_PIN = 1;
    }
    else if(timeslot==outputs34_turnoff_time)
    {
        OUTPUT3_PIN = 0;
        OUTPUT4_PIN = 0;
    }
     
    //Clear interrupt flag
    PIR1bits.TMR1IF = 0;
}

void main_loop_frequency(void)
{
    //Specific setup
    pwm_stop();
    _setup_timer_1();
    
    //Initialize brightness lookup array
    uint16_t brightness_levels[BRIGHTNESS_LEVEL_COUNT] = BRIGHTNESS_LEVELS;
    //Initialize frequency lookup array
    uint16_t frequency_levels[FREQUENCY_LEVEL_COUNT] = FREQUENCY_LEVELS;
    
    //Restore operating mode from eeprom
    uint8_t op_mode = eeprom_read(EEPROM_MODE_ADDRESS);
    if(!((op_mode==OPERATING_MODE_REGULAR) || (op_mode==OPERATING_MODE_LOW_FREQUENCY)))
    {
        op_mode = OPERATING_MODE_REGULAR;
        eeprom_write(EEPROM_MODE_ADDRESS, op_mode);
        __delay_ms(10);
    }
    
    //Restore brightness from eeprom
    uint8_t brightness = eeprom_read(EEPROM_BRIGHTNESS_ADDRESS);
    if(brightness<BRIGHTNESS_LEVEL_MIN || brightness>BRIGHTNESS_LEVEL_MAX)
    {
        brightness = BRIGHTNESS_LEVEL_MIN;
        eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness);
        __delay_ms(10);
    }
    uint8_t brightness_saved = brightness;
    
    //Restore frequency from eeprom
    uint8_t frequency = eeprom_read(EEPROM_FREQUENCY_ADDRESS);
    if(frequency<FREQUENCY_LEVEL_MIN || frequency>FREQUENCY_LEVEL_MAX)
    {
        frequency = FREQUENCY_LEVEL_MIN;
        eeprom_write(EEPROM_FREQUENCY_ADDRESS, frequency);
        __delay_ms(10);
    }
    timer_high_byte = HIGH_BYTE(frequency_levels[frequency]);
    timer_low_byte = LOW_BYTE(frequency_levels[frequency]);
    
    //Reset off count
    int16_t off_count = 0;
    

    
    //Start according to op_mode
    //op_mode=OPERATING_MODE_LOW_FREQUENCY;
    if(op_mode==OPERATING_MODE_REGULAR)
    {
    //Set dutycycle
        pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
        pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
        pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
        pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]);
        //Start the thing
        pwm_start();
    }
    else
    {
        outputs12_turnoff_time = 0 + (brightness_levels[brightness]>>2);
        outputs34_turnoff_time = 128 + (brightness_levels[brightness]>>2);
        _start_timer_1();
    }
    
    
    while(1)
    {
        if(brightness>=BRIGHTNESS_LEVEL_MIN)
        {
            if(op_mode==OPERATING_MODE_REGULAR)
            {
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
            else //op_mode==OPERATING_MODE_LOW_FREQUENCY
            {
                if(encoder1_count>0)
                {
                    if(brightness<BRIGHTNESS_LEVEL_MAX)
                    {
                        ++brightness;
                        outputs12_turnoff_time = 0 + (brightness_levels[brightness]>>2);
                        outputs34_turnoff_time = 128 + (brightness_levels[brightness]>>2);
                    }
                    encoder1_count = 0;
                }   
                if(encoder1_count<0)
                {
                    if(brightness>BRIGHTNESS_LEVEL_MIN)
                    {
                        --brightness;
                        outputs12_turnoff_time = 0 + (brightness_levels[brightness]>>2);
                        outputs34_turnoff_time = 128 + (brightness_levels[brightness]>>2);
                    }
                    encoder1_count = 0;
                }
            }

        }
        
        if(encoder1_button)
        {
            if(op_mode==OPERATING_MODE_REGULAR)
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
            else //op_mode==OPERATING_MODE_LOW_FREQUENCY
            {
                if(brightness==0)
                {
                    //Output is currently off. Restore saved value and clear count
                    brightness = brightness_saved;
                    outputs12_turnoff_time = 0 + (brightness_levels[brightness]>>2);
                    outputs34_turnoff_time = 128 + (brightness_levels[brightness]>>2);
                    _start_timer_1();
                    encoder1_button = 0;
                }    
                else
                {
                    //Output is currently on. Save value and turn output off
                    brightness_saved = brightness;
                    brightness = 0;
                    _stop_timer_1();
                    encoder1_button = 0;
                }
            }
        }
        
        if((op_mode==OPERATING_MODE_LOW_FREQUENCY) && (brightness>0))
        {
            //Output 2, i.e. channels 3 & 4
            if(encoder2_count>0)
            {
                if(frequency<FREQUENCY_LEVEL_MAX)
                {
                    ++frequency;
                    timer_high_byte = HIGH_BYTE(frequency_levels[frequency]);
                    timer_low_byte = LOW_BYTE(frequency_levels[frequency]);
                }
                encoder2_count = 0;
            }   
            if(encoder2_count<0)
            {
                if(frequency>FREQUENCY_LEVEL_MIN)
                {
                    --frequency;
                    timer_high_byte = HIGH_BYTE(frequency_levels[frequency]);
                    timer_low_byte = LOW_BYTE(frequency_levels[frequency]);
                }
                encoder2_count = 0;
            }    
        }
        
        if(encoder2_button)
        {
            if(op_mode==OPERATING_MODE_REGULAR)
            {
                //We are currently in regular mode, switch to low frequency mode
                op_mode = OPERATING_MODE_LOW_FREQUENCY;
                pwm_stop();
                outputs12_turnoff_time = 0 + (brightness_levels[brightness]>>2);
                outputs34_turnoff_time = 128 + (brightness_levels[brightness]>>2);
                _start_timer_1();
                encoder2_button = 0;
            }    
            else
            {
                //We are currently in low frequency mode, switch to regular mode
                op_mode = OPERATING_MODE_REGULAR;
                _stop_timer_1();
                pwm_set_dutycycle(PWM_CHANNEL_1, brightness_levels[brightness]);
                pwm_set_dutycycle(PWM_CHANNEL_2, brightness_levels[brightness]);
                pwm_set_dutycycle(PWM_CHANNEL_3, brightness_levels[brightness]);
                pwm_set_dutycycle(PWM_CHANNEL_4, brightness_levels[brightness]); 
                pwm_start();
                encoder2_button = 0;
            }
        }

        //Turn off power supply after some seconds of inactivity
        if(brightness==0)
        {
            ++off_count;
            if(off_count==TURN_OFF_DELAY)
            {
                //Save last state to eeprom
                eeprom_write(EEPROM_BRIGHTNESS_ADDRESS, brightness_saved);
                __delay_ms(10);
                eeprom_write(EEPROM_FREQUENCY_ADDRESS, frequency);
                __delay_ms(10);
                eeprom_write(EEPROM_MODE_ADDRESS, op_mode);
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

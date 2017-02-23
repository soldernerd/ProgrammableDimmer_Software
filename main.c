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

// PIC16F18325 Configuration Bit Settings
// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC with 2x PLL (32MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)
// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = SBOREN   // Brown-out Reset Enable bits (Brown-out Reset enabled according to SBOREN)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = OFF    // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be set and cleared repeatedly (subject to the unlock sequence))
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)
// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)
// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

//CPU Frequency
#define _XTAL_FREQ 32000000

//Enable N-FET on pin 13, RA0
#define ENABLE_PORT PORTA
#define ENABLE_TRIS TRISA
#define ENABLE_MASK 0b00000001

/*
//LED on pin 9, RC1
#define LED_PORT PORTC
#define LED_TRIS TRISC
#define LED_MASK 0b00000010

//PWM on pin 2, RA5
#define PWM_PORT PORTA
#define PWM_TRIS TRISA
#define PWM_MASK 0b00100000

//Tach on pin3, RA4
#define TACH_PORT PORTA
#define TACH_TRIS TRISA
#define TACH_ANSEL ANSELA
#define TACH_MASK 0b0010000

//Temperature sensor on pin 8, RC2
#define TEMPERATURE_PORT PORTC
#define TEMPERATURE_TRIS TRISC
#define TEMPERATURE_ANSEL ANSELC
#define TEMPERATURE_MASK 0b00000100

//Temperature debug output on pin 5, RC5
#define DEBUGA_PORT PORTC
#define DEBUGA_TRIS TRISC
#define DEBUGA_MASK 0b00100000

//Target speed debug output on pin 6, RC4
#define DEBUGB_PORT PORTC
#define DEBUGB_TRIS TRISC
#define DEBUGB_MASK 0b00010000

//Actual speed debug output on pin 7, RC3
#define DEBUGC_PORT PORTC
#define DEBUGC_TRIS TRISC
#define DEBUGC_MASK 0b00001000

//ADC averaging
#define ADC_AVERAGE_SHIFT 5
#define ADC_AVERAGE_COUNT ((1<<ADC_AVERAGE_SHIFT) - 1)
#define ADC_AVERAGE_ADD (ADC_AVERAGE_COUNT/2)

//Duty cycle calculation
#define STARTUP_DUTYCYCLE 160
#define DUTY_CYCLE_MIN 40
#define DUTY_CYCLE_MAX 320
#define FAN_SPEED_MIN 150 
#define FAN_SPEED_MAX 900
#define FAN_SPEED_SLOPE 3
#define FAN_TEMPERATURE_THRESHOLD 300

//Temperature measurement
uint8_t adc_count;
uint32_t adc_sum;
uint16_t adc_voltage; //in millivolts
uint16_t temperature; //in 0.1 degrees centigrade
uint8_t ready;

//Pulse width modulation (PWM)
uint16_t duty_cycle;

//LED indicator
uint16_t led_count;

//Fan speed measurement
uint16_t tach_capture_0;
uint16_t tach_capture_1;
uint16_t tach_measurement;
uint16_t actual_fan_speed; //in 10 revolutions per minute, e.g. 450=4500rpm

//Target fan speed
uint16_t target_fan_speed; //in 10 revolutions per minute, e.g. 450=4500rpm 
 * 
 * */

void interrupt ISR(void)
{ 
    uint16_t adc_value;
    /*
    
    if(PIR1) //Timer 6 overflow interrupt
    {
      //LED on time is proportional to target_fan_speed / FAN_SPEED_MAX
      if(led_count==0)
      {
         LED_PORT |= LED_MASK;
      }
      if(led_count==(target_fan_speed>>2))
      {
         LED_PORT &= ~LED_MASK;  
      }
      ++led_count;
      if(led_count==FAN_SPEED_MAX>>2)
      {
          led_count = 0;
      }

      //Read result of ADC conversion
      adc_value = ADRESH;
      adc_value <<= 8;
      adc_value |= ADRESL;
      //Start a new conversion
      ADCON0 |= 0b00000010;
      //Add current value to sum
      adc_sum += adc_value;
      ++adc_count;
      if(adc_count==ADC_AVERAGE_COUNT)
      {
         adc_sum += ADC_AVERAGE_ADD;
         //10bit ADC with 1.024V reference -> Voltage (in mV) = ADC reading
         adc_voltage = (uint16_t)(adc_sum >> ADC_AVERAGE_SHIFT);
         //LM35 output is 10mV per degree centigrade -> temperature = adc_voltage
         temperature = adc_voltage;
         adc_sum = 0;
         adc_count = 0;
         ready = 1;
      }
      //Clear Interrupt Flag
      PIR2 &= 0b01111111;
    } 
    
    if(PIR4&0b00000001) //Capture interrupt
    {
      //Save captured timer values
      tach_capture_0 = tach_capture_1;
      tach_capture_1 = CCPR1H;
      tach_capture_1 <<= 8;
      tach_capture_1 |= CCPR1L;
      if(tach_capture_1!=tach_capture_0)
      {
        tach_measurement = tach_capture_1 - tach_capture_0;
      }
      
      //Clear Interrupt Flag
      PIR4 &= 0b11111110;
    } 
    */
}

void init(void)
{
    //Enable as output
    ENABLE_TRIS &= ~ENABLE_MASK;
    ENABLE_PORT |= ENABLE_MASK;
    
    /*
    //Temperature measurement
    adc_count = 0;
    adc_sum = 0;
    adc_voltage = 0;
    temperature = 0;
    ready = 0;

    //Pulse width modulation (PWM)
    duty_cycle = STARTUP_DUTYCYCLE;

    //LED indicator
    led_count = 0;

    //Fan speed measurement
    tach_capture_0 = 0;
    tach_capture_1 = 0;
    actual_fan_speed = 0;

    //Target fan speed
    target_fan_speed = FAN_SPEED_MIN;
    
    //LED and PWM as output
    LED_TRIS &= ~LED_MASK;
    PWM_TRIS &= ~PWM_MASK;
    
    //Debug pins as output
    DEBUGA_TRIS &= ~DEBUGA_MASK;
    DEBUGB_TRIS &= ~DEBUGB_MASK;
    DEBUGC_TRIS &= ~DEBUGC_MASK;
    
    //Temperature as analog input
    TEMPERATURE_TRIS |= TEMPERATURE_MASK; 
    TEMPERATURE_ANSEL |= TEMPERATURE_MASK;
    
    //Tach signal as digital input
    TACH_TRIS |= TACH_MASK;
    TACH_ANSEL &= ~TACH_MASK;
    
    //Set up ADC for temperature measurement
    FVRCON = 0b10000001; //Enable Fixed Voltage Reference, Vref=1.024V
    ADCON0 = 0b01001001; //Enable ADC module, channel ANC2 (RC2), page 242
    ADCON1 = 0b11100011; //Result right justified, Fosc/64, GND and internal Fixed Voltage Reference (FVR) as references, page 243
    __delay_ms(5); //Wait for cannel selection to become effective
    ADCON0 |= 0b00000010; //Start a first conversion
    
    //Configure PWM using PWM5 and Timer 2 (page 193)
    PR2 = 80; //320 steps resolution
    TMR2 = 00;
    T2CON = 0b00000101; //Timer2 on, prescaler=4, postscaler=1
    RA5PPS = 0b00000010; //RA5 -> PWM5
    PWM5CON = 0b10110000; //PWM on, active-low
    PWM5DCH = (uint8_t) (duty_cycle>>2);
    PWM5DCL = (uint8_t) (duty_cycle & 0b11);
    
    //Configure CCP1 with timer0 in capture mode to capture tach signal
    T0CON0 = 0b10010000; //Timer0 on, 16bit mode, postscaler=1
    T0CON1 = 0b01000100; //Fosc/4, prescaler=16 -> Fosc/64 
    CCPTMRS &= 0b11111100; //Select Timer1 for CCP module 0
    CCP1PPS = 0b00000100; //CCP1 input is RA4, i.e. tach signal
    CCP1CAP = 0x00; //CCP1 pin as source
    CCP1CON = 0b10000110; // CCP1 on, capture every 4th rising edge
    
    //Configure debug outputs using CCP2, CCP3, CCP4 and Timer 4
    PR4 = 250; //1000 steps resolution
    TMR4 = 00;
    T4CON = 0b00000100; //Timer2 on, prescaler=1, postscaler=1
    CCPTMRS &= 0b00000011; //Timer4 for  CCP modules 2/3/4
    CCPTMRS |= 0b10101000; //Timer4 for  CCP modules 2/3/4
    RC5PPS = 0b00001101; //RC5 -> CCP2
    CCP2CON = 0b10011111; //CCP2 enabled, PWM mode, left-aligned
    CCPR2H = 0x00; //Most significant bits
    CCPR2L = 0x00; //Least significant bits
    RC4PPS = 0b00001110; //RC5 -> CCP3
    CCP3CON = 0b10011111; //CCP3 enabled, PWM mode, left-aligned
    CCPR3H = 0x30; //Most significant bits
    CCPR3L = 0x00; //Least significant bits
    RC3PPS = 0b00001111; //RC3 -> CCP4
    CCP4CON = 0b10011111; //CCP4 enabled, PWM mode, left-aligned
    CCPR4H = 0x40; //Most significant bits
    CCPR4L = 0x00; //Least significant bits
    
    //Configure Timer6 for ADC and LED control
    T6CON = 0b00010111; //Timer4 on, prescaler=64, postscaler=3 -> 163 interrupts/sec 
    
    //Configure Interrupts
    INTCON = 0b11000000; //General and peripheral interrupts on
    PIE0 = 0x00;
    PIE2 = 0x00;
    PIE2 = 0b10000000; //TMR6 to PR6 Match Interrupt
    PIE3 = 0x00;
    PIE4 = 0b00000001; //CCP1 capture interrupt
    //Clear Interrupt flags
    PIR0 = 0;
    PIR1 = 0;
    PIR3 = 0;
    PIR4 = 0; 
     * */
}

void main(void) 
{
    
    int16_t count = 0;
    init(); 
    
    while(1)
    {
        __delay_ms(100);
        ++count;
        if(count==50)
        {
            ENABLE_PORT &= ~ENABLE_MASK;
            count = 0;
        }
        /*
        if(ready)
        {
            //Calculate target fan speed
            target_fan_speed = FAN_SPEED_MIN;
            if(temperature>FAN_TEMPERATURE_THRESHOLD)
            {
                target_fan_speed += (temperature-FAN_TEMPERATURE_THRESHOLD) * FAN_SPEED_SLOPE;
            }
            if(target_fan_speed>FAN_SPEED_MAX)
            {
                target_fan_speed = FAN_SPEED_MAX; 
            }
            
            //Calculate actual fan speed
            actual_fan_speed = 6000000 / tach_measurement;
            
            //Calculate new duty cycle
            old_duty_cycle = duty_cycle;
            error = actual_fan_speed - target_fan_speed;
            correction = error>>4;
            if(error<0) //fan too slow
                duty_cycle -= correction;
            else //fan too fast
                --duty_cycle;
            
            //Some sanity checks on duty cycle
            if(duty_cycle<DUTY_CYCLE_MIN)
                duty_cycle = DUTY_CYCLE_MIN;
            if(duty_cycle>DUTY_CYCLE_MAX)
                duty_cycle = DUTY_CYCLE_MAX;
            
            //Set new PWM duty cycle
            PWM5DCH = (uint8_t) (duty_cycle>>2);
            PWM5DCL = (uint8_t) (duty_cycle & 0b11);
            
            //Output temperature debug signal
            CCPR2H = (uint8_t) (temperature>>2);
            CCPR2L = (uint8_t) (temperature & 0b11);
            
            //Output target speed debug signal
            CCPR3H = (uint8_t) (target_fan_speed>>2);
            CCPR3L = (uint8_t) (target_fan_speed & 0b11);
            
            //Output actual speed debug signal
            CCPR4H = (uint8_t) (actual_fan_speed>>2);
            CCPR4L = (uint8_t) (actual_fan_speed & 0b11);

            ready = 0;
        }
        */
    }
}

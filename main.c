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
#include <pic16f18325.h>

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
#define ENABLE_TRIS TRISAbits.TRISA0
#define ENABLE_PIN PORTAbits.RA0
#define ENABLE_PORT PORTA
#define ENABLE_VARIABLE portA
#define ENABLE_MASK 0b00000001

//Output Channel 1 on pin 12, RA1
#define OUTPUT1_TRIS TRISAbits.TRISA1
#define OUTPUT1_PIN PORTAbits.RA1
#define OUTPUT1_PORT PORTA
#define OUTPUT1_VARIABLE portA
#define OUTPUT1_MASK 0b00000010

//Output Channel 2 on pin 11, RA2
#define OUTPUT2_TRIS TRISAbits.TRISA2
#define OUTPUT2_PIN PORTAbits.RA2
#define OUTPUT2_PORT PORTA
#define OUTPUT2_VARIABLE portA
#define OUTPUT2_MASK 0b00000100

//Output Channel 3 on pin 10, RC0
#define OUTPUT3_TRIS TRISCbits.TRISC0
#define OUTPUT3_PIN PORTCbits.RC0
#define OUTPUT3_PORT PORTC
#define OUTPUT3_VARIABLE portC
#define OUTPUT3_MASK 0b00000001

//Output Channel 4 on pin 9, RC1
#define OUTPUT4_TRIS TRISCbits.TRISC1
#define OUTPUT4_PIN PORTCbits.RC1
#define OUTPUT4_PORT PORTC
#define OUTPUT4_VARIABLE portC
#define OUTPUT4_MASK 0b00000001

//Encoder 1 input A on RA4 / pin 3
#define ENCODER1_A_TRIS TRISAbits.TRISA4
#define ENCODER1_A_ANSEL ANSELAbits.ANSA4
#define ENCODER1_A_PIN PORTAbits.RA4
#define ENCODER1_A_RISING IOCAPbits.IOCAP4
#define ENCODER1_A_FALLING IOCANbits.IOCAN4
#define ENCODER1_A_FLAG IOCAFbits.IOCAF4

//Encoder 1 input B on RC4 / pin 6
#define ENCODER1_B_TRIS TRISCbits.TRISC4
#define ENCODER1_B_ANSEL ANSELCbits.ANSC4
#define ENCODER1_B_PIN PORTCbits.RC4
#define ENCODER1_B_RISING IOCCPbits.IOCCP4
#define ENCODER1_B_FALLING IOCCNbits.IOCCN4
#define ENCODER1_B_FLAG IOCCFbits.IOCCF4

//Encoder 1 input PB on RC5 / pin 5
#define ENCODER1_PB_TRIS TRISCbits.TRISC5
#define ENCODER1_PB_ANSEL ANSELCbits.ANSC5
#define ENCODER1_PB_PIN PORTCbits.RC5
#define ENCODER1_PB_RISING IOCCPbits.IOCCP5
#define ENCODER1_PB_FALLING IOCCNbits.IOCCN5
#define ENCODER1_PB_FLAG IOCCFbits.IOCCF5

//Encoder 2 input A on RC3 / pin 7
#define ENCODER2_A_TRIS TRISCbits.TRISC3
#define ENCODER2_A_ANSEL ANSELCbits.ANSC3
#define ENCODER2_A_PIN PORTCbits.RC3
#define ENCODER2_A_RISING IOCCPbits.IOCCP3
#define ENCODER2_A_FALLING IOCCNbits.IOCCN3
#define ENCODER2_A_FLAG IOCCFbits.IOCCF3

//Encoder 2 input B on RA5 / pin 2
#define ENCODER2_B_TRIS TRISAbits.TRISA5
#define ENCODER2_B_ANSEL ANSELAbits.ANSA5
#define ENCODER2_B_PIN PORTAbits.RA5
#define ENCODER2_B_RISING IOCAPbits.IOCAP5
#define ENCODER2_B_FALLING IOCANbits.IOCAN5
#define ENCODER2_B_FLAG IOCAFbits.IOCAF5

//Encoder 2 input PB on RC2 / pin 8
#define ENCODER2_PB_TRIS TRISCbits.TRISC2
#define ENCODER2_PB_ANSEL ANSELCbits.ANSC2
#define ENCODER2_PB_PIN PORTCbits.RC2
#define ENCODER2_PB_RISING IOCCPbits.IOCCP2
#define ENCODER2_PB_FALLING IOCCNbits.IOCCN2
#define ENCODER2_PB_FLAG IOCCFbits.IOCCF2


//Global variables
uint8_t portA = 0x00;
uint8_t portC = 0x00;
uint16_t channel_1_dutycycle = 0;
uint16_t channel_2_dutycycle = 0;
uint16_t channel_3_dutycycle = 0;
uint16_t channel_4_dutycycle = 0;

//Function prototypes
static void output1_on(void);
static void output1_off(void);
static void output2_on(void);
static void output2_off(void);

//enum


/*
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

void interrupt _isr(void)
{
    //Encoder 1
    if(ENCODER1_A_FLAG)
    {
        if(!ENCODER1_B_PIN)
        {
            //++os.encoderCount;
            output1_on();
        }
        ENCODER1_A_FLAG;
    }   
    if(ENCODER1_B_FLAG)
    {
        if(!ENCODER1_A_PIN)
        {
            //--os.encoderCount;
            output1_off();
        }
        ENCODER1_B_FLAG;
    }
    //Encoder 2
    if(ENCODER2_A_FLAG)
    {
        if(!ENCODER2_B_PIN)
        {
            //++os.encoderCount;
            output2_on();
        }
        ENCODER2_A_FLAG;
    }   
    if(ENCODER2_B_FLAG)
    {
        if(!ENCODER2_A_PIN)
        {
            //--os.encoderCount;
            output2_off();
        }
        ENCODER2_B_FLAG;
    } 
}

static void pwm_init(void)
{
    
}

static void encoder_init(void)
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
    ENCODER2_A_FLAG = 0;
    ENCODER2_B_FLAG = 0;
    
    //Enable interrupts on rising edges
    ENCODER1_A_RISING = 1;
    ENCODER1_B_RISING = 1;
    ENCODER2_A_RISING = 1;
    ENCODER2_B_RISING = 1;
    
    //Enable interrupt-on-change interrupts
    PIE0bits.IOCIE = 1;
    
    //Enable peripheral interrupts
    INTCONbits.PEIE = 1;
    
    //Enable global interrupts
    INTCONbits.GIE = 1;
}

static void output1_on(void)
{
    OUTPUT1_VARIABLE |= OUTPUT1_MASK;
    OUTPUT1_PORT = OUTPUT1_VARIABLE;
}

static void output1_off(void)
{
    OUTPUT1_VARIABLE &= ~OUTPUT1_MASK;
    OUTPUT1_PORT = OUTPUT1_VARIABLE;   
}

static void output2_on()
{
    OUTPUT2_VARIABLE |= OUTPUT2_MASK;
    OUTPUT2_PORT = OUTPUT2_VARIABLE;
}

static void output2_off()
{
    OUTPUT2_VARIABLE &= ~OUTPUT2_MASK;
    OUTPUT2_PORT = OUTPUT2_VARIABLE;   
}

void init(void)
{
    //Configure pin 13 as output and drive it high immediately
    //This enables the N-FET and ensures power stays on
    ENABLE_TRIS = 0;
    ENABLE_PIN = 1;
    ENABLE_VARIABLE |= ENABLE_MASK;
    
    //Configure output pins
    OUTPUT1_TRIS = 0;
    OUTPUT2_TRIS = 0;
    OUTPUT3_TRIS = 0;
    OUTPUT4_TRIS = 0;
    
    //output1_on();
    //output2_on();
    
    //Initialize PWM
    pwm_init();
    
    //Enable encoder
    encoder_init();
    
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
            ENABLE_PIN = 0;
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

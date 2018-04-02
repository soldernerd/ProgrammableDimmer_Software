/* 
 * File:   config.h
 * Author: Luke
 *
 * Created on 15. März 2017, 00:18
 */

#ifndef CONFIG_H
#define	CONFIG_H

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

//Lock and unlock peripheral pin select
#define  PPSUnLock()    {INTCONbits.GIE = 0; PPSLOCK = 0x55; PPSLOCK = 0xAA; PPSLOCKbits.PPSLOCKED = 0; INTCONbits.GIE = 1;}
#define  PPSLock() 		{INTCONbits.GIE = 0;PPSLOCK = 0x55; PPSLOCK = 0xAA; PPSLOCKbits.PPSLOCKED = 1; INTCONbits.GIE = 1;}

//High and Low Byte macros
#define LOW_BYTE(x) ((unsigned char)((x)&0xFF))
#define HIGH_BYTE(x) ((unsigned char)(((x)>>8)&0xFF))

//Output Channel 1 on pin 12, RA1
#define OUTPUT1_TRIS TRISAbits.TRISA1
#define OUTPUT1_PIN LATAbits.LATA1
#define OUTPUT1_PPS RA1PPS

//Output Channel 2 on pin 11, RA2
#define OUTPUT2_TRIS TRISAbits.TRISA2
#define OUTPUT2_PIN LATAbits.LATA2
#define OUTPUT2_PPS RA2PPS

//Output Channel 3 on pin 10, RC0
#define OUTPUT3_TRIS TRISCbits.TRISC0
#define OUTPUT3_PIN LATCbits.LATC0
#define OUTPUT3_PPS RC0PPS

//Output Channel 4 on pin 9, RC1
#define OUTPUT4_TRIS TRISCbits.TRISC1
#define OUTPUT4_PIN LATCbits.LATC1
#define OUTPUT4_PPS RC1PPS

//CPU Frequency
#define _XTAL_FREQ 32000000

//Define the function the the dimmer should perform
//#define FUNCTION_DUALWHITE
//#define FUNCTION_RGB
//#define FUNCTION_FREQUENCY
#define FUNCTION_WARMCOOL

//Define the time until turn-off in milliseconds
#define TURN_OFF_DELAY 3000

//Define the type of encoders
#define ENCODER_PEC09_12DENT_TOP
//#define ENCODER_PEC09_24DENT_TOP
//#define ENCODER_PEC15_BOTTOM

#define SOURCE_TIMER_2 0b00
#define SOURCE_TIMER_4 0b10
#define SOURCE_TIMER_6 0b11

//Set parameters for dual white implementation
#ifdef FUNCTION_DUALWHITE
    #define OUTPUT_1_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_2_TIMER_SOURCE SOURCE_TIMER_4
    #define OUTPUT_3_TIMER_SOURCE SOURCE_TIMER_6
    #define OUTPUT_4_TIMER_SOURCE SOURCE_TIMER_2
    #define TIMER_2_STARTING_VALUE 0
    #define TIMER_4_STARTING_VALUE 128
    #define TIMER_6_STARTING_VALUE 128
    #define BRIGHTNESS_LEVEL_COUNT_1 20
    #define BRIGHTNESS_LEVEL_MIN_1 1
    #define BRIGHTNESS_LEVEL_MAX_1 (BRIGHTNESS_LEVEL_COUNT_1 - 1)
    #define BRIGHTNESS_LEVELS_1 {0,5,8,12,20,33,54,90,138,165,198,238,286,343,411,493,592,710,853,1023}
    #define BRIGHTNESS_LEVEL_COUNT_2 20
    #define BRIGHTNESS_LEVEL_MIN_2 1
    #define BRIGHTNESS_LEVEL_MAX_2 (BRIGHTNESS_LEVEL_COUNT_2 - 1)
    #define BRIGHTNESS_LEVELS_2 {0,5,8,12,20,33,54,90,138,165,198,238,286,343,411,493,592,710,853,1023}
#endif /*FUNCTION_DUALWHITE*/

//Set parameters for rgb implementation
#ifdef FUNCTION_RGB
    #define OUTPUT_1_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_2_TIMER_SOURCE SOURCE_TIMER_4
    #define OUTPUT_3_TIMER_SOURCE SOURCE_TIMER_6
    #define OUTPUT_4_TIMER_SOURCE SOURCE_TIMER_2
    #define TIMER_2_STARTING_VALUE 0
    #define TIMER_4_STARTING_VALUE 58
    #define TIMER_6_STARTING_VALUE 170
    #define NUMBER_OF_COLORS 24
    #define RGB_NEUTRAL_RED 255
    #define RGB_NEUTRAL_GREEN 90
    #define RGB_NEUTRAL_BLUE 50
    #define BRIGHTNESS_LEVEL_COUNT 32
    #define BRIGHTNESS_LEVEL_MIN 1
    #define BRIGHTNESS_LEVEL_MAX (BRIGHTNESS_LEVEL_COUNT - 1)
    #define BRIGHTNESS_LEVELS {0,11,12,14,16,19,22,25,29,34,40,47,55,64,74,87,101,118,138,161,187,219,255,298,347,405,473,552,644,751,877,1023}
    #define OUTPUT_RED PWM_CHANNEL_1
    #define OUTPUT_GREEN PWM_CHANNEL_2
    #define OUTPUT_BLUE PWM_CHANNEL_3
    //#define OUTPUT_WHITE PWM_CHANNEL_4
#endif /*FUNCTION_RGB*/

//Set parameters for variable frequency implementation
#ifdef FUNCTION_FREQUENCY
    #define OUTPUT_1_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_2_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_3_TIMER_SOURCE SOURCE_TIMER_4
    #define OUTPUT_4_TIMER_SOURCE SOURCE_TIMER_4
    #define TIMER_2_STARTING_VALUE 0
    #define TIMER_4_STARTING_VALUE 128
    //Brightness
    #define BRIGHTNESS_LEVEL_COUNT 20
    #define BRIGHTNESS_LEVEL_MIN 1
    #define BRIGHTNESS_LEVEL_MAX (BRIGHTNESS_LEVEL_COUNT - 1)
    #define BRIGHTNESS_LEVELS {0,16,20,25,32,40,51,64,81,101,128,161,203,256,322,406,512,644,812,1023}
    //Frequency
    #define FREQUENCY_LEVEL_COUNT 25
    #define FREQUENCY_LEVEL_MIN 0
    #define FREQUENCY_LEVEL_MAX (FREQUENCY_LEVEL_COUNT - 1)
    #define FREQUENCY_LEVELS {57723, 60328, 61630, 62411, 62932, 63304, 63583, 63800, 63973, 64116, 64234, 64334, 64420, 64494, 64559, 64617, 64668, 64714, 64755, 64792, 64826, 64857, 64885, 64911, 64935}
#endif /*FUNCTION_FREQUENCY*/

//Set parameters for warm white / cold white implementation
#ifdef FUNCTION_WARMCOOL
    #define OUTPUT_1_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_2_TIMER_SOURCE SOURCE_TIMER_2
    #define OUTPUT_3_TIMER_SOURCE SOURCE_TIMER_4
    #define OUTPUT_4_TIMER_SOURCE SOURCE_TIMER_4
    #define TIMER_2_STARTING_VALUE 0
    #define TIMER_4_STARTING_VALUE 128
    #define BRIGHTNESS_LEVEL_COUNT 20
    #define BRIGHTNESS_LEVEL_MIN 1
    #define BRIGHTNESS_LEVEL_MAX (BRIGHTNESS_LEVEL_COUNT - 1)
    #define BRIGHTNESS_LEVELS {0,5,8,12,20,33,54,90,138,165,198,238,286,343,411,493,592,710,853,1023}
    #define COLOR_LEVEL_COUNT 17
    #define COLOR_LEVEL_MIN 0
    #define COLOR_LEVEL_MAX (COLOR_LEVEL_COUNT - 1)
    #define COLOR_LEVELS {0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x100}
    #define COLOR_LEVEL_NEUTRAL 8
#endif /*FUNCTION_WARMCOOL*/


#endif	/* CONFIG_H */


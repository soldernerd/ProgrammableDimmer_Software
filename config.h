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

//CPU Frequency
#define _XTAL_FREQ 32000000

//Define the function the the dimmer should perform
//#define FUNCTION_DUALWHITE
#define FUNCTION_RGB

//Define the time until turn-off in milliseconds
#define TURN_OFF_DELAY 3000

//Define the type of encoders
//#define ENCODER_PEC09_TOP
#define ENCODER_PEC15_BOTTOM

//Set parameters for dual white implementation
#ifdef FUNCTION_DUALWHITE
    #define TIMER_2_STARTING_VALUE 0
    #define TIMER_4_STARTING_VALUE 128
    #define TIMER_6_STARTING_VALUE 128
    #define BRIGHTNESS_LEVEL_COUNT_1 16
    #define BRIGHTNESS_LEVEL_MIN_1 1
    #define BRIGHTNESS_LEVEL_MAX_1 (BRIGHTNESS_LEVEL_COUNT_1 - 1)
    #define BRIGHTNESS_LEVELS_1 {0,5,41,90,138,165,198,238,286,343,411,493,592,710,853,1023}
    #define BRIGHTNESS_LEVEL_COUNT_2 16
    #define BRIGHTNESS_LEVEL_MIN_2 1
    #define BRIGHTNESS_LEVEL_MAX_2 (BRIGHTNESS_LEVEL_COUNT_2 - 1)
    #define BRIGHTNESS_LEVELS_2 {0,5,41,90,138,165,198,238,286,343,411,493,592,710,853,1023}
#endif

//Set parameters for rgb implementation
#ifdef FUNCTION_RGB
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

#endif


#endif	/* CONFIG_H */


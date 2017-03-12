/* 
 * File:   pwm.h
 * Author: Luke
 *
 * Created on 12. März 2017, 21:08
 */

#ifndef PWM_H
#define	PWM_H

typedef enum
{
	PWM_CHANNEL_1,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
    PWM_CHANNEL_4
} PwmChannel_t;

//Output Channel 1 on pin 12, RA1
#define OUTPUT1_TRIS TRISAbits.TRISA1
#define OUTPUT1_PIN PORTAbits.RA1
#define OUTPUT1_PORT PORTA
#define OUTPUT1_VARIABLE portA
#define OUTPUT1_MASK 0b00000010
#define OUTPUT1_PPS RA1PPS

//Output Channel 2 on pin 11, RA2
#define OUTPUT2_TRIS TRISAbits.TRISA2
#define OUTPUT2_PIN PORTAbits.RA2
#define OUTPUT2_PORT PORTA
#define OUTPUT2_VARIABLE portA
#define OUTPUT2_MASK 0b00000100
#define OUTPUT2_PPS RA2PPS

//Output Channel 3 on pin 10, RC0
#define OUTPUT3_TRIS TRISCbits.TRISC0
#define OUTPUT3_PIN PORTCbits.RC0
#define OUTPUT3_PORT PORTC
#define OUTPUT3_VARIABLE portC
#define OUTPUT3_MASK 0b00000001
#define OUTPUT3_PPS RC0PPS

//Output Channel 4 on pin 9, RC1
#define OUTPUT4_TRIS TRISCbits.TRISC1
#define OUTPUT4_PIN PORTCbits.RC1
#define OUTPUT4_PORT PORTC
#define OUTPUT4_VARIABLE portC
#define OUTPUT4_MASK 0b00000001
#define OUTPUT4_PPS RC1PPS

//CCP Modules
#define CCP1_MODULE 0b01100
#define CCP2_MODULE 0b01101
#define CCP3_MODULE 0b01110
#define CCP4_MODULE 0b01111

//Some constants
#define PWM_DUTYCYCLE_MINIMUM 0
#define PWM_DUTYCYCLE_MAXIMUM 1023


void pwm_init(void);

uint16_t pwm_get_dutycycle(PwmChannel_t channel);
void pwm_set_dutycycle(PwmChannel_t channel, uint16_t dutycycle);
void pwm_change_dutycycle(PwmChannel_t channel, int16_t change_amount);


#endif	/* PWM_H */


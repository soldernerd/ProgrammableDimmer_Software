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

//CCP Modules
#define CCP1_MODULE 0b01100
#define CCP2_MODULE 0b01101
#define CCP3_MODULE 0b01110
#define CCP4_MODULE 0b01111
#define OUTPUT_LATCH 0b0000

//Some constants
#define PWM_DUTYCYCLE_MINIMUM 0
#define PWM_DUTYCYCLE_MAXIMUM 1023


void pwm_init(void);
void pwm_start(void);
void pwm_stop(void);

uint16_t pwm_get_dutycycle(PwmChannel_t channel);
void pwm_set_dutycycle(PwmChannel_t channel, uint16_t dutycycle);
void pwm_change_dutycycle(PwmChannel_t channel, int16_t change_amount);


#endif	/* PWM_H */


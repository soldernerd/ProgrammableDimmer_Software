/* 
 * File:   function_frequency.h
 * Author: luke
 *
 * Created on 20. Februar 2018, 20:36
 */

#ifndef FUNCTION_FREQUENCY_H
#define	FUNCTION_FREQUENCY_H

#define EEPROM_BRIGHTNESS_ADDRESS 0x00
#define EEPROM_FREQUENCY_ADDRESS 0x01

void main_loop_frequency(void);

//Function prototypes
void turn_on(void);
void turn_off(void);

#endif	/* FUNCTION_FREQUENCY_H */


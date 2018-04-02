/* 
 * File:   function_warmcool.h
 * Author: Luke
 *
 * Created on 02. April 2018, 18:53
 */

#ifndef FUNCTION_WARMCOOL_H
#define	FUNCTION_WARMCOOL_H

#define EEPROM_BRIGHTNESS_ADDRESS 0x00
#define EEPROM_COLOR_ADDRESS 0x01

void main_loop_warmcool(void);

//Function prototypes
void turn_on(void);
void turn_off(void);

#endif	/* FUNCTION_WARMCOOL_H */


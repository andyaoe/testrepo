/*
 * str_utils.h
 *
 *  Created on: Apr 29, 2014
 *      Author: chadi
 */

#ifndef STR_UTILS_H_
#define STR_UTILS_H_

#include "common.h"

#include "stdint.h"

global char *str_uint8_to_ahex(char *destp, uint8_t value);
global char *str_uint16_to_ahex(char *destp, uint16_t value);

global void swap(char *start, char *end);
global void reverse(char *str, uint16_t length); /* A utility function to reverse a string  */
global void reverse_str(uint8_t *str);
//global uint8_t* itoa(int16_t num, uint8_t* str, uint8_t base);// Implementation of itoa()
global char* itoa(int num, char *str, int base);
//global char *ltoa(long num, char *str, int radix);
global char *ultoa(unsigned long num, char *str, int radix);

global void itob(int16_t num, uint8_t *str, uint8_t base);
//global int32_t atol(char *str);

#endif /* STR_UTILS_H_ */

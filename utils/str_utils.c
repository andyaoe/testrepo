/*
 * str_utils.c
 *
 *  Created on: Apr 29, 2014
 *      Author: chadi
 */

#include "str_utils.h"
#include "stdbool.h"
#include "string.h"
#include "stdint.h"

/*************
 *
 * Name: str_uint8_to_ahex
 *
 * Purpose: Converts a uint8_t value to a 0 padded
 *          character array representation of the
 *          value in ASCII in hex with no null
 *          termination.
 *
 * Parms:   destp - pointer to memory location
 *                  where the string conversion
 *                  should be placed.
 *          value - value to be converted
 *
 * Return:  destp
 *
 * Abort:   None (No fatal errors)
 *
 * Notes:   Does not include null termination, so
 *          calling function must ensure destp buffer
 *          is at least 2 bytes big.
 *
 **************/
global char *str_uint8_to_ahex(char *destp, uint8_t value)
{
	uint8_t lsn;         /* least significant nibble */
	uint8_t msn;         /* most significant nible */
    char *indexp;   	 /* index pointer into destp */

    indexp = destp;
    msn = (value & 0xF0) >> 4; /* mask and shift MSB */
    if (msn <= 9)
    {
        *indexp++ = msn + '0';  /* '0'-'9' value */
    }
    else
    {
        *indexp++ = msn + 'A' - 10;  /* 'A'-'F' value */
    }

    lsn = value & 0x0F; /* mask off LSB */

    if (lsn <= 9)
    {
        *indexp++ = lsn + '0';  /* '0'-'9' value */
    }
    else
    {
        *indexp++ = lsn + 'A' - 10;  /* 'A'-'F' value */
    }

    return destp;
}

/*************
 *
 * Name: str_uint16_to_ahex
 *
 * Purpose: Converts a uint16_t value to a 0 padded
 *          string representation of the value in
 *          ASCII in hex and then Null terminate the
 *          string.
 *
 * Parms:   destp - pointer to memory location
 *                  where the string conversion
 *                  should be placed.
 *          value - value to be converted
 *
 * Return:  destp
 *
 * Abort:   None (No fatal errors)
 *
 * Notes:   Includes null termination, so calling
 *          function must ensure destp buffer is at
 *          least 5 bytes big.
 *
 **************/
global char *str_uint16_to_ahex(char *destp, uint16_t value)
{
    uint8_t lsb; /* least significant byte */
    uint8_t msb; /* most significant byte */

    /* calculate MSB & LSB */
    msb = (value & 0xFF00) >> 8;
    lsb = (value & 0x00FF);

    /* convert MSB & LSB */
    str_uint8_to_ahex(destp, msb);
    str_uint8_to_ahex(destp+2, lsb);

    *(destp+4) = 0; /* null-terminate string */

    return destp;
}


global void swap(char* start,char* end)
{
	uint8_t temp = *start;
	*start = *end;
	*end = temp;
}

/* A utility function to reverse a string  */
global void reverse(char *str, uint16_t length)
{
	uint16_t start = 0;
	uint16_t end = length -1;
    while (start < end)
    {
        swap((str+start), (str+end));
        start++;
        end--;
    }
}

// Implementation of itoa()
//global uint8_t* itoa(int16_t num, uint8_t *str, uint8_t base)
global char* itoa(int num, char *str, int base)
{
	uint16_t i = 0;
    bool isNegative = false;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
    	int16_t rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

/*
global char *ltoa(long num, char *str, int radix)
{
  char sign = 0;
  char temp[33]; //an int can only be 32 bits long

  //at radix 2 (binary) the string
  //is at most 16 + 1 null long.
  int temp_loc = 0;
  int digit;
  int str_loc = 0;

  //save sign for radix 10 conversion
  if (radix == 10 && num < 0) {
    sign = 1;
    num = -num;
  }

  //construct a backward string of the number.
  do {
    digit = (unsigned int)num % radix;
    if (digit < 10) 
      temp[temp_loc++] = digit + '0';
    else
      temp[temp_loc++] = digit - 10 + 'A';
    
    num = ((unsigned int)num) / radix;
    
  } while ((unsigned int)num > 0);

  //now add the sign for radix 10
  if (radix == 10 && sign) {
    temp[temp_loc] = '-';
  } else {
    temp_loc--;
  }

  //now reverse the string.
  while ( temp_loc >=0 ) {// while there are still chars
    str[str_loc++] = temp[temp_loc--]; 
  }
  str[str_loc] = 0; // add null termination.

  return str;
}
*/


global char *ultoa(unsigned long num, char *str, int radix) {
	char temp[33]; //an int can only be 16 bits long
	//at radix 2 (binary) the string
	//is at most 16 + 1 null long.
	int temp_loc = 0;
	int digit;
	int str_loc = 0;

	//construct a backward string of the number.
	do {
		digit = (unsigned int) num % radix;
		if (digit < 10)
			temp[temp_loc++] = digit + '0';
		else
			temp[temp_loc++] = digit - 10 + 'A';
		num = ((unsigned int)num) / radix;
	} while ((unsigned int) num > 0);

	temp_loc--;

	//now reverse the string.
	while (temp_loc >= 0) { // while there are still chars
		str[str_loc++] = temp[temp_loc--];
	}
	str[str_loc] = 0; // add null termination.

	return str;
}

/* ATOL: CONVERT A STRING OF DIGITS TO A LONG INTEGER  */
/*
global int32_t atol(char *str)
{
	uint8_t error = 0;
	int16_t sign = 1;
    int32_t dec = 0;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (error == 0 && *str != '\0')
        if (*str >= '0' && *str <= '9')
            dec = 10 * dec + (*str++ - '0');
        else {
        	error = 1; //INVALID DECIMAL NUMBER
        }
    return sign * dec;
}

*/

/* ITOB: CONVERT num TO BASE base CHARACTERS IN str  */
global void itob(int16_t num, uint8_t *str, uint8_t base)
{
	int16_t ch;
	int16_t sign = (num < 0) ? -1 : 1;

    do {
       *str++ = ((ch = sign * (num % base)) < 10)  ?  ch + '0' : ch - 10 + 'A';
    } while ((num /= base) != 0);
    if (sign < 0)
        *str++ = '-';
    *str = '\0';
    reverse_str(str);
}

/* REVERSE: REVERSE STRING s IN PLACE using pointers */
global void reverse_str(uint8_t *str)
{
	int16_t ch;
	uint8_t *str_temp;

    for (str_temp = str + (strlen((char *)str) - 1); str < str_temp; str++, str_temp--) {
        ch = *str;
        *str = *str_temp;
        *str_temp = ch;
    }
}

#include <stdio.h>
#include <inttypes.h>
#include "NetworkHeader.h"

const char* byte_to_binary(uint8_t x, char* binary);

/**
* Takes as a parameter a pointer to the first of 2 bytes of a field specifying length.
*
* Shifts the first byte 8 orders of magnitude to the left and adds it to the value
* of the bit on the right
**/
unsigned int getLength(char* field)
{
	char firstBin[17]; char secondBin[9];
	byte_to_binary(field[0], firstBin);
	byte_to_binary(field[1], secondBin);
	strcat(firstBin, secondBin);
	return strtoul(firstBin, NULL, 2);
/*
  uint8_t field0, field1;
  field0 = field[0];
  field1 = field[1];
  int result = (field0 << 8) | field1;
  return (unsigned int) result;
	//return (unsigned int) ( (field[0] << 8) + field[1] );*/
}


/**
* Takes as a parameter a pointer to the first of 2 bytes of a field specifying length.
*
* Shifts the first byte 8 orders of magnitude to the left and adds it to the value
* of the bit on the right
**/
void convertLengthTo2Bytes(char* ptr, unsigned long length)
{
	ptr[1] = (uint16_t)length;
	ptr[0] = (uint16_t)length >> 8;
/*
	// get the rightmost 8 bits of the length field
	char byte2 = (char) length % 256;

	// get the next 8 bits of the length field
	char byte1 = (char) length / 256;

	ptr[0] = byte1;
	ptr[1] = byte2;*/

}




/* testing code
main() 
{
	char field[2];
	field[0] = 127;
	field[1] = 5;
	unsigned int a = getLength(field);
	printf("%u\n", getLength(field));
}*/

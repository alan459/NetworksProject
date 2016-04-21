#include "NetworkHeader.h"

const char* byte_to_binary(uint8_t x, char* binary);

// given a packet, looks at the length field (4-5th bytes) and returns
// its corresponding decimal value as unsigned long
unsigned long retrieveLength(char* packet)
{
		char firstBin[17]; char secondBin[9];
		byte_to_binary(packet[4], firstBin);
		byte_to_binary(packet[5], secondBin);
		strcat(firstBin, secondBin);
		return strtoul(firstBin, NULL, 2);
}

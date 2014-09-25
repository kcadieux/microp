#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BYTES_IN_CRC 2
#define BITS_PER_BYTE 8
#define POLYNOM 0x8408
#define SUCCESS 0;
#define FAILURE 1;

typedef signed long int32_t;

unsigned short crc_table[256];

extern unsigned short CrcAssVersion(char * characters, int stringLength);

/// <summary> 
/// Generates a CRC for the given array of characters
/// </summary>
/// <param_name="characters">
/// The initial array of characters
/// </param>
/// <param_name="stringLength">
/// The length of the string
/// </param>
/// <returns>
/// The CRC as an unsigned short (2 bytes)
/// </returns>
unsigned short CrcCVersion(char * characters, int stringLength)
{	
	if (stringLength <= 0)
	{
		return 0; //All your bases are belong to me
	}
	
	unsigned short rem = 0;
	int charSelected;
	int bitSelected;
	for(charSelected = 0; charSelected < stringLength; charSelected++)
	{
		rem = rem^characters[charSelected];
		for (bitSelected = 0; bitSelected < BITS_PER_BYTE; bitSelected++)
		{
			if (rem&0x0001)
			{
				rem = (rem>>1)^POLYNOM;
			}
			else
			{
				rem = rem>>1;
			}
		}
	}
	
	return rem;
}

char * AppendCharsToString(char * string, int stringLength, char c1, char c2)
{
	char * newConcat = (char *) malloc(stringLength+BYTES_IN_CRC+1);
	memcpy(newConcat, string, sizeof(char)*(stringLength));
	newConcat[stringLength] = c1;
	newConcat[stringLength+1] = c2;
	return newConcat;
}

/// <summary> 
/// Checks if the given CRC is consistent with the initial string 
/// </summary>
/// <param_name="initialString">
/// The initial string
/// </param>
/// <param_name="stringLength">
/// The length of the string
/// </param>
/// <param_name="CrcFunction">
/// The pointer to the CRC generation function to test
/// </param>
/// <returns>
/// SUCCESS if the CRC is valid, FAILURE otherwise
/// </returns>
int CrcCheck(char * initialString, int stringLength, unsigned short (*CrcFunction)(char *,int))
{	
	unsigned short crc;
	crc = CrcFunction(initialString, stringLength);
	char first = (char)crc;
	char second = (char)(crc>>BITS_PER_BYTE);
	
	char * stringAndCrc = AppendCharsToString(initialString, stringLength, first, second);
		
	if(CrcFunction(stringAndCrc, stringLength + BYTES_IN_CRC)) 
	{
		return FAILURE;
	}
	
	return SUCCESS;
}

/// <summary> 
/// Code entry point
/// </summary>
/// <returns>
/// Zero
/// </returns>
int main()
{	
	char * characters = "2014";//"In Flanders fields the poppies blow, Between the crosses, row on row, That mark our place; and in the sky, The larks, still bravely singing, fly, Scarce heard amid the guns below. We are the Dead. Short days ago, We lived, felt dawn, saw sunset glow, Loved and were loved, and now we lie, In Flanders fields. Take up our quarrel with the foe, To you from failing hands we throw, The torch; be yours to hold it high. If ye break faith with us who die, We shall not sleep, though poppies grow, In Flanders fields.";
	int stringLength = 4;	
	int resultC = CrcCheck(characters, stringLength, CrcCVersion);
	int resultAss = CrcCheck(characters, stringLength, CrcAssVersion);
	return 0;
}


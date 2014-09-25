#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_IN_CRC 2
#define BITS_PER_BYTE 8
#define POLYNOM 0x8408
#define SUCCESS 0;
#define FAILURE 1;

typedef signed long int32_t;

extern unsigned short CrcAssVersion(unsigned short crc, char * characters, int stringLength);

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
unsigned short CrcCVersion(unsigned short rem, char * characters, int stringLength)
{	
	if (stringLength <= 0)
	{
		return 0; //All your bases are belong to me
	}
	
	rem = 0;
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
int CrcCheck(char * initialString, int stringLength, unsigned short (*CrcFunction)(unsigned short, char *,int))
{	
	unsigned short crc;
	crc = CrcFunction(crc, initialString, stringLength);
	char first = (char)crc;
	char second = (char)(crc>>BITS_PER_BYTE);
	
	char stringAndCrc[stringLength + BYTES_IN_CRC + 1]; // The 1 is due to the fact that the string must be null terminated
	snprintf(stringAndCrc, sizeof stringAndCrc, "%s%c%c", initialString, first, second);
	unsigned short zz;
	zz = CrcFunction(crc, stringAndCrc, stringLength + BYTES_IN_CRC);
	// If the CRC of the concat'd string isn't 0, it's a failure	
	if(CrcFunction(crc, stringAndCrc, stringLength + BYTES_IN_CRC)) 
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
	char * characters = "2014";	
	int stringLength = 4;	
	int resultC = CrcCheck(characters, stringLength, CrcCVersion);
	int resultAss = CrcCheck(characters, stringLength, CrcAssVersion);
	unsigned short crc = 0;
	CrcAssVersion(crc, characters, stringLength);
	

	return 0;
}


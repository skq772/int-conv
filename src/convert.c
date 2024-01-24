#include "convert.h"
#include <stdlib.h>

char* integerToSMBinary(u64 number, u8 desired_size)
{
	// Set the variables.
	u8 index = 64, is_negative = (number & 1ull<<63) != 0;
	// If given number is negative, convert it to be positive (it could be just multiplied by -1).
	if (is_negative)
		number = 1 + ~number;
	// Search for the minimum C string length and test if the number fits to it, and check if desired_size is correct.
	while (!(number & 1ull<<--index) && index);
	if (desired_size < 2 || desired_size > 64 || ++index >= desired_size)
		return 0;
	// Allocate the C string.
	char* result = (char*)malloc(desired_size + 1);
	// Set its sign.
	*result = is_negative + '0';
	// Set the variables so they can be used to read the number bit by bit and written to the result.
	--desired_size;
	index = 1;
	// Iteration through the number.
	do
		result[index++] = ((number & 1ull<<--desired_size) > 0) + '0';
	while (desired_size);
	// Terminate the string and return it.
	result[index] = 0;
	return result;
}

char* integerToOCBinary(u64 number, u8 desired_size)
{
	// Set the variables.
	u8 index = 64, is_negative = (number & 1ull<<63) != 0;
	// If given number is negative, convert it to be in one's completion.
	if (is_negative)
		--number;
	// Search for the minimum C string length and test if the number fits to it, and check if desired_size is correct.
	while (!(is_negative^((number & (1ull<<--index)) != 0)) && index);
	if (desired_size < 2 || desired_size > 64 || ++index >= desired_size)
		return 0;
	// Allocate the C string.
	char* result = (char*)malloc(desired_size + 1);
	// Reuse the index variable and iterate through the number bit by bit.
	index = 0;
	do
		result[index++] = ((number & 1ull<<--desired_size) > 0) + '0';
	while (desired_size);
	// Terminate the string and return it.
	result[index] = 0;
	return result;
}

char* integerToTCBinary(u64 number, u8 desired_size)
{
	// Set the variables.
	u8 index = 64, is_negative = (number & 1ull<<63) != 0;
	// Convertion is not needed, because machines actually use two's completion method to store integers.
	// Search for the minimum C string length and test if the number fits to it, and check if desired_size is correct.
	while (!(is_negative^((number & (1ull<<--index)) != 0)) && index);
	if (desired_size < 2 || desired_size > 64 || ++index >= desired_size)
		return 0;
	// Allocate the C string.
	char* result = (char*)malloc(desired_size + 1);
	// Reuse the index variable and iterate through the number bit by bit.
	index = 0;
	do
		result[index++] = ((number & 1ull<<--desired_size) > 0) + '0';
	while (desired_size);
	// Terminate the string and return it.
	result[index] = 0;
	return result;
}

u8 SMBinaryToInteger(char* number, u8 maximum_length, u64* result)
{
	// Test if all parameters are correct.
	if (!number || !result || maximum_length < 2 || maximum_length > 64)
		return 0;
	// Iterate through the number and test if it is not containing illegal characters.
	u8 length = -1;
	while (number[++length])
		if ((number[length] != '0' && number[length] != '1') || length >= maximum_length)
			return 0;
	// Set the variables and rewrite bit by bit to the result.
	*result = 0;
	maximum_length = 0;
	--length;
	while (length--)
		if (number[++maximum_length] == '1')
			*result += 1ull<<length;
	// If given number is negative, convert it to be positive (it could be just multiplied by -1) and return true.
	if (*number == '1')
		*result = 1 + ~*result;
	return 1;
}

u8 OCBinaryToInteger(char* number, u8 maximum_length, u64* result)
{
	// Test if all parameters are correct.
	if (!number || !result || maximum_length < 2 || maximum_length > 64)
		return 0;
	// Iterate through the number and test if it is not containing illegal characters.
	u8 length = -1;
	while (number[++length])
		if ((number[length] != '0' && number[length] != '1') || length >= maximum_length)
			return 0;
	// Set the variables and write bit by bit (inverted if number is negative).
	*result = 0;
	maximum_length = 0;
	--length;
	while (length--)
		if ((number[++maximum_length] - '0')^(*number - '0'))
			*result += 1ull<<length;
	// If given number is negative, convert it to be positive (it could be just multiplied by -1) and return true.
	if (*number == '1')
		*result = 1 + ~*result;
	return 1;
}

u8 TCBinaryToInteger(char* number, u8 maximum_length, u64* result)
{
	// Test if all parameters are correct.
	if (!number || !result || maximum_length < 2 || maximum_length > 64)
		return 0;
	// Iterate through the number and test if it is not containing illegal characters.
	u8 length = -1;
	while (number[++length])
		if ((number[length] != '0' && number[length] != '1') || length >= maximum_length)
			return 0;
	// Set the variables and write bit by bit (inverted if number is negative).
	*result = 0;
	maximum_length = 0;
	--length;
	while (length--)
		if ((number[++maximum_length] - '0')^(*number - '0'))
			*result += 1ull<<length;
	// If given number is negative, convert it to be positive and return true.
	if (*number == '1')
		*result = ~*result;
	return 1;
}

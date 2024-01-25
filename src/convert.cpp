#include "convert.h"

namespace convert
{
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
		char* result = new char[desired_size + 1];
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
		char* result = new char[desired_size + 1];
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
		char* result = new char[desired_size + 1];
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

	// The beginning of definition of the weird stuff that may (I do not know though) help handle endianness of floats and doubles.
	static u8 isEndiannessOfIntAndFloatDifferent()
	{
		f32 f = -1.0;
		return !((*(u32*)&f>>31)&0x1);
	}

	static u8 isEndiannessOfIntAndDoubleDifferent()
	{
		f64 d = -1.0;
		return !((*(u64*)&d>>63)&0x1);
	}

	static const u8 is_endianness_of_int_and_float_different = isEndiannessOfIntAndFloatDifferent();
	static const u8 is_endianness_of_int_and_double_different = isEndiannessOfIntAndDoubleDifferent();
	// End of definition of the weird stuff. The constants appear in float/double to binary and vice versa convertion functions.

	char* floatToBinary(f32 number)
	{
		// Allocate the C string.
		char* result = new char[33];
		// Convert float to u32 and do swap-byte operation if needed.
		u32 converted = *(u32*)&number;
		if (is_endianness_of_int_and_float_different)
			converted = ((converted<<24)&0xff000000) | 
						((converted<<8)&0xff0000) | 
						((converted>>8)&0xff00) | 
						((converted>>24)&0xff);
		// Iterate through float and rewrite bit by bit to the C string.
		u8 index = 31;
		do
			result[31-index] = ((converted & 1<<index) != 0) + '0';
		while (index--);
		// Terminate C string and return it.
		result[32] = 0;
		return result;
	}

	char* doubleToBinary(f64 number)
	{
		// Allocate the C string.
		char* result = new char[65];
		// Convert float to u64 and do swap-byte operation if needed.
		u64 converted = *(u64*)&number;
		if (is_endianness_of_int_and_double_different)
			converted = ((converted<<56)&0xff00000000000000) | 
						((converted<<40)&0xff000000000000) | 
						((converted<<24)&0xff0000000000) | 
						((converted<<8)&0xff00000000) | 
						((converted>>8)&0xff000000) | 
						((converted>>24)&0xff0000) | 
						((converted>>40)&0xff00) | 
						((converted>>56)&0xff);
		// Iterate through double and rewrite bit by bit to the C string.
		u8 index = 63;
		do
			result[63-index] = ((converted & 1ull<<index) != 0) + '0';
		while (index--);
		// Terminate C string and return it.
		result[64] = 0;
		return result;
	}

	u8 binaryToFloat(char *number, f32 *result)
	{
		// Test C string if it is correct.
		u8 index = -1;
		while (number[++index])
			if ((number[index] != '0' && number[index] != '1') || index >= 32)
				return 0;
		if (index != 32)
			return 0;
		// Convert pointer to float to pointer to u32 and rewrite from C string.
		u32* target = (u32*)result;
		*target = 0;
		while (index--)
			if (number[index] == '1')
				*target += 1<<(31-index);
		// Do swap-byte operation if needed and return true.
		if (is_endianness_of_int_and_float_different)
			*target  =  ((*target<<24)&0xff000000) | 
						((*target<<8)&0xff0000) | 
						((*target>>8)&0xff00) | 
						((*target>>24)&0xff);
		return 1;
	}

	u8 binaryToDouble(char *number, f64 *result)
	{
		// Test C string if it is correct.
		u8 index = -1;
		while (number[++index])
			if ((number[index] != '0' && number[index] != '1') || index >= 64)
				return 0;
		if (index != 64)
			return 0;
		// Convert pointer to double to pointer to u64 and rewrite from C string.
		u64* target = (u64*)result;
		*target = 0;
		while (index--)
			if (number[index] == '1')
				*target += 1ull<<(63-index);
		// Do swap-byte operation if needed and return true.
		if (is_endianness_of_int_and_double_different)
			*target  =  ((*target<<56)&0xff00000000000000) | 
						((*target<<40)&0xff000000000000) | 
						((*target<<24)&0xff0000000000) | 
						((*target<<8)&0xff00000000) | 
						((*target>>8)&0xff000000) | 
						((*target>>24)&0xff0000) | 
						((*target>>40)&0xff00) | 
						((*target>>56)&0xff);
		return 1;
	}
}

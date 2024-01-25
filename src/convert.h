// Big-endian number convertion functions written in C.
// Note: character arrays are allocated using the C++ new operator and they are not modified or freed by the functions.

#ifndef _CPP_CONVERT_H_
#define _CPP_CONVERT_H_

#include "stdvar.h"

namespace convert
{
    // Convert signed integer to sign-magnitude binary represented in C string.
    // Parameters:
    // - u64 number - integer to convert;
    // - u8 desired_size - desired C string length in bits (2 - 64).
    // Return values:
    // - pointer to converted number represented in C string on success;
    // - null pointer on fail.
    // Errors:
    // - desired size is less than 2 or greater than 64;
    // - converted number cannot fit to C string with desired size.
    char* integerToSMBinary(u64 number, u8 desired_size);

    // Convert signed integer to one's complement binary represented in C string.
    // Parameters:
    // - u64 number - integer to convert;
    // - u8 desired_size - desired C string length in bits (2 - 64).
    // Return values:
    // - pointer to converted number represented in C string on success;
    // - null pointer on fail.
    // Errors:
    // - desired size is less than 2 or greater than 64;
    // - converted number cannot fit to C string with desired size.
    char* integerToOCBinary(u64 number, u8 desired_size);

    // Convert signed integer to two's complement binary represented in C string.
    // Parameters:
    // - u64 number - integer to convert;
    // - u8 desired_size - desired C string length in bits (2 - 64).
    // Return values:
    // - pointer to converted number represented in C string on success;
    // - null pointer on fail.
    // Errors:
    // - desired size is less than 2 or greater than 64;
    // - converted number cannot fit to C string with desired size.
    char* integerToTCBinary(u64 number, u8 desired_size);

    // Convert sign-magnitude binary represented in C string to signed integer.
    // Parameters:
    // - char* number - binary represented in C string;
    // - u8 maximum_size - the maximum size the C string may have;
    // - u64* result - pointer to the target variable.
    // Return values:
    // - true on success;
    // - false on fail.
    // Errors:
    // - the number is a null pointer;
    // - given number has characters other than 0, '0' and '1';
    // - given maximum length is less than 2 and greater than 64;
    // - C string is too long;
    // - the result is a null pointer.
    // Note:
    // - given number is not modified by this function.
    u8 SMBinaryToInteger(char* number, u8 maximum_length, u64* result);

    // Convert one's completion binary represented in C string to signed integer.
    // Parameters:
    // - char* number - binary represented in C string;
    // - u8 maximum_size - the maximum size the C string may have;
    // - u64* result - pointer to the target variable.
    // Return values:
    // - true on success;
    // - false on fail.
    // Errors:
    // - the number is a null pointer;
    // - given number has characters other than 0, '0' and '1';
    // - given maximum length is less than 2 and greater than 64;
    // - C string is too long;
    // - the result is a null pointer.
    // Note:
    // - given number is not modified by this function.
    u8 OCBinaryToInteger(char* number, u8 maximum_length, u64* result);

    // Convert two's completion binary represented in C string to signed integer.
    // Parameters:
    // - char* number - binary represented in C string;
    // - u8 maximum_size - the maximum size the C string may have;
    // - u64* result - pointer to the target variable.
    // Return values:
    // - true on success;
    // - false on fail.
    // Errors:
    // - the number is a null pointer;
    // - given number has characters other than 0, '0' and '1';
    // - given maximum length is less than 2 and greater than 64;
    // - C string is too long;
    // - the result is a null pointer.
    // Note:
    // - given number is not modified by this function.
    u8 TCBinaryToInteger(char* number, u8 maximum_length, u64* result);

    // Convert float to binary in C string.
    // Parameters:
    // - f32 number - float to be converted.
    // Return value:
    // - pointer to binary representations of the number in C string.
    char* floatToBinary(f32 number);

	// Convert double to binary in C string.
	// Parameters:
	// - f64 number - double to be converted.
	// Return value:
	// - pointer to binary representation of the number in C string.
	char* doubleToBinary(f64 number);

    // Convert binary to float.
    // Parameters:
    // - char* number - C string with float represented in binary;
    // - f32* result - pointer to variable that is a result of the convertion.
    // Return values:
    // - true on success;
    // - false on fail.
    // Errors:
    // - the number is a null pointer;
    // - given number has characters other than 0, '0' and '1';
    // - length of the C string is not equal to 32;
    // - the result is a null pointer.
	u8 binaryToFloat(char* number, f32* result);

    // Convert binary to double.
    // Parameters:
    // - char* number - C string with double represented in binary;
    // - f64* result - pointer to variable that is a result of the convertion.
    // Return values:
    // - true on success;
    // - false on fail.
    // Errors:
    // - the number is a null pointer;
    // - given number has characters other than 0, '0' and '1';
    // - length of the C string is not equal to 64;
    // - the result is a null pointer.
	u8 binaryToDouble(char* number, f64* result);
}
#endif
